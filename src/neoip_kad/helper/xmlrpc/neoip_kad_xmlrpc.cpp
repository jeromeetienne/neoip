/*! \file
    \brief Definition of the \ref kad_xmlrpc_t class

\par Brief Description
This implement a handler of xmlrpc for operation on a given kad_listener_t

*/

/* system include */
/* local include */
#include "neoip_kad_xmlrpc.hpp"
#include "neoip_kad_xmlrpc_cmd.hpp"
#include "neoip_kad_xmlrpc_sess.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 * 
 * @param kad_listener	a pointer on the kad_listener on which this kad_xmlrpc_t operates
 * @param url_path	if set, init a httpd_handler_cb_t in the lib_session httpd
 *                      using url_path as path
 */
kad_xmlrpc_t::kad_xmlrpc_t(kad_listener_t *kad_listener, const std::string &url_path
				, const kad_xmlrpc_profile_t &profile)		throw()
{
	// copy the parameter
	this->kad_listener	= kad_listener;
	this->profile		= profile;
	this->url_path		= url_path;
	// add the basic handler
	if( !url_path.empty() )
		lib_session_get()->get_httpd()->handler_add(url_path, this, NULL);
}

/** \brief Destructor
 */
kad_xmlrpc_t::~kad_xmlrpc_t()							throw()
{
	// remote the handler from httpd
	if( !url_path.empty() )
		lib_session_get()->get_httpd()->handler_del(url_path, this, NULL);
	// delete all pending commands
	while( !xmlrpc_cmd_db.empty() )		nipmem_delete	xmlrpc_cmd_db.front();
	// delete all pending session
	while( !xmlrpc_sess_db.empty() )	nipmem_delete	xmlrpc_sess_db.front();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Process a httpd_request_t addressed to this kad_xmlrpc_t
 * 
 * - it may be called externally if the httpd_handler_cb_t is not done locally
 */
httpd_err_t kad_xmlrpc_t::httpd_handler(httpd_request_t &request)	throw()
{
	// log to debug
	KLOG_ERR("request method=" << request.get_method() << " path=" << request.get_path() );

	// intercept the xmlrpc
	if( request.get_method() == "POST" && request.get_path() == "/neoip_lib/RPC2"){
		// create a xmlrpc_cmd_t - it link itself to kad_http_t
		xmlrpc_cmd_t *	xmlrpc_cmd	= nipmem_new xmlrpc_cmd_t(this);
		// Start the xmlrpc_cmd
		return xmlrpc_cmd->start(request);
	}
	// if this point is reached, return NOT_FOUND
	return httpd_err_t::NOT_FOUND;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t kad_xmlrpc_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)
										throw()
{
	// this function does nothing but forwarding the httpd_request_t to the internal handler
	return httpd_handler(request);
}

NEOIP_NAMESPACE_END





