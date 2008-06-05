/*! \file
    \brief Definition of the \ref xmlrpc_resp0_t class

\par Note about using the old httpd_t
- this is done on top of the old httpd_t because the current http_resp0_t
  doesnt support yet the simple request
  - in POST or GET
- TODO to be refactored later on top of http_listener_t and have it passed
  as parameter in the start() parameter
  - using the notyetimplemented http_sresp0_t

\par Note on how to do a xmlrpc client in ruby
- noted to remember it while testing
- require 'xmlrpc/client'
  server = XMLRPC::Client.new2("http://127.0.0.1:9080/neoip/RPC2")
  result = server.call("add", 5, 3)

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_resp0.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_xmlrpc_resp0_sfct.hpp"
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
xmlrpc_resp0_t::xmlrpc_resp0_t()		throw()
{
}

/** \brief Destructor
 */
xmlrpc_resp0_t::~xmlrpc_resp0_t()		throw()
{
	lib_session_t *	lib_session	= lib_session_get();
	// remote the handler from httpd
	if( !url_path.empty() )	lib_session->get_httpd()->handler_del(url_path, this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operations
 */
xmlrpc_err_t	xmlrpc_resp0_t::start(const std::string &url_path)	throw()
{
	lib_session_t *	lib_session	= lib_session_get();
	// copy the parameter
	this->url_path	= url_path;

	// setup the url_path in the lib_session_t
	lib_session->get_httpd()->handler_add(url_path, this, NULL);
	
	// return no error
	return xmlrpc_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			old httpd_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t xmlrpc_resp0_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw()
{
	xmlrpc_resp0_sfct_addr_t	sfct_addr	= NULL;
	void *			sfct_userptr	= NULL;
	xmlrpc_parse_t		xmlrpc_parse;
	xmlrpc_build_t		xmlrpc_build;	
	// log to debug
	KLOG_DBG("request method=" << request.get_method() << " path=" << request.get_path() );

	// if it not a POST, return a NOT_FOUND now
	if( request.get_method() != "POST" )	return httpd_err_t::NOT_FOUND;

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

	// go thru the sfct_db_arr trying to find the received method_name
	for(size_t i = 0; i < sfct_db_arr().size(); i++){
		xmlrpc_resp0_sfct_db_t::db_t &		db	= sfct_db_arr()[i]->db;
		xmlrpc_resp0_sfct_db_t::db_t::iterator	iter;
		// try to find method_name in this xmlrpc_resp0_sfct_db
		iter	= db.find(method_name);
		// if it is not found, goto the next
		if( iter == db.end() )	continue;
		// if it has been found, set the sfct_addr and leave the loop
		sfct_addr	= iter->second;
		sfct_userptr	= sfct_db_arr()[i]->userptr;
		break;
	}

	// if a sfct_addr has been found, call it, or return a fault
	// TODO would be nice to be able to throw fault from those functions tho..
	// - maybe if the sfct_addr return a exception, put the e.what in the xmlrpc fault ?
	// - doesnt seems bad
	if( sfct_addr ){
		(*sfct_addr)(sfct_userptr, xmlrpc_parse, xmlrpc_build);
	}else{
		xmlrpc_build	<< xmlrpc_build_t::FAULT(-1, "unknown method " + method_name);
	}

	// put the just built xmlrpc reply into the httpd_request_t
	request.get_reply() << xmlrpc_build.to_stdstring();	
	request.get_reply_mimetype() = "text/xml";

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END





