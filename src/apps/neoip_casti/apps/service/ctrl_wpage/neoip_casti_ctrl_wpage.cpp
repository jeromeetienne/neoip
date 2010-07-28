/*! \file
    \brief Definition of the \ref casti_ctrl_wpage_t

\par Brief Description
- This object is made to allow control via webpage. this is done via xmlrpc/jsrest
- It is possible to start/stop the casti_swarm_t thru this.
- It is using the same terminology as dhcp as the principle is similar ?
- TODO to improve this documentation

- There is no timeout for the request because the casti_swarm_t will autodelete
  itself as soon as the connection to the scasti_uri disconnect.
  - so closing/crashing the browser will automatically disconnect the http_uri_t
  - and so stop the casti_swarm_t

*/

/* system include */
/* local include */
#include "neoip_casti_ctrl_wpage.hpp"
#include "neoip_casti_apps.hpp"
#include "neoip_casti_swarm.hpp"
#include "neoip_casti_swarm_arg.hpp"
#include "neoip_bt_scasti_mod_type.hpp"
#include "neoip_bt_ezswarm.hpp"
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
casti_ctrl_wpage_t::casti_ctrl_wpage_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_xmlrpc_listener	= NULL;
	m_xmlrpc_resp		= NULL;
}

/** \brief Destructor
 */
casti_ctrl_wpage_t::~casti_ctrl_wpage_t()	throw()
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
bt_err_t	casti_ctrl_wpage_t::start(casti_apps_t *m_casti_apps)		throw()
{
	http_listener_t *http_listener	= m_casti_apps->http_listener();
	// copy the parameter
	this->m_casti_apps	= m_casti_apps;

	// sanity check - http_listener MUST NOT be NULL
	DBG_ASSERT( http_listener );

	// build the http_uri_t on which to listen
	http_uri_t	xmlrpc_uri = "http://0.0.0.0/neoip_casti_ctrl_wpage_xmlrpc.cgi";
	http_uri_t	jsrest_uri = "http://0.0.0.0/neoip_casti_ctrl_wpage_jsrest.js";
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
	m_xmlrpc_resp->insert_method("request_stream");
	m_xmlrpc_resp->insert_method("release_stream");

	// return no error
	return bt_err_t::OK;
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
bool	casti_ctrl_wpage_t::neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &xmlrpc_err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw()
{
	// call the proper handler depending on the method_name
	if( method_name == "request_stream" ){
		xmlrpc_err_out	= xmlrpc_call_request_stream(xmlrpc_parse, xmlrpc_build);
	}else if( method_name == "release_stream" ){
		xmlrpc_err_out	= xmlrpc_call_release_stream(xmlrpc_parse, xmlrpc_build);
	}else	DBG_ASSERT( 0 );
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
xmlrpc_err_t	casti_ctrl_wpage_t::xmlrpc_call_request_stream(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw()
{
	xmlrpc_err_t	xmlrpc_err	= xmlrpc_err_t::OK;
	casti_swarm_arg_t swarm_arg	= m_casti_apps->swarm_arg_default();
	std::string	cast_name;
	std::string	cast_privtext;
	http_uri_t	scasti_uri;
	std::string	scasti_mod_str;
	http_uri_t	mdata_srv_uri;
	http_uri_t	http_peersrc_uri;
	std::string	web2srv_str;
	bt_err_t	bt_err;
	// log to debug
	KLOG_ERR("enter");
	// use xmlrpc_parse_t to get the arguments of the xmlrpc call
	NEOIP_XMLRPC_RESP_PARSE_BEGIN	(xmlrpc_parse);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, mdata_srv_uri);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, cast_name);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, cast_privtext);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, scasti_uri);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, scasti_mod_str);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, http_peersrc_uri);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, web2srv_str);
	NEOIP_XMLRPC_RESP_PARSE_END	(xmlrpc_parse, xmlrpc_err);
	// if there is a error in the xmlrpc_parse_t, return now
	if( xmlrpc_err.failed() )	return xmlrpc_err;

	// to populate the casti_swarm_arg_t with the xmlrpc call parameters
	if( !mdata_srv_uri.is_null())	swarm_arg.mdata_srv_uri		(mdata_srv_uri);
	if( !cast_name.empty() )	swarm_arg.cast_name		(cast_name);
	if( !cast_privtext.empty() )	swarm_arg.cast_privtext		(cast_privtext);
	if( !scasti_uri.is_null())	swarm_arg.scasti_uri		(scasti_uri);
	if( !scasti_mod_str.empty() )	swarm_arg.scasti_mod		(scasti_mod_str);
	if( !http_peersrc_uri.is_null())swarm_arg.http_peersrc_uri	(http_peersrc_uri);
	if( !web2srv_str.empty())	swarm_arg.web2srv_str		(web2srv_str);

	// check the resulting casti_swarm_arg_t
	bt_err		= swarm_arg.check();
	if( bt_err.failed() )	return xmlrpc_err_t(xmlrpc_err_t::ERROR, bt_err.reason());

	// try to get the casti_swarm for this mdata_srv_uri/cast_name
	casti_swarm_t *	casti_swarm	= m_casti_apps->swarm_by(mdata_srv_uri, cast_name, cast_privtext);

	// if there is a previous casti_swarm_t which is currently stopping, delete it
	// - the new one takes precedence
	if( casti_swarm && casti_swarm->state().is_stopping() )	nipmem_zdelete casti_swarm;

	// if casti_swarm doesnt yet exist, launch it now
	if( !casti_swarm ){
		casti_swarm	= nipmem_new casti_swarm_t();
		bt_err		= casti_swarm->start(swarm_arg);
		if( bt_err.failed() ){
			nipmem_zdelete	casti_swarm;
			return xmlrpc_err_t(xmlrpc_err_t::ERROR, "Cant start stream due to " + bt_err.to_string());
		}
	}else{
		// refresh the idle_timeout
		casti_swarm->idle_timeout_refresh();
		// if casti_swarm already exists, update web2srv_str
		casti_swarm->web2srv_str(web2srv_str);
	}
	
	// build the xmlrpc response with the cast_privhash
	NEOIP_XMLRPC_RESP_BUILD(xmlrpc_build, casti_swarm->cast_privhash() );

	// return noerror
	return	xmlrpc_err_t::OK;
}

