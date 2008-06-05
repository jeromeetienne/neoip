/*! \file
    \brief Definition of the \ref apps_httpdetect_t

\par Brief Description
apps_httpdetect_t is made to allow webpages to detect the presence of neoip apps
running on the browser computer.
- currently this is done via a xmlrpc_net which allow to access it from pure xmlrpc
  or thru jsrest
  
- TODO to remove the http_sresp_t stuff
  - this was a first attempts for a jsrest, but this is hardcoded
  - this is obsoleted by the xmlrpc_resp_t which is more generic

*/

/* system include */
/* local include */
#include "neoip_apps_httpdetect.hpp"
#include "neoip_xmlrpc_listener.hpp"
#include "neoip_xmlrpc_resp.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_http_sresp.hpp"			// TODO to remove
#include "neoip_http_sresp_ctx.hpp"		// TODO to remove
#include "neoip_http_resp_mode.hpp"		// TODO to remove
#include "neoip_libsess_err.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_file.hpp"
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
apps_httpdetect_t::apps_httpdetect_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_http_sresp		= NULL;
	m_xmlrpc_listener	= NULL;
	m_xmlrpc_resp		= NULL;
}

/** \brief Destructor
 */
apps_httpdetect_t::~apps_httpdetect_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the m_http_sresp if needed
	nipmem_zdelete	m_http_sresp;
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
libsess_err_t	apps_httpdetect_t::start(http_listener_t *http_listener
						, const std::string &p_canon_name
						, const std::string &p_version)	throw()
{
	http_err_t	http_err;
	// copy the parameter
	this->m_canon_name	= p_canon_name;
	this->m_version		= p_version;
	// sanity check - http_listener MUST NOT be NULL
	DBG_ASSERT( http_listener );
	// start the http_sresp_t 
	m_http_sresp	= nipmem_new http_sresp_t();
	http_err	= m_http_sresp->start(http_listener
				, "http://0.0.0.0/" + m_canon_name + "_local_ctor.js"
				, http_method_t::GET, http_resp_mode_t::REJECT_SUBPATH
				, this, NULL);
	if( http_err.failed() )	return libsess_err_from_http(http_err);


	// build the http_uri_t on which to listen
	http_uri_t 	xmlrpc_uri = "http://0.0.0.0/" + m_canon_name + "_appdetect_xmlrpc.cgi";
	http_uri_t	jsrest_uri = "http://0.0.0.0/" + m_canon_name + "_appdetect_jsrest.js";
	
	// start the xmlrpc_listener_t
	xmlrpc_err_t	xmlrpc_err;
	m_xmlrpc_listener= nipmem_new xmlrpc_listener_t();
	xmlrpc_err	= m_xmlrpc_listener->start(http_listener, xmlrpc_uri, jsrest_uri);
	if( xmlrpc_err.failed() )	return libsess_err_from_xmlrpc(xmlrpc_err);
	// start the xmlrpc_resp_t
	m_xmlrpc_resp	= nipmem_new xmlrpc_resp_t();
	xmlrpc_err	= m_xmlrpc_resp->start(m_xmlrpc_listener, this, NULL);
	if( xmlrpc_err.failed() )	return libsess_err_from_xmlrpc(xmlrpc_err);
	// add the method_name in the xmlrpc_resp_t
	m_xmlrpc_resp->insert_method("probe_apps");

	// return no error
	return libsess_err_t::OK;
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
bool	apps_httpdetect_t::neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &xmlrpc_err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw()
{
	// call the proper handler depending on the method_name
	if( method_name == "probe_apps" ){
		xmlrpc_err_out	= handle_call_probe_apps(xmlrpc_parse, xmlrpc_build);
	}else	DBG_ASSERT( 0 );
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
xmlrpc_err_t	apps_httpdetect_t::handle_call_probe_apps(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw()
{
	xmlrpc_err_t	xmlrpc_err	= xmlrpc_err_t::OK;
	// log to debug
	KLOG_ERR("enter");
	// use xmlrpc_parse_t to get the arguments of the xmlrpc call
	NEOIP_XMLRPC_RESP_PARSE_BEGIN	(xmlrpc_parse);
	// NOTE: this call has no argument, so no argument parsing
	NEOIP_XMLRPC_RESP_PARSE_END	(xmlrpc_parse, xmlrpc_err);
	// if there is a error in the xmlrpc_parse_t, return now
	if( xmlrpc_err.failed() )	return xmlrpc_err;


	// NOTE: currently return only the apps version
	// - maybe to return more complete info would be better....
	// - unclear so keep it simple for now

	// build the xmlrpc response with the m_version
	NEOIP_XMLRPC_RESP_BUILD(xmlrpc_build, m_version);

	// return noerror
	return	xmlrpc_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			http_sresp_t main callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_resp_t when to notify an event
 * 
 * @return true if the http_resp_t is still valid after the callback
 */
bool	apps_httpdetect_t::neoip_http_sresp_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
				, http_sresp_ctx_t &sresp_ctx)		throw()
{
	const std::string &	canon_name	= m_canon_name;
	std::ostringstream &	oss		= sresp_ctx.response_body();
	std::string		apps_prefix	= "neoip_";
	std::string		apps_suffix	= canon_name.substr(apps_prefix.size());
	// log to debug
	KLOG_ERR("enter sresp_ctx=" << sresp_ctx);

	// sanity check - the hardcoded prefix MUST be the expected one
	// - NOTE: this way to extract the apps_suffix is dirty but works for now
	DBG_ASSERT( m_canon_name.substr(0, apps_prefix.size()) == apps_prefix );

	// Set the mime-type for javascript
	sresp_ctx.rephd().header_db().update("Content-Type", "application/x-javascript");
	// build the response_body
	oss	<< "if( typeof neoip == 'undefined' )	var neoip = {};\n";
	oss	<< "neoip." << apps_suffix << "_present=true;\n";
	oss	<< "neoip." << apps_suffix << "_version=\"" << m_version << "\";\n";
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END;




