/*! \file
    \brief Header of the neoip_string class
    
*/


#ifndef __NEOIP_HTTP_NESTED_URI_HPP__ 
#define __NEOIP_HTTP_NESTED_URI_HPP__ 
/* system include */
/* local include */
#include "neoip_http_uri.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief helper to manipulate nested url 
 * 
 * - e.g. http://innerhost.org/innerpath/http/appache.org/index.html
 * - there are no requirement on the inner http_uri_t but there are some on the outter one
 * - the goal is to have the nested uri to be interpreted the same as the inner one
 * - it aims for proxying http
 */
class http_nested_uri_t {
private:
	static const std::string	ESC_STRING;
public:
	static bool		is_valid_outter(const http_uri_t &outter_uri)	throw();
	static bool		is_valid_nested(const http_uri_t &nested_uri)	throw();
	static http_uri_t	parse_inner(const http_uri_t &nested_uri)	throw();
	static http_uri_t	parse_outter(const http_uri_t &nested_uri)	throw();
	static http_uri_t	build(const http_uri_t &outter_uri
						, const http_uri_t &inner_uri)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_NESTED_URI_HPP__  */



