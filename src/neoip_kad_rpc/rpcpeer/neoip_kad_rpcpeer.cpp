/*! \file
    \brief Definition of the \ref kad_rpcpeer_t class

*/

/* system include */
/* local include */
#include "neoip_kad_rpcpeer.hpp"
#include "neoip_kad_rpcpeer_id.hpp"
#include "neoip_kad_rpclistener.hpp"
#include "neoip_kad_rpcresp.hpp"
#include "neoip_kad_rpcclosestnode.hpp"
#include "neoip_kad_rpcdelete.hpp"
#include "neoip_kad_rpcquery_some.hpp"
#include "neoip_kad_rpcquery.hpp"
#include "neoip_kad_rpcstore.hpp"
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
kad_rpcpeer_t::kad_rpcpeer_t()		throw()
{
	// zero some field
	m_rpclistener	= NULL;
	m_kad_peer	= NULL;
	httpd_request	= NULL;
}

/** \brief Destructor
 */
kad_rpcpeer_t::~kad_rpcpeer_t()		throw()
{
	// if httpd_request is set, the http reply has not yet been sent, notify an error now
	if( httpd_request ){
		kad_rpcresp_t *	rpcresp	= rpclistener()->rpcresp();
		// notify the http error to the caller
		// - i dunno which one, so i sent httpd_err_t::INTERNAL_ERROR which seems the closest...
		rpcresp->notify_httpd_reply(*httpd_request, httpd_err_t::INTERNAL_ERROR);
		// delete the httpd_request_t if needed
		nipmem_zdelete	httpd_request;
	}
	// delete all kad_rpcclosestnode_t from the closestnode_db
	while( !rpcclosestnode_db().empty() )	nipmem_delete rpcclosestnode_db().front();
	// delete all kad_rpcdelete_t from the delete_db
	while( !rpcdelete_db().empty() )	nipmem_delete rpcdelete_db().front();
	// delete all kad_rpcquery_some_t from the query_some_db
	while( !rpcquery_some_db().empty() )	nipmem_delete rpcquery_some_db().front();
	// delete all kad_rpcquery_t from the query_db
	while( !rpcquery_db().empty() )		nipmem_delete rpcquery_db().front();
	// delete all kad_rpcstore_t from the store_db
	while( !rpcstore_db().empty() )		nipmem_delete rpcstore_db().front();
	// unlink this object from the kad_rpclistener_t if needed
	if( m_rpclistener )	rpclistener()->rpcpeer_unlink(this);
	// delete the kad_peer_t if needed
	nipmem_zdelete	m_kad_peer;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operations
 */
kad_err_t	kad_rpcpeer_t::start(kad_rpcresp_t *kad_rpcresp, const httpd_request_t &recved_request
							, const std::string &method_name
							, xmlrpc_parse_t &xmlrpc_parse)	throw()
{
	kad_rpclistener_id_t	rpclistener_id;	
	kad_realmid_t		kad_realmid;
	kad_peerid_t		kad_peerid;
	kad_err_t		kad_err;
	// log to debug
	KLOG_ERR("enter");

	// parse the xmlrpc_parse_t to get the rpc parameters
	try {
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> rpclistener_id	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> kad_realmid	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> kad_peerid		>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> idle_maxdelay	>> xmlrpc_parse_t::PARAM_END;
	} catch(xml_except_t &e) {
		return kad_err_t(kad_err_t::ERROR, "Invalid parameter");
	}


	// if the caller specific a idle_maxdelay of 0, this is considered as infinite
	if( idle_maxdelay == delay_t(0) )	idle_maxdelay	= delay_t::INFINITE;
	// start the idle_timeout
	idle_timeout.start(idle_maxdelay, this, NULL);
	
// TODO to review the stuff about ensuring no other kad_rpcpeer_t has the same peerid
	// TODO to check if there i already a kad_rpcpeer_t with this kad_rpcpeer_id_t
	// - if yes, return an error
	// - is it up to here to check that ? why not leave it to the kad_peer_t
	// - relation with the unicity/locality rules, the logic behind kad_peer_t/kad_listener_t
	//   MUST NOT be duplicated here
	//   - if the kad_peer_t is not allowed to be duplicated, it is not up to here to handle it
	//   - ok but what happen in the kad_peer_t if the peerid already exist ?
	///  - i dunno to check

	
	// log to debug
	KLOG_ERR("rpclistener_id="	<< rpclistener_id);
	KLOG_ERR("kad_realmid="		<< kad_realmid);
	KLOG_ERR("kad_peerid=" 		<< kad_peerid);
	KLOG_ERR("idle_maxdelay="	<< idle_maxdelay);
	
		
	// try to find a kad_rpclistener_t matching the kad_rpclistener_id_t
	this->m_rpclistener	= kad_rpcresp->rpclistener_from_id(rpclistener_id);
	if( !m_rpclistener )	return kad_err_t(kad_err_t::ERROR, "no kad_rpclistener_t matches this kad_rpclistener_id_t.");
	// link this object to the kad_rpclistener_t 
	rpclistener()->rpcpeer_dolink(this);

	// actually start the kad_peer_t
	m_kad_peer	= nipmem_new kad_peer_t();
	kad_err		= kad_peer()->start(m_rpclistener->kad_listener(), kad_realmid, kad_peerid);
	if( kad_err.failed() )		return kad_err;

	/*
	 * NOTE: if this point is reached, the rpc method is considered successfull 
	 */

	// backup the httpd_request_t
	httpd_request	= nipmem_new httpd_request_t(recved_request);
	// initiate a zerotimer_t to send the reply
	reply_zerotimer.append(this, NULL);

	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			reply_zerotimer callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	kad_rpcpeer_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// sanity check - the httpd_request_t MUST be set
	DBG_ASSERT( httpd_request );

	// build a reply with the kad_rpclistener_id_t (which uniquely identify this kad_rpclistener_t)
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::RESP_BEG;
	xmlrpc_build	<< rpcpeer_id();
	xmlrpc_build << xmlrpc_build_t::RESP_END;
	// log to debug
	KLOG_ERR("Built xml reply is " << xmlrpc_build.to_stdstring());

	// put the reply into the httpd reply
	httpd_request->get_reply() << xmlrpc_build.to_stdstring();
	httpd_request->get_reply_mimetype() = "text/xml";
	// notify the delayed reply to the httpd_t
	kad_rpcresp_t *	rpcresp	= rpclistener()->rpcresp();
	rpcresp->notify_httpd_reply(*httpd_request, httpd_err_t::OK);
	
	// delete the httpd_request and mark it unused
	nipmem_zdelete httpd_request;
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a kad_rpcpeer_id_t for this kad_rpcpeer_t
 */
kad_rpcpeer_id_t	kad_rpcpeer_t::rpcpeer_id()	const throw()
{
	return kad_rpcpeer_id_t::from_rpcpeer(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			idle_timeout handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Refresh the idle_timeout
 */
void	kad_rpcpeer_t::idle_refresh()	throw()
{
	// restart the idle timeout for kad_rpclistener_t
	rpclistener()->idle_refresh();
	// restart the idle timeout for this kad_rpcpeer_t 
	idle_timeout.start(idle_maxdelay, this, NULL);
}

/** \brief callback called when the idle_timeout expire
 */
bool kad_rpcpeer_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// autodelete
	nipmem_delete	this;	
	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END





