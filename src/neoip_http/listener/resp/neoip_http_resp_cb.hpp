/*! \file
    \brief Declaration of the http_resp_t
    
*/


#ifndef __NEOIP_HTTP_RESP_CB_HPP__ 
#define __NEOIP_HTTP_RESP_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	http_resp_t;
class	http_reqhd_t;
class	socket_full_t;
class	bytearray_t;

/** \brief the callback class for http_resp_t
 */
class http_resp_cb_t {
public:
	/** \brief callback notified by \ref http_resp_t when to notify an event
	 * 
	 * @return true if the http_resp_t is still valid after the callback
	 */
	virtual bool neoip_http_resp_cb(void *cb_userptr, http_resp_t &cb_http_resp
					, const http_reqhd_t &http_reqhd, socket_full_t *socket_full
					, const bytearray_t &recved_data)	throw() = 0;
	// virtual destructor
	virtual ~http_resp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_RESP_CB_HPP__  */



