/*! \file
    \brief Definition of the \ref wikidbg_http_t class

\par Possible Improvement
- to allow to have http page - aka stuff which return httpd_err_t and stuff
  - LATER: i think it is already done for ages :)

*/

/* system include */
#include <stdlib.h>
/* local include */
#include "neoip_wikidbg_http.hpp"
#include "neoip_wikidbg_global_db.hpp"
#include "neoip_httpd.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
wikidbg_http_t::wikidbg_http_t(httpd_t *httpd_server, const std::string &url_path)	throw()
{
	// copy the parameter
	this->httpd_server	= httpd_server;
	this->url_path		= url_path;
	// register the handler
	httpd_server->handler_add(url_path, this, NULL);	
}

/** \brief Destructor
 */
wikidbg_http_t::~wikidbg_http_t()	throw()
{
	// unregister the handler
	httpd_server->handler_del(url_path, this, NULL);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            Query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief build an url for a given keyword for a specific object
 */
std::string	wikidbg_http_t::build_url(const std::string &keyword, void *object_ptr)	const throw()
{
	std::ostringstream	oss;
	// add the url_path
	oss << url_path;
	// add the keyword
	oss << "?keyword=" << keyword;
	oss << "&pointer=" << object_ptr;
	// return the built string
	return oss.str();	
}


/** \brief return the html to put in a form if the destination of the form is a wikidbg_url
 */
std::string	wikidbg_http_t::build_form_hidden_param(const std::string &keyword, void *object_ptr)	const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "<input type=\"hidden\" name=\"keyword\" value=\"" << keyword	<< "\">";
	oss << "<input type=\"hidden\" name=\"pointer\" value=\"" << object_ptr	<< "\">";
	// return the just built string
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t wikidbg_http_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &httpd_request)
										throw()
{
	std::string	keyword_str	= httpd_request.get_variable("keyword");
	std::string	pointer_str	= httpd_request.get_variable("pointer");
	void *		object_ptr;
	// log to debug
	KLOG_DBG("enter");

	// if the variable are not present, return an error
	if( keyword_str.empty() )	return httpd_err_t::BOGUS_REQUEST;
	if( pointer_str.empty() )	return httpd_err_t::BOGUS_REQUEST;
	
	// convert the pointer_str into a object_ptr
	std::istringstream	iss(pointer_str);
	iss >> object_ptr;

	// ask the wikidbg_global_db_t to handle the page associaited with this keyword	
	return wikidbg_global_db_t::get_keyword_page(keyword_str, object_ptr, httpd_request);
}



NEOIP_NAMESPACE_END


