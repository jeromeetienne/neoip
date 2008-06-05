/*! \file
    \brief Definition of the \ref tls_privctx_t

\par Brief Description
tls_privctx_t contains all the gnutls specific pointer.
- this allows not to have those pointers in my own .hpp. and thus avoid to
  include "gnutls/gnutls.h" in all my .cpp
- this allows to centralize here many of the specificity of gnutls handling
- TODO make it tunable - this is currently completetly unflexible
  - reason: i dont currently understand the possible options in gnutls

*/


/* system include */
/* local include */
#include "neoip_tls_privctx.hpp"
#include "neoip_tls_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
tls_privctx_t::tls_privctx_t()	throw()
{
	// zero some fields
	m_session	= NULL;
	m_cert_cred	= NULL;
	m_anon_cli_cred	= NULL;
	m_anon_srv_cred	= NULL;
	m_dh_params	= NULL;
}

/** \brief Destructor
 */
tls_privctx_t::~tls_privctx_t()	throw()
{
	// delete m_session if needed
	if( m_session )		gnutls_deinit(m_session);
	// delete m_cert_cred if needed
	if( m_cert_cred )	gnutls_certificate_free_credentials(m_cert_cred);
	// delete m_anon_cli_cred if needed
	if( m_anon_cli_cred )	gnutls_anon_free_client_credentials(m_anon_cli_cred);
	// delete m_anon_srv_cred if needed
	if( m_anon_srv_cred )	gnutls_anon_free_server_credentials(m_anon_srv_cred);
	// delete m_dh_params if needed
	if( m_dh_params )	gnutls_dh_params_deinit( m_dh_params );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Setup this tls_privctx_t for a client
 */
crypto_err_t	tls_privctx_t::start_client(const tls_profile_t &profile)	throw()
{
	// forward to the proper handler depending on profile.authtype
	switch( profile.authtype().value() ){
	case tls_authtype_t::ANON:	return start_client_anon(profile);
	case tls_authtype_t::CERT:	return start_client_cert(profile);
	default:	DBG_ASSERT( 0 );
	}
	// NOTE: this point should never be reached
	DBG_ASSERT( 0 );
	return crypto_err_t::ERROR;
}

/** \brief Setup this tls_privctx_t for a server
 */
crypto_err_t	tls_privctx_t::start_server(const tls_profile_t &profile)	throw()
{
	// forward to the proper handler depending on profile.authtype
	switch( profile.authtype().value() ){
	case tls_authtype_t::ANON:	return start_server_anon(profile);
	case tls_authtype_t::CERT:	return start_server_cert(profile);
	default:	DBG_ASSERT( 0 );
	}
	// NOTE: this point should never be reached
	DBG_ASSERT( 0 );
	return crypto_err_t::ERROR;
	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Setup the tls_privctx_t for client with certificate
 */
crypto_err_t	tls_privctx_t::start_client_cert(const tls_profile_t &profile)	throw()
{
	crypto_err_t	crypto_err;
	int		gnutls_err;
	// sanity check - tlc_privctx_t MUST be null
	DBG_ASSERT( is_null() );
	// sanity check - profile.authtype() MUST be tls_authtype_t::CERT
	DBG_ASSERT( profile.authtype() == tls_authtype_t::CERT );

	/*************** certificate stuff	*******************************/
	// X509 stuff
	gnutls_err	= gnutls_certificate_allocate_credentials(&m_cert_cred);
	if( gnutls_err < 0 )		return crypto_err_from_gnutls(gnutls_err);
	// setup the profile.local_privkey() and profile.local_cert_arr() into m_cert_cred 
	crypto_err	= setup_cred_cert_key(profile);
	if( crypto_err.failed() )	return crypto_err;
	// setup the profile.trusted_cert_arr() into m_cert_cred
	crypto_err	= setup_cred_cert_trust(profile);
	if( crypto_err.failed() )	return crypto_err;


	// initialize tls session with all the common stuff
	crypto_err	= setup_session_common(profile, false);
	if( crypto_err.failed() )	return crypto_err;
	// Allow connections to servers that have X509
	const int cert_type_priority[3] = { GNUTLS_CRT_X509, 0 };
	gnutls_err	= gnutls_certificate_type_set_priority(m_session, cert_type_priority);
	if( gnutls_err < 0 )		return crypto_err_from_gnutls(gnutls_err);
	
	// put the x509 credentials to the current session
	gnutls_err	= gnutls_credentials_set(m_session, GNUTLS_CRD_CERTIFICATE, m_cert_cred);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);

	// return no error
	return crypto_err_t::OK;
}

/** \brief Setup the tls_privctx_t for client with anonymous
 */
crypto_err_t	tls_privctx_t::start_client_anon(const tls_profile_t &profile)	throw()
{
	crypto_err_t	crypto_err;
	int		gnutls_err;
	// sanity check - tlc_privctx_t MUST be null
	DBG_ASSERT( is_null() );
	// sanity check - profile.authtype() MUST be tls_authtype_t::ANON
	DBG_ASSERT( profile.authtype() == tls_authtype_t::ANON );

	// allocate m_anon_cli_cred
	gnutls_err	= gnutls_anon_allocate_client_credentials(&m_anon_cli_cred);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	
	// initialize tls session with all the common stuff
	crypto_err	= setup_session_common(profile, false);
	if( crypto_err.failed() )	return crypto_err;
		
	// set m_session priority to use anon
	const int kx_prio[] = { GNUTLS_KX_ANON_DH, 0 };
	gnutls_err	= gnutls_kx_set_priority(m_session, kx_prio);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	
	// put the anonymous credentials to the current session
	gnutls_err	= gnutls_credentials_set(m_session, GNUTLS_CRD_ANON, m_anon_cli_cred);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);

	// return no error
	return crypto_err_t::OK;
}

