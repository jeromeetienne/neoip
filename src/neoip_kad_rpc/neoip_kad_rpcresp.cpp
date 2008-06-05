/*! \file
    \brief Definition of the \ref kad_rpcresp_t class

- TODO handle the listener_dtor(rpclistener_id)
  - peer_dtor(rpcpeer_id)
  - how to handle them ?
- i think all the command have a LOT of duplication...

    
\par Note about using the old httpd_t
- this is done on top of the old httpd_t because the current http_resp_t
  doesnt support yet the simple request
  - in POST or GET
- TODO to be refactored later on top of http_listener_t and have it passed
  as parameter in the start() parameter
  - using the notyetimplemented http_sresp_t

 
*/

/* system include */
/* local include */
#include "neoip_kad_rpcresp.hpp"
#include "neoip_kad_rpclistener.hpp"
#include "neoip_kad_rpclistener_id.hpp"
#include "neoip_kad_rpcpeer.hpp"
#include "neoip_kad_rpcpeer_id.hpp"
#include "neoip_kad_rpcresp.hpp"
#include "neoip_kad_rpcstore.hpp"
#include "neoip_kad_rpcquery.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_httpd.hpp"
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
kad_rpcresp_t::kad_rpcresp_t()		throw()
{
}

/** \brief Destructor
 */
