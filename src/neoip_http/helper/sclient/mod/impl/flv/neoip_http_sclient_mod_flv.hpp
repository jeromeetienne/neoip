/*! \file
    \brief Header of the http_sclient_mod_flv_t
    
*/


#ifndef __NEOIP_HTTP_SCLIENT_MOD_FLV_HPP__ 
#define __NEOIP_HTTP_SCLIENT_MOD_FLV_HPP__ 
/* system include */
/* local include */
#include "neoip_http_sclient_mod_vapi.hpp"
#include "neoip_http_sclient_mod_type.hpp"
#include "neoip_file_range.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_sclient_t;
class	http_err_t;
class	http_reqhd_t;
class	http_rephd_t;
class	bytearray_t;

/** \brief Handle the module 'flv' to deliver flv
 */
class http_sclient_mod_flv_t : public http_sclient_mod_vapi_t, NEOIP_COPY_CTOR_DENY {
private:
	http_sclient_t *	m_http_sclient;
	http_sclient_mod_type_t	m_type;

	/*************** Internal state	***************************************/
	bool			m_flvhd_removed;	//!< true if flv header has been removed
	file_range_t		m_caller_range;		//!< the range requested by the caller
public:
	/*************** ctor/dtor	***************************************/
	http_sclient_mod_flv_t() 		throw();
	~http_sclient_mod_flv_t()		throw();

	/*************** Setup function	***************************************/
	http_err_t	start(http_sclient_t *m_http_sclient)	throw();

	/*************** http_sclient_mod_vapi_t	*******************************/
	const http_sclient_mod_type_t &	type()	const throw()	{ return m_type;	}
	http_err_t	reqhd_caller2server(http_reqhd_t *http_reqhd)		const throw();
	http_err_t	rephd_server2caller(http_rephd_t *http_rephd_ptr
					, const http_reqhd_t &caller_reqhd) 	const throw();
	file_size_t	contentlen_server2internal(const http_rephd_t &server_rephd
					, const http_reqhd_t &caller_reqhd)	throw();
	http_err_t	reply_body_server2caller(bytearray_t &reply_body
					, const http_reqhd_t &caller_reqhd)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SCLIENT_MOD_FLV_HPP__  */



