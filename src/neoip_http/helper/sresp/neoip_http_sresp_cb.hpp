/*! \file
    \brief Declaration of the http_sresp_t
    
*/


#ifndef __NEOIP_HTTP_SRESP_CB_HPP__ 
#define __NEOIP_HTTP_SRESP_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	http_sresp_t;
class	http_sresp_ctx_t;

/** \brief the callback class for http_sresp_t
 */
class http_sresp_cb_t {
public:
	/** \brief callback notified by \ref http_sresp_t when to notify an event
	 * 
	 * @return true if the http_sresp_t is still valid after the callback
	 */
	virtual bool neoip_http_sresp_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw() = 0;
	// virtual destructor
	virtual ~http_sresp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SRESP_CB_HPP__  */



