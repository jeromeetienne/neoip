/*! \file
    \brief Declaration of the http_client_t
    
*/


#ifndef __NEOIP_HTTP_CLIENT_CB_HPP__ 
#define __NEOIP_HTTP_CLIENT_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	http_client_t;
class	http_err_t;
class	http_rephd_t;
class	socket_full_t;
class	bytearray_t;

/** \brief the callback class for http_client_t
 */
class http_client_cb_t {
public:
	/** \brief callback notified by \ref http_client_t when to notify an event
	 * 
	 * @param http_err	contain the error in case of error at the socket level
	 * @param http_rephd	the http_rephd_t replied by the server
	 * @param socket_full	the socket_full_t established by the http_client_t - it is already started
	 * @param recved_data	some data which may have been read ahead (during http_rephd_t reception)
	 * @return a tokeep for http_client_t
	 */
	virtual bool neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
				, const http_err_t &http_err, const http_rephd_t &http_rephd
				, socket_full_t *socket_full, const bytearray_t &recved_data) throw() = 0;

	/** \brief Callback used by http_client_t to get the data to post
	 * 
	 * - NOTE: used only if the http_reqhd_t::method() is a POST
	 * - NOTE: the http_client_t MUST NOT be deleted during this callback 
	 * 
	 * @param offset	the offset within the data to post
	 * @param maxlen	the maximum length of the returned datum_t (ALWAYS > 0)
	 * @return a datum_t of the data to post. it has a maximum length of 'maxlen'
	 * 	   if null, it means no more data are to be posted.
	 */
	virtual datum_t	neoip_http_client_data2post_cb(void *cb_userptr, http_client_t &cb_http_client
					, size_t offset, size_t maxlen)	throw()
					{ return datum_t();	}

	// virtual destructor
	virtual ~http_client_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_CLIENT_CB_HPP__  */



