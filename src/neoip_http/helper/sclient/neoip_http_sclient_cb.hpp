/*! \file
    \brief Declaration of the http_sclient_t
    
*/


#ifndef __NEOIP_HTTP_SCLIENT_CB_HPP__ 
#define __NEOIP_HTTP_SCLIENT_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class http_sclient_t;
class http_err_t;
class http_sclient_res_t;

/** \brief the callback class for http_client_t
 */
class http_sclient_cb_t {
public:
	/** \brief callback notified by \ref http_sclient_t when to notify an event
	 */
	virtual bool neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &client_res)		throw() = 0;

	/** \brief callback notified only when http_sclient_t has new progress_chunk available
	 * 
	 * - and only if http_sclient_profile_t::progress_chunk_len() != 0
	 * - NOTE: as it isnt pure virtual, it may not be defined if not needed
	 */
	virtual bool neoip_http_sclient_progress_chunk_cb(void *cb_userptr
				, http_sclient_t &cb_sclient)	throw() { return true;	}
	// virtual destructor
	virtual ~http_sclient_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SCLIENT_CB_HPP__  */