/** \brief handle the call for 'release_stream'
 */
xmlrpc_err_t	casti_ctrl_wpage_t::xmlrpc_call_release_stream(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw()
{
	xmlrpc_err_t	xmlrpc_err	= xmlrpc_err_t::OK;
	std::string	cast_name;
	std::string	cast_privtext;
	http_uri_t	mdata_srv_uri;
	// log to debug
	KLOG_ERR("enter");

	// use xmlrpc_parse_t to get the arguments of the xmlrpc call
	NEOIP_XMLRPC_RESP_PARSE_BEGIN	(xmlrpc_parse);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, mdata_srv_uri);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, cast_name);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, cast_privtext);
	NEOIP_XMLRPC_RESP_PARSE_END	(xmlrpc_parse, xmlrpc_err);
	// if there is a error in the xmlrpc_parse_t, return now
	if( xmlrpc_err.failed() )	return xmlrpc_err;

	// try to get the casti_swarm for this mdata_srv_uri/cast_name
	casti_swarm_t *	casti_swarm	= m_casti_apps->swarm_by(mdata_srv_uri, cast_name, cast_privtext);

	// if no casti_swarm matches those parameters, return an error
	if( !casti_swarm )	return xmlrpc_err_t(xmlrpc_err_t::ERROR, "no matching stream");

	// do a casti_swarm->gracefull_shutdown()
	casti_swarm->gracefull_shutdown("User Released the stream");

	// build the xmlrpc response for no error
	NEOIP_XMLRPC_RESP_BUILD(xmlrpc_build, uint32_t(0) );

	// return noerror
	return	xmlrpc_err_t::OK;
}

NEOIP_NAMESPACE_END;




