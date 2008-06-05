/*! \file
    \brief Header of the \ref tls_privctx_t

*/


#ifndef __NEOIP_TLS_PRIVCTX_HPP__ 
#define __NEOIP_TLS_PRIVCTX_HPP__ 
/* system include */
#include "gnutls/gnutls.h"		// NOTE: it is ok to include it here as this .hpp
					// is included only in .cpp.
/* local include */
#include "neoip_tls_privctx_wikidbg.hpp"
#include "neoip_crypto_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	tls_profile_t;

/** \brief to handle the privctx for get_t
 */
class tls_privctx_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<tls_privctx_t, tls_privctx_wikidbg_init> {
public:
	gnutls_session_t			m_session;
	gnutls_certificate_credentials_t	m_cert_cred;
	gnutls_anon_client_credentials_t	m_anon_cli_cred;
	gnutls_anon_server_credentials_t	m_anon_srv_cred;
	gnutls_dh_params_t			m_dh_params;

	/*************** Internal function	*******************************/
	crypto_err_t	setup_session_common(const tls_profile_t &profile
							, bool is_server)	throw();
	crypto_err_t	setup_cred_cert_key(const tls_profile_t &profile)	throw();
	crypto_err_t	setup_cred_cert_trust(const tls_profile_t &profile)	throw();
	crypto_err_t	setup_dh_params(const tls_profile_t &profile)		throw();
	crypto_err_t	start_client_cert(const tls_profile_t &profile)		throw();
	crypto_err_t	start_client_anon(const tls_profile_t &profile)		throw();
	crypto_err_t	start_server_cert(const tls_profile_t &profile)		throw();
	crypto_err_t	start_server_anon(const tls_profile_t &profile)		throw();
public:
	/*************** ctor/dtor	***************************************/
	tls_privctx_t()		throw();
	~tls_privctx_t()	throw();
	
	/*************** Setup function	***************************************/
	crypto_err_t	start_client(const tls_profile_t &profile)	throw();
	crypto_err_t	start_server(const tls_profile_t &profile)	throw();
	
	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return !m_session;	}
	gnutls_session_t	session()	const throw()	{ DBG_ASSERT(m_session);
								  return m_session;	}

	/*************** List of friend class	*******************************/
	friend class	tls_privctx_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TLS_PRIVCTX_HPP__  */



