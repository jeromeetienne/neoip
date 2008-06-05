/*! \file
    \brief Definition of the \ref oload_httpo_ctrl_t

\par Brief Description
This object is made to allow control the bt_httpo_full_t connected to neoip-oload.

\par About bt_httpo_full_t identification
- this is done by using a  httpo_full_id which is a random number chosen by the client
- ISSUE: httpo_full_id is chosen by the client *but* must be unique inside neoip-oload
  - so there is a risk of collision
  - it is recommended to get a httpo_full_id which is large and random 'enougth' 
    to make the collision so unlikely that it is negligible.

\par About the change_maxrate
- currently the main goal is to workaround a bug/feature of flash plugin
  aka to freeze the xmit of bt_httpo_full_t after a given threshold
- the flash plugin download data as fast as possible independantly of how
  fast the video is played
  - CON: waste of bandwidth: this may cause to download a lot more than necessary
  - CON: waste of CPU: it the bandwidth is high, the flash plugin takes a 
    significant amount of CPU
- google video works around this trouble with hardcoding a bandwidth limiter
  in their server. 
  - neoip-oload provides the same service with the outter_var httpo_maxrate
    and httpo_maxrate_thres
  - this rpc provides a more precise and more flexible way to do it.    

*/

/* system include */
/* local include */
#include "neoip_oload_httpo_ctrl.hpp"
#include "neoip_oload_apps.hpp"
#include "neoip_oload_swarm.hpp"
#include "neoip_oload_helper.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_xmlrpc_listener.hpp"
#include "neoip_xmlrpc_resp.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
oload_httpo_ctrl_t::oload_httpo_ctrl_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_xmlrpc_listener	= NULL;
	m_xmlrpc_resp		= NULL;
}

/** \brief Destructor
 */
oload_httpo_ctrl_t::~oload_httpo_ctrl_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete xmlrpc_resp_t if needed
	nipmem_zdelete	m_xmlrpc_resp;
	// delete xmlrpc_listener_t if needed
	nipmem_zdelete	m_xmlrpc_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	oload_httpo_ctrl_t::start(oload_apps_t *m_oload_apps)		throw()
{
	http_listener_t *http_listener	= m_oload_apps->http_listener();
	// copy the parameter
	this->m_oload_apps	= m_oload_apps;
	// sanity check - http_listener MUST NOT be NULL
	DBG_ASSERT( http_listener );

	// build the http_uri_t on which to listen
	http_uri_t	xmlrpc_uri = "http://0.0.0.0/neoip_oload_httpo_ctrl_xmlrpc.cgi";
	http_uri_t	jsrest_uri = "http://0.0.0.0/neoip_oload_httpo_ctrl_jsrest.js";
	// start the xmlrpc_listener_t
	xmlrpc_err_t	xmlrpc_err;
	m_xmlrpc_listener= nipmem_new xmlrpc_listener_t();
	xmlrpc_err	= m_xmlrpc_listener->start(http_listener, xmlrpc_uri, jsrest_uri);
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);
	// start the xmlrpc_resp_t
	m_xmlrpc_resp	= nipmem_new xmlrpc_resp_t();
	xmlrpc_err	= m_xmlrpc_resp->start(m_xmlrpc_listener, this, NULL);
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);
	// add the method_name in the xmlrpc_resp_t
	m_xmlrpc_resp->insert_method("set_httpo_maxrate");

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the bt_httpo_full_t which got this string as 'httpo_full_id' outter_var
 * 
 * - NOTE: this function is rather messy...
 */
