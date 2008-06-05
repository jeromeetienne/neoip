/*! \file
    \brief Header of shortcut for using the wikidbg_http_t of the lib session
    
*/


#ifndef __NEOIP_LIB_HTTPD_WIKIDBG_HPP__ 
#define __NEOIP_LIB_HTTPD_WIKIDBG_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_cpp_demangle.hpp"
#include "neoip_namespace.hpp"

// list of include which are almost always needed when using the wikidbg_http_t of lib_session_t
// - so they are put here in order to reduce the noise in the user source
#include "neoip_html_builder.hpp"
#include "neoip_httpd_request.hpp"

NEOIP_NAMESPACE_BEGIN;

std::string	wikidbg_html(const std::string &keyword, const void *c_obj_ptr)		throw();
std::string	wikidbg_url(const std::string &keyword, const void *c_obj_ptr)		throw();
std::string	wikidbg_url_rootpath()							throw();
std::string	wikidbg_form_hidden_param(const std::string &keyword, const void *c_obj_ptr)	throw();
std::string	wikidbg_html_callback_typename(const void *c_obj_ptr, const std::string &typename_str) throw();

/** \brief return the html associted with this keyword/object_ptr
 */
template <typename T> std::string	wikidbg_html_callback(T *object_ptr)	throw()
{
	if( object_ptr == NULL )	return "null";
	return wikidbg_html_callback_typename(object_ptr, neoip_cpp_typename(*object_ptr));
}
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_HTTPD_WIKIDBG_HPP__  */



