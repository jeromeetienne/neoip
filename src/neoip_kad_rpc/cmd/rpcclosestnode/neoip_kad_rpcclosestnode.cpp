/*! \file
    \brief Definition of the \ref kad_rpcclosestnode_t class

*/

/* system include */
/* local include */
#include "neoip_kad_rpcclosestnode.hpp"
#include "neoip_kad_rpcpeer.hpp"
#include "neoip_kad_rpcpeer_id.hpp"
#include "neoip_kad_rpclistener.hpp"
#include "neoip_kad_rpcresp.hpp"
#include "neoip_kad.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_rpcclosestnode_t::kad_rpcclosestnode_t()		throw()
{
	// zero some field
	m_rpcpeer	= NULL;
	m_kad_closestnode	= NULL;
	httpd_request	= NULL;
}

/** \brief Destructor
 */
kad_rpcclosestnode_t::~kad_rpcclosestnode_t()		throw()
{
	// if httpd_request is set, the http reply has not yet been sent, notify an error now
	if( httpd_request ){
		kad_rpcresp_t *	rpcresp	= rpcpeer()->rpclistener()->rpcresp();
		// notify the http error to the caller
		// - i dunno which one, so i sent httpd_err_t::INTERNAL_ERROR which seems the closest...
		rpcresp->notify_httpd_reply(*httpd_request, httpd_err_t::INTERNAL_ERROR);
		// delete the httpd_request_t if needed
		nipmem_zdelete	httpd_request;
	}
	// unlink this object from the kad_rpclistener_t if needed
	if( m_rpcpeer )	rpcpeer()->rpcclosestnode_unlink(this);
	// delete the kad_closestnode_t if needed
	nipmem_zdelete	m_kad_closestnode;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operations
 */
kad_err_t	kad_rpcclosestnode_t::start(kad_rpcresp_t *kad_rpcresp, const httpd_request_t &recved_request
							, const std::string &method_name
							, xmlrpc_parse_t &xmlrpc_parse)	throw()
{
	kad_rpcpeer_id_t	rpcpeer_id;	
	kad_targetid_t		targetid;
	size_t			max_nb_node;
	delay_t			expire_delay;
	kad_err_t		kad_err;
	// log to debug
	KLOG_ERR("enter");

	// parse the xmlrpc_parse_t to get the rpc parameters
	try {
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> rpcpeer_id		>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> targetid		>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> max_nb_node	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	} catch(xml_except_t &e) {
		return kad_err_t(kad_err_t::ERROR, "Invalid parameter");
	}

	
	// try to find a kad_rpcpeer_t matching the kad_rpcpeer_id_t
	this->m_rpcpeer	= kad_rpcresp->rpcpeer_from_id(rpcpeer_id);
	if( !m_rpcpeer )	return kad_err_t(kad_err_t::ERROR, "no kad_rpcpeer_t matches this kad_rpcpeer_id_t.");
	// link this object to the kad_rpcpeer_t 
	rpcpeer()->rpcclosestnode_dolink(this);
	
	// log to debug
	KLOG_ERR("rpcpeer_id="		<< rpcpeer_id);
	KLOG_ERR("targetid="		<< targetid);
	KLOG_ERR("max_nb_node="		<< max_nb_node);
	KLOG_ERR("expire_delay="	<< expire_delay);

	// start the kad_closestnode_t
	m_kad_closestnode	= nipmem_new kad_closestnode_t();
	kad_err		= kad_closestnode()->start(rpcpeer()->kad_peer(), targetid, max_nb_node
							, expire_delay, this, NULL);
	if( kad_err.failed() )		return kad_err;

	/*
	 * NOTE: if this point is reached, the rpc method is considered successfully started 
	 */

	// backup the httpd_request_t to send the reply later
	httpd_request	= nipmem_new httpd_request_t(recved_request);

	// return no error
	return kad_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			kad_closestnode_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_closestnode_t has an event to notify
 */
bool kad_rpcclosestnode_t::neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
						, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the httpd_request_t MUST be set
	DBG_ASSERT( httpd_request );
	// sanity check - the kad_event_t MUST be closestnode_ok()
	DBG_ASSERT( kad_event.is_closestnode_ok() );

	// build a reply with the kad_rpclistener_id_t (which uniquely identify this kad_rpclistener_t)
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::RESP_BEG;
	xmlrpc_build	<< kad_event;
	xmlrpc_build << xmlrpc_build_t::RESP_END;
	// log to debug
	KLOG_ERR("Built xml reply is " << xmlrpc_build.to_stdstring());

	// put the reply into the httpd reply
	httpd_request->get_reply() << xmlrpc_build.to_stdstring();
	httpd_request->get_reply_mimetype() = "text/xml";
	// notify the delayed reply to the httpd_t
	kad_rpcresp_t *	rpcresp	= rpcpeer()->rpclistener()->rpcresp();
	rpcresp->notify_httpd_reply(*httpd_request, httpd_err_t::OK);
	
	// autodelete
	nipmem_delete this;
	// return dontkeep
	return false;
}


NEOIP_NAMESPACE_END





