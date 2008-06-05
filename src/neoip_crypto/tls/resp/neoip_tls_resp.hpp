/*! \file
    \brief Header of the \ref tls_resp_t

*/


#ifndef __NEOIP_TLS_RESP_HPP__ 
#define __NEOIP_TLS_RESP_HPP__ 
/* system include */
/* local include */
#include "neoip_tls_resp_wikidbg.hpp"
#include "neoip_tls_profile.hpp"
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	tls_full_t;
class	tls_privctx_t;
class	crypto_err_t;

/** \brief This object contains the resp needed for a scrambled connection
 */
class tls_resp_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<tls_resp_t, tls_resp_wikidbg_init> {
private:
	tls_profile_t	m_profile;	//!< the profile for this object
	tls_privctx_t *	m_tls_privctx;	//!< the tls_privctx_t
	pkt_t		m_recv_buffer;	//!< buffer of the data received and not yet consumed
	pkt_t		m_xmit_buffer;	//!< buffer of the data to send
	
	/*************** read/write callback	*******************************/
	friend ssize_t	tls_resp_c_read_cb(void *userptr, void *buf_ptr, size_t buf_len);
	friend ssize_t	tls_resp_c_write_cb(void *userptr, const void *buf_ptr, size_t buf_len);
	ssize_t		cpp_read_cb(void *buf_ptr, size_t buf_len)		throw();
	ssize_t		cpp_write_cb(const void *buf_ptr, size_t buf_len)	throw();
public:
	/*************** ctor/dtor	***************************************/
	tls_resp_t()	throw();
	~tls_resp_t()	throw();
	
	/*************** Setup function	***************************************/
	tls_resp_t&	profile(const tls_profile_t &profile)	throw();
	crypto_err_t	start()					throw();

	/*************** action function	*******************************/
	crypto_err_t	notify_recved_data(const pkt_t &recved_pkt
					, tls_full_t **tls_full_out)	throw(); 
	pkt_t &		xmit_buffer()	throw()	{ return m_xmit_buffer;	}
	
	/*************** List of friend class	*******************************/
	friend class	tls_resp_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TLS_RESP_HPP__  */



