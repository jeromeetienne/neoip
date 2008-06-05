/*! \file
    \brief Header of the neoip_wikidbg class
    
*/


#ifndef __NEOIP_WIKIDBG_HTTP_HPP__ 
#define __NEOIP_WIKIDBG_HTTP_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class httpd_t;

/** \brief Implement a wiki debug
 * 
 * - a very custom layer to easily display the content of the running apps
 */
class wikidbg_http_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	httpd_t *	httpd_server;	//!< the httpd_t on which this object is attached
	std::string	url_path;	//!< the url_path which lead to the wiki debug

	/*************** httpd callback	***************************************/
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();
public:
	/*************** ctor/dtor	***************************************/
	wikidbg_http_t(httpd_t *httpd_server, const std::string &url_path)	throw();
	~wikidbg_http_t()							throw();
	
	/*************** query function	***************************************/
	const std::string &	get_url_path()		const throw()	{ return url_path;	}
	std::string	build_url(const std::string &keyword, void *object_ptr)			const throw();
	std::string	build_form_hidden_param(const std::string &keyword, void *object_ptr)	const throw();	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_WIKIDBG_HTTP_HPP__  */