kad_rpcresp_t::~kad_rpcresp_t()		throw()
{
	// delete all kad_rpcpeer_t from the listener_db
	while( !rpclistener_db().empty() )	nipmem_delete rpclistener_db().front();
	// remote the handler from httpd
	lib_session_t *	lib_session	= lib_session_get();
	if( !url_path.empty() )	lib_session->get_httpd()->handler_del(url_path, this, NULL);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operations
 */
kad_err_t	kad_rpcresp_t::start(const std::string &url_path)	throw()
{
	// copy the parameter
	this->url_path	= url_path;

	// setup the url_path in the lib_session_t
	lib_session_get()->get_httpd()->handler_add(url_path, this, NULL);	
	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on a kad_rpclistener_t matching the kad_rpclistener_id_t 
 * 
 * - return NULL if none match
 */
kad_rpclistener_t *kad_rpcresp_t::rpclistener_from_id(const kad_rpclistener_id_t &rpclistener_id) const throw()
{
	rpclistener_db_t::const_iterator	iter;
	// go thru the whole rpclistener_db
	for(iter = m_rpclistener_db.begin(); iter != m_rpclistener_db.end(); iter++){
		kad_rpclistener_t *	rpclistener	= *iter;
		// if this kad_rpclistener_t match, return its point
		if( rpclistener->rpclistener_id() == rpclistener_id )	return rpclistener; 
	}
	// if this point is reached, no kad_rpclistener_t match, so return NULL
	return NULL;
}

/** \brief Return a pointer on a kad_rpcpeer_t matching the kad_rpcpeer_id_t 
 * 
 * - return NULL if none match
 */
kad_rpcpeer_t *kad_rpcresp_t::rpcpeer_from_id(const kad_rpcpeer_id_t &rpcpeer_id)	const throw()
{
	// try to find a kad_rpclistener_t matching the rpcpeer_id.rpclistener_id() 
	kad_rpclistener_t *	rpclistener	= rpclistener_from_id(rpcpeer_id.rpclistener_id());
	// if no rpclistener is found, return NULL
	if( !rpclistener )	return NULL;
	// try to find a kad_rpcpeer_t matching the rpcpeerid within this rpclistener
	kad_rpcpeer_t *		rpcpeer		= rpclistener->rpcpeer_from_id(rpcpeer_id);
	// if no rpcpeer is found, return NULL
	if( !rpcpeer )	return NULL;
	// return the found rpcpeer
	return rpcpeer;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Forward the httpd_request_t to the registered httpd_t
 * 
 * - it send it using notify_delayed_reply because ALL httpd_request_t are reply as delayed
 *   - except the one in error during the start() of the method
 */
void	kad_rpcresp_t::notify_httpd_reply(const httpd_request_t &httpd_request
					, const httpd_err_t &httpd_err)		throw()
{
	lib_session_get()->get_httpd()->notify_delayed_reply(httpd_request, httpd_err);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			old httpd_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t kad_rpcresp_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw()
{
	xmlrpc_parse_t		xmlrpc_parse;
	// log to debug
	KLOG_ERR("request method=" << request.get_method() << " path=" << request.get_path() );

	// if it not a POST, return a NOT_FOUND now
	if( request.get_method() != "POST" )	return httpd_err_t::NOT_FOUND;

	KLOG_ERR("posted_data=" << request.get_posted_data() );

	// set the document for xmlrpc_parse_t
	xmlrpc_parse.set_document(request.get_posted_data());
	// if the xml inside the document is unparsable, return an error
	if( xmlrpc_parse.is_null() )	return httpd_err_t::BOGUS_REQUEST;

	// parse the method_name
	std::string	method_name;
	try {
		xmlrpc_parse >> xmlrpc_parse_t::CALL_BEG(method_name);
	} catch(xml_except_t &e){
		KLOG_ERR("xml_except_t=" << e.what());
		return httpd_err_t::BOGUS_REQUEST;
	}

	// if a sfct_addr has been found, call it, or return a fault
	kad_err_t	kad_err;
	if( method_name == "listener_ctor" ){
		kad_rpclistener_t *	kad_rpclistener;
		kad_rpclistener	= nipmem_new kad_rpclistener_t();
		kad_err		= kad_rpclistener->start(this, request, method_name, xmlrpc_parse);
		if( kad_err.failed() )	nipmem_delete kad_rpclistener;
// TODO how to handle the listener_ctor ?
	}else if( method_name == "peer_ctor" ){
// TODO how to handle the peer_dtor ?
		kad_rpcpeer_t *	kad_rpcpeer;
		kad_rpcpeer	= nipmem_new kad_rpcpeer_t();
		kad_err		= kad_rpcpeer->start(this, request, method_name, xmlrpc_parse);
		if( kad_err.failed() )	nipmem_delete kad_rpcpeer;
	}else if( method_name == "store" ){
		kad_rpcstore_t*	kad_rpcstore;
		kad_rpcstore	= nipmem_new kad_rpcstore_t();
		kad_err		= kad_rpcstore->start(this, request, method_name, xmlrpc_parse);
		if( kad_err.failed() )	nipmem_delete kad_rpcstore;
	}else if( method_name == "query" ){
		kad_rpcquery_t*	kad_rpcquery;
		kad_rpcquery	= nipmem_new kad_rpcquery_t();
		kad_err		= kad_rpcquery->start(this, request, method_name, xmlrpc_parse);
		if( kad_err.failed() )	nipmem_delete kad_rpcquery;
	}else{
		kad_err	= kad_err_t(kad_err_t::ERROR, "unknown method " + method_name);
	}

	// if the xmlrpc method_name can not be started, reply a xmlrpc_built_t::FAULT immediatly
	if( kad_err.failed() ){
		KLOG_ERR("sending fault -1 " << kad_err);
		xmlrpc_build_t	xmlrpc_build;
		xmlrpc_build	<< xmlrpc_build_t::FAULT(-1, kad_err.to_string());
		// put the just built xmlrpc reply into the httpd_request_t
		request.get_reply() << xmlrpc_build.to_stdstring();	
		request.get_reply_mimetype() = "text/xml";
		// return no error - as the error is at the xmlrpc level, not http one
		// - from the spec 'Unless there's a lower-level error, always return 200 OK."
		//   what is lower-level error isnt specified, so i assumed it is http and lower
		return httpd_err_t::OK;
	}

	// mark the http_request_t as DELAYED_REPLY
	return httpd_err_t::DELAYED_REPLY;
}

NEOIP_NAMESPACE_END





