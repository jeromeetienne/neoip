/*! \file
    \brief Declaration of the http_sclient_mod_vapi_t

\par Brief Description
a \ref http_sclient_mod_vapi_t is a object which handling the moding the 
normal behaviour of http_sclient_t. normal as in http_sclient_mod_raw_t
    
*/

#ifndef __NEOIP_HTTP_SCLIENT_MOD_VAPI_HPP__ 
#define __NEOIP_HTTP_SCLIENT_MOD_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_sclient_mod_type_t;
class	http_err_t;
class	http_reqhd_t;
class	http_rephd_t;
class	file_size_t;
class	bytearray_t;

/** \brief Virtual API for the http_sclient_mod_t stuff
 * 
 * - various hook attached to http_sclient_t depending on how they should 
 *   be processed.
 */
class http_sclient_mod_vapi_t {
public:
	/** \brief return the http_sclient_mod_type_t of this implementation 
	 */
	virtual const http_sclient_mod_type_t &	type()				const throw() = 0;
	/** \brief hook called to convert http_reqhd_t from a caller view to a server view
	 * @return an http_err_t if http_err.failed() stop processing immediatly
	 */
	virtual http_err_t	reqhd_caller2server(http_reqhd_t *http_reqhd)	const throw() = 0;

	/** \brief hook called to convert http_rephd_t from a server view to a caller view
	 * @return an http_err_t if http_err.failed() stop processing immediatly
	 */
	virtual http_err_t	rephd_server2caller(http_rephd_t *http_rephd_ptr
					, const http_reqhd_t &caller_reqhd) 	const throw() = 0;

	/** \brief hook called to get the 'internal' content_length from a server http_rephd_t
	 * @return a file_size_t if file_size.is_null() then stop processing immediatly
	 */
	virtual file_size_t	contentlen_server2internal(const http_rephd_t &server_rephd
					, const http_reqhd_t &caller_reqhd)	throw() = 0;

	/** \brief hook called to convert reply_body from a server view to a caller view
	 * @return an http_err_t if http_err.failed() stop processing immediatly
	 */
	virtual http_err_t	reply_body_server2caller(bytearray_t &reply_body
					, const http_reqhd_t &caller_reqhd)	throw() = 0;

	// virtual destructor
	virtual ~http_sclient_mod_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SCLIENT_MOD_VAPI_HPP__  */



