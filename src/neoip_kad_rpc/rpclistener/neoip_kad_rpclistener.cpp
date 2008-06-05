/*! \file
    \brief Definition of the \ref kad_rpclistener_t class

- TODO to handle the idle timeout
  - hgow to refresh the timeout
  - how to parse the delay from the caller ?
    - 0 means infinite
    - in xmlrpc delay_t are coded as double
- TODO to review the stuff about ensuring no other kad_rpclistener_t has the same listenerid

- code the kad_rpcpeer_id_t and kad_rpclistener_id_t
  - use the 'already existing unique key', no need to add more state with a slotpool_t
  - kad_rpcpeer_id_t = kad_peerid_t of the kad_peer_t
  - kad_rpclistener_id_t = udp_listen_lview
    
*/

/* system include */
/* local include */
#include "neoip_kad_rpclistener.hpp"
#include "neoip_kad_rpclistener_id.hpp"
#include "neoip_kad_rpcpeer.hpp"
#include "neoip_kad_rpcpeer_id.hpp"
#include "neoip_kad_rpcresp.hpp"
#include "neoip_kad.hpp"
#include "neoip_udp_vresp.hpp"
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
kad_rpclistener_t::kad_rpclistener_t()		throw()
{
	// zero some field
	m_rpcresp	= NULL;
	m_udp_vresp	= NULL;
	m_kad_listener	= NULL;
	httpd_request	= NULL;	
}

/** \brief Destructor
 */
kad_rpclistener_t::~kad_rpclistener_t()		throw()
{
	// if httpd_request is set, the http reply has not yet been sent, notify an error now
	if( httpd_request ){
		// notify the http error to the caller
		// - i dunno which one, so i sent httpd_err_t::INTERNAL_ERROR which seems the closest...
		rpcresp()->notify_httpd_reply(*httpd_request, httpd_err_t::INTERNAL_ERROR);
		// delete the httpd_request_t if needed
		nipmem_zdelete	httpd_request;
	}
	// delete all kad_rpcpeer_t from the peer_db
	while( !rpcpeer_db().empty() )	nipmem_delete rpcpeer_db().front();
	// unlink this object from the kad_rpcresp_t 
	if( m_rpcresp )	rpcresp()->rpclistener_unlink(this);
	// delete the kad_listener_t if needed
	nipmem_zdelete	m_kad_listener;	
	// delete the udp_vresp_t if needed
	nipmem_zdelete	m_udp_vresp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operations
 */
kad_err_t	kad_rpclistener_t::start(kad_rpcresp_t *m_rpcresp, const httpd_request_t &recved_request
				, const std::string &method_name, xmlrpc_parse_t &xmlrpc_parse)	throw()
{
	ipport_addr_t	udp_listen_lview;
	ipport_addr_t	udp_listen_pview;
	ipport_addr_t	nslan_addr;
	inet_err_t	inet_err;
	kad_err_t	kad_err;
	// log to debug
	KLOG_ERR("enter");

	// parse the xmlrpc_parse_t to get the rpc parameters
	try {
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> udp_listen_lview	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> udp_listen_pview	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> nslan_addr		>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::PARAM_BEG >> idle_maxdelay	>> xmlrpc_parse_t::PARAM_END;
	} catch(xml_except_t &e) {
		return kad_err_t(kad_err_t::ERROR, "Invalid parameter");
	}

	// if the caller specific a idle_maxdelay of 0, this is considered as infinite
	if( idle_maxdelay == delay_t(0) )	idle_maxdelay	= delay_t::INFINITE;
	// start the idle_timeout
	idle_timeout.start(idle_maxdelay, this, NULL);

	// log to debug
	KLOG_ERR("udp_listen_lview="	<< udp_listen_lview);
	KLOG_ERR("udp_listen_pview="	<< udp_listen_pview);
	KLOG_ERR("nslan_addr=" 		<< nslan_addr);
	KLOG_ERR("idle_maxdelay="	<< idle_maxdelay);

	// create the udp_vresp
	m_udp_vresp	= nipmem_new udp_vresp_t();
	inet_err 	= udp_vresp()->start(udp_listen_lview);
	if( inet_err.failed() )		return kad_err_from_inet(inet_err);
	// init kad_listener
	ipport_aview_t	udp_listen_aview= ipport_aview_t()	.lview(udp_vresp()->get_listen_addr())
								.pview(udp_listen_pview);
	m_kad_listener	= nipmem_new kad_listener_t();
	kad_err 	= kad_listener()->start(nslan_addr, udp_vresp(), udp_listen_aview);
	if( kad_err.failed() )		return kad_err;

	/*
	 * NOTE: if this point is reached, the rpc method is considered successfull 
	 */

	// copy the parameter
	this->m_rpcresp	= m_rpcresp;
	// link this object to the kad_rpcresp_t 
	rpcresp()->rpclistener_dolink(this);

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
bool	kad_rpclistener_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// sanity check - the httpd_request_t MUST be set
	DBG_ASSERT( httpd_request );

	// build a reply with the kad_rpclistener_id_t (which uniquely identify this kad_rpclistener_t)
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::RESP_BEG;
	xmlrpc_build	<< rpclistener_id();
	xmlrpc_build << xmlrpc_build_t::RESP_END;
	// log to debug
	KLOG_ERR("Built xml reply is " << xmlrpc_build.to_stdstring());

	// put the reply into the httpd reply
	httpd_request->get_reply() << xmlrpc_build.to_stdstring();
	httpd_request->get_reply_mimetype() = "text/xml";
	// notify the delayed reply to the httpd_t
	rpcresp()->notify_httpd_reply(*httpd_request, httpd_err_t::OK);
	
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

/** \brief Return a kad_rpclistener_id_t for this kad_rpclistener_t
 */
kad_rpclistener_id_t	kad_rpclistener_t::rpclistener_id()	const throw()
{
	return kad_rpclistener_id_t::from_rpclistener(this);
}

/** \brief Return a pointer on a kad_rpcpeer_t matching the kad_rpcpeer_id_t 
 * 
 * - return NULL if none match
 */
kad_rpcpeer_t *kad_rpclistener_t::rpcpeer_from_id(const kad_rpcpeer_id_t &rpcpeer_id) const throw()
{
	rpcpeer_db_t::const_iterator	iter;
	// go thru the whole rpcpeer_db
	for(iter = m_rpcpeer_db.begin(); iter != m_rpcpeer_db.end(); iter++){
		kad_rpcpeer_t *	rpcpeer	= *iter;
		// if this kad_rpcpeer_t match, return its point
		if( rpcpeer->rpcpeer_id() == rpcpeer_id )	return rpcpeer; 
	}
	// if this point is reached, no kad_rpcpeer_t match, so return NULL
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			idle_timeout handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Refresh the idle_timeout
 */
void	kad_rpclistener_t::idle_refresh()	throw()
{
	// restart the idle timeout 
	idle_timeout.start(idle_maxdelay, this, NULL);
}

/** \brief callback called when the idle_timeout expire
 */
bool kad_rpclistener_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// autodelete
	nipmem_delete	this;	
	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END