/** \brief Setup the tls_privctx_t for server with anonymous
 */
crypto_err_t	tls_privctx_t::start_server_anon(const tls_profile_t &profile)	throw()
{
	crypto_err_t	crypto_err;
	int		gnutls_err;
	// sanity check - tlc_privctx_t MUST be null
	DBG_ASSERT( is_null() );
	// sanity check - profile.authtype() MUST be tls_authtype_t::ANON
	DBG_ASSERT( profile.authtype() == tls_authtype_t::ANON );

	// allocate m_anon_srv_cred
	gnutls_err	= gnutls_anon_allocate_server_credentials(&m_anon_srv_cred);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// generate and set m_dh_params into m_anon_srv_cred
	crypto_err	= setup_dh_params(profile);
	if( crypto_err.failed() )	return crypto_err;
	gnutls_anon_set_server_dh_params(m_anon_srv_cred, m_dh_params);	
	
	// initialize tls session with all the common stuff
	crypto_err	= setup_session_common(profile, true);
	if( crypto_err.failed() )	return crypto_err;
	// set m_session priority to use anon
	const int kx_prio[] = { GNUTLS_KX_ANON_DH, 0 };
	gnutls_err	= gnutls_kx_set_priority(m_session, kx_prio);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	
	// put the anonymous credentials to the current session
	gnutls_err	= gnutls_credentials_set(m_session, GNUTLS_CRD_ANON, m_anon_srv_cred);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);

#if 0	// NOTE: disabled as i dont think this is needed
	// set the minimum size of the prime that will be used for the handshake.
	gnutls_dh_set_prime_bits(m_session, profile.dh_param_nbit());
#endif
	// return no error
	return crypto_err_t::OK;
}


/** \brief Setup the tls_privctx_t for server with certificate
 */
crypto_err_t	tls_privctx_t::start_server_cert(const tls_profile_t &profile)	throw()
{
	int		gnutls_err;
	crypto_err_t	crypto_err;
	// sanity check - tlc_privctx_t MUST be null
	DBG_ASSERT( is_null() );
	// sanity check - profile.authtype() MUST be tls_authtype_t::CERT
	DBG_ASSERT( profile.authtype() == tls_authtype_t::CERT );

	/*************** certificate stuff	*******************************/
	// allocate m_cert_cred
	gnutls_err	= gnutls_certificate_allocate_credentials(&m_cert_cred);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// generate and set m_dh_params into m_cert_cred
	crypto_err	= setup_dh_params(profile);
	if( crypto_err.failed() )	return crypto_err;
	gnutls_certificate_set_dh_params(m_cert_cred, m_dh_params);

	// setup the profile.local_privkey() and profile.local_cert_arr() into m_cert_cred 
	crypto_err	= setup_cred_cert_key(profile);
	if( crypto_err.failed() )	return crypto_err;
	// setup the profile.trusted_cert_arr() into m_cert_cred
	crypto_err	= setup_cred_cert_trust(profile);
	if( crypto_err.failed() )	return crypto_err;
	
	// initialize tls session with all the common stuff
	crypto_err	= setup_session_common(profile, true);
	if( crypto_err.failed() )	return crypto_err;
	// Allow connections to servers that have X509
	const int cert_type_priority[3] = { GNUTLS_CRT_X509, 0 };
	gnutls_err	= gnutls_certificate_type_set_priority(m_session, cert_type_priority);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// put the x509 credentials to the current session
	gnutls_err	= gnutls_credentials_set(m_session, GNUTLS_CRD_CERTIFICATE, m_cert_cred);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);