bt_httpo_full_t * oload_httpo_ctrl_t::httpo_full_by_id(const std::string &httpo_full_id)throw()
{
	std::list<oload_swarm_t *> &	swarm_db	= m_oload_apps->swarm_db;
	std::list<oload_swarm_t *>::iterator	iter_swarm;
	// go thru all the current oload_swarm_t
	for(iter_swarm = swarm_db.begin(); iter_swarm != swarm_db.end(); iter_swarm++){
		oload_swarm_t *			oload_swarm	= *iter_swarm;
		std::list<bt_httpo_full_t *> &	httpo_full_db	= oload_swarm->m_httpo_full_db;
		std::list<bt_httpo_full_t *>::iterator	iter_httpo;
		// go thru all the bt_httpo_full_t of this oload_swarm_t
		for(iter_httpo = httpo_full_db.begin(); iter_httpo != httpo_full_db.end(); iter_httpo++){
			bt_httpo_full_t * httpo_full	= *iter_httpo;
			const http_reqhd_t &http_reqhd	= httpo_full->http_reqhd();
			// get the outter_uri for this bt_httpo_full_t
			const http_uri_t & nested_uri	= http_reqhd.uri();
			http_uri_t 	outter_uri	= oload_helper_t::parse_outter_uri(nested_uri);
			// if this bt_httpo_full_t "httpo_full_id" outter_var matches, return it
			std::string	var_value	= outter_uri.var().get_first_value("httpo_full_id");
			if( var_value == httpo_full_id )		return httpo_full;
		}
	}
	// return NULL if none matches
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xmlrpc_resp_t main callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref xmlrpc_resp_t when to notify an event
 * 
 * - WARNING: it is not allowed to destroy the xmlrpc_listener_t on which 
 *   whic xmlrpc_resp_t is listening on *DURING* the callback.
 * 
 * @return true if the xmlrpc_resp_t is still valid after the callback
 */
bool	oload_httpo_ctrl_t::neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &xmlrpc_err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw()
{
	// call the proper handler depending on the method_name
	if( method_name == "set_httpo_maxrate" ){
		xmlrpc_err_out	= xmlrpc_call_set_maxrate(xmlrpc_parse, xmlrpc_build);
	}else{	DBG_ASSERT( 0 );	}
	// log to debug
	if( xmlrpc_err_out.failed() )	KLOG_ERR("xmlrpc_err_out=" << xmlrpc_err_out);
	// return tokeep	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xmlrpc_resp_t call handler
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handle the call for 'request_stream'
 */
xmlrpc_err_t	oload_httpo_ctrl_t::xmlrpc_call_set_maxrate(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw()
{
	xmlrpc_err_t	xmlrpc_err	= xmlrpc_err_t::OK;
	std::string	httpo_full_id;
	std::string	maxrate_str;
	// log to debug
	KLOG_DBG("enter");

	// use xmlrpc_parse_t to get the arguments of the xmlrpc call
	NEOIP_XMLRPC_RESP_PARSE_BEGIN	(xmlrpc_parse);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, httpo_full_id);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, maxrate_str);
	NEOIP_XMLRPC_RESP_PARSE_END	(xmlrpc_parse, xmlrpc_err);
	// if there is a error in the xmlrpc_parse_t, return now
	if( xmlrpc_err.failed() )	return xmlrpc_err;

	// log to debug
	KLOG_ERR("enter httpo_full_id=" << httpo_full_id << " maxrate=" << maxrate_str);
	
	// try to find the bt_httpo_full_t for this httpo_full_id
	bt_httpo_full_t * httpo_full	= httpo_full_by_id(httpo_full_id);

	// if no bt_httpo_full_t matches, return an error
	if( !httpo_full ){
		std::string reason = "no bt_httpo_full_t for httpo_full_id=" + httpo_full_id;
		return xmlrpc_err_t(xmlrpc_err_t::ERROR, reason);
	}

	// if the bt_httpo_full_t is not yet started, return an error
	if( !httpo_full->is_started() ){
		std::string reason = "this bt_httpo_full_t is not yet started";
		return xmlrpc_err_t(xmlrpc_err_t::ERROR, reason);
	} 

	// convert the size_t into a file_size
	file_size_t	maxrate	= file_size_t::from_str(maxrate_str);
	// notify the bt_httpo_full_t of the threshold
	httpo_full->xmit_maxrate(maxrate.to_double());
	
	// build the xmlrpc response returning uint32_t(0) to indicate success
	NEOIP_XMLRPC_RESP_BUILD(xmlrpc_build, uint32_t(0));

	// return noerror
	return	xmlrpc_err_t::OK;
}


NEOIP_NAMESPACE_END;




