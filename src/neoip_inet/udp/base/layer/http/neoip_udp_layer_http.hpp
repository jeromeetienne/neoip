/*! \file
    \brief Header of the \ref udp_layer_http_t class
    
*/


#ifndef __NEOIP_UDP_LAYER_HTTP_HPP__ 
#define __NEOIP_UDP_LAYER_HTTP_HPP__ 
/* system include */
/* local include */
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_html_builder.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class udp_layer_t;

/** \brief define a handler for a specific http path
 */
class udp_layer_http_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	udp_layer_t *	layer;	//!< back pointer on the layer
	html_builder_t		h;
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();
	
	std::string	page_header()	throw();
	std::string	http_rootpath() const throw();
	httpd_err_t	homepage_http_cb(httpd_request_t &request)	throw();
	
	std::string	resp_summary()								throw();
	std::string	resp_link(const std::string &label, const std::string &operation)	throw();
	httpd_err_t	resp_list_http_cb(httpd_request_t &request)				throw();
	
	std::string	itor_summary()								throw();
	std::string	itor_link(const std::string &label, const std::string &operation)	throw();
	httpd_err_t	itor_list_http_cb(httpd_request_t &request)				throw();

	std::string	full_summary()								throw();
	std::string	full_link(const std::string &label, const std::string &operation)	throw();
	httpd_err_t	full_list_http_cb(httpd_request_t &request)				throw();	
public:
	// ctor/dtor
	udp_layer_http_t(udp_layer_t *udp_layer)	throw();
	~udp_layer_http_t()					throw();
};



NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_LAYER_HTTP_HPP__  */