#if 0	// NOTE: disabled as i dont think this is needed
	// set the minimum size of the prime that will be used for the handshake.
	gnutls_dh_set_prime_bits(m_session, profile.dh_param_nbit());
#endif

	// request client certificate if any.
	gnutls_certificate_server_set_request(m_session, GNUTLS_CERT_REQUEST);

	// return no error
	return crypto_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Common setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Setup the local key 
 */
crypto_err_t	tls_privctx_t::setup_cred_cert_key(const tls_profile_t &profile)	throw()
{
	int	gnutls_err;
	// sanity check - m_cert_cred MUST be already set
	DBG_ASSERT( m_cert_cred );
	// if there is no profile.local_privkey(), return no error now
	if( profile.local_privkey().is_null() )	return crypto_err_t::OK;

	// setup some variable
	gnutls_x509_privkey_t	privkey		= profile.local_privkey().gnutls_privkey;
	// populate the cert_db	
	size_t			cert_db_size	= profile.local_cert_arr().size();
	gnutls_x509_crt_t *	cert_db		= (gnutls_x509_crt_t *)nipmem_alloca(sizeof(gnutls_x509_crt_t)*cert_db_size);
	for(size_t i = 0; i < cert_db_size; i++)
		cert_db[i]	= profile.local_cert_arr()[i].gnutls_cert;
	
	// update the m_cert_cred
	gnutls_err	= gnutls_certificate_set_x509_key(m_cert_cred, cert_db, cert_db_size, privkey);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
		
	// return no error
	return crypto_err_t::OK;
}

/** \brief Setup the trust_cert_arr
 */
crypto_err_t	tls_privctx_t::setup_cred_cert_trust(const tls_profile_t &profile)	throw()
{
	int	gnutls_err;
	// sanity check - m_cert_cred MUST be already set
	DBG_ASSERT( m_cert_cred );
	// if there is no profile.trusted_cert_arr(), return no error now
	if( profile.trusted_cert_arr().empty() )	return crypto_err_t::OK;

	// populate the cert_db	
	size_t			cert_db_size	= profile.trusted_cert_arr().size();
	gnutls_x509_crt_t *	cert_db		= (gnutls_x509_crt_t *)nipmem_alloca(sizeof(gnutls_x509_crt_t)*cert_db_size);
	for(size_t i = 0; i < cert_db_size; i++)
		cert_db[i]	= profile.trusted_cert_arr()[i].gnutls_cert;
	
	// update the m_cert_cred
	gnutls_err	= gnutls_certificate_set_x509_trust(m_cert_cred, cert_db, cert_db_size);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
		
	// return no error
	return crypto_err_t::OK;
}

/** \brief Setup the dh_params
 */
crypto_err_t	tls_privctx_t::setup_dh_params(const tls_profile_t &profile)	throw()
{
	int	gnutls_err;
	// sanity check - m_dh_params MUST NOT be set
	DBG_ASSERT( m_dh_params == NULL );
	// generate dh parameter
	gnutls_err	= gnutls_dh_params_init(&m_dh_params);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	gnutls_err	= gnutls_dh_params_generate2(m_dh_params, profile.dh_param_nbit());
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
		
	// return no error
	return crypto_err_t::OK;
}

/** \brief Setup the common part of the session
 */
crypto_err_t	tls_privctx_t::setup_session_common(const tls_profile_t &profile
							, bool is_server)	throw()
{
	int	gnutls_err;
	// sanity check - m_session MUST NOT be set
	DBG_ASSERT( m_session == NULL );
	// Initialize TLS session
	gnutls_err	= gnutls_init(&m_session, is_server ? GNUTLS_SERVER : GNUTLS_CLIENT);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// Use default priorities
	gnutls_err	= gnutls_set_default_priority(m_session);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// return no error
	return crypto_err_t::OK;
}

NEOIP_NAMESPACE_END

