/*! \file
    \brief Header of the neoip_httpd class
    
*/


#ifndef __NEOIP_HTTPD_HANDLER_CB_HPP__ 
#define __NEOIP_HTTPD_HANDLER_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_httpd_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class httpd_request_t;
class httpd_t;
class datum_t;

/** \brief the callback class for httpd_request_t
 */
class httpd_handler_cb_t {
public:
	/** \brief callback notified when a httpd request is received by this \ref httpd_handler_cb_t
	 * 
	 * @return	the http_err_t to indicate the reply http code or internal communication
	 *              e.g. httpd_err_t::DELAYED_REPLY
	 */
	virtual httpd_err_t neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)
										throw() = 0;
	virtual ~httpd_handler_cb_t() {};
};



NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTPD_HANDLER_CB_HPP__  */



