/*! \file
    \brief Definition of the unit test for the \ref socket_client_t

\par Brief Description
this nunit performs a 'in-memory' tls connection establishement between
tls_itor_t and tls_resp_t. then exchange data in both direction thru
the 2 tls_full_t created.

*/

/* system include */
/* local include */
#include "neoip_tls_nunit.hpp"
#include "neoip_tls.hpp"
#include "neoip_x509.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function - perform a tls connection 'in memory'
 */
nunit_res_t	tls_testclass_t::cnx_anon2anon(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	// build the resp_profile
	tls_profile_t	resp_profile;
	resp_profile.authtype	( tls_authtype_t::ANON );
	// build the itor_profile
	tls_profile_t	itor_profile;
	itor_profile.authtype	( tls_authtype_t::ANON );
	// launch a standalone_cnx using those parameters
	nunit_res	= standalone_cnx(resp_profile, itor_profile);
	if( nunit_res.error().failed() )	return nunit_res;
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function - perform a tls connection 'in memory'
 */
nunit_res_t	tls_testclass_t::cnx_cert2cert(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t	nunit_res;
	crypto_err_t	crypto_err;
	x509_privkey_t	ca_privkey;
	x509_request_t	ca_request;
	x509_cert_t	ca_cert;
	x509_privkey_t	user_privkey;
	x509_request_t	user_request;
	x509_cert_t	user_cert;
	
	// generate the ca_cert (a selfsigned one)
	crypto_err	= ca_privkey.generate(512);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= ca_request.generate("main_ca", ca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= ca_cert.generate_selfsigned(ca_request, ca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );

	// generate the user certificate (authenticated by the CA certificate)
	crypto_err	= user_privkey.generate(512);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= user_request.generate("netcat2.4afree.net", user_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= user_cert.generate_from_request(user_request, ca_cert, ca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );

	// check that the user_cert can be authenticate with the ca_cert
	NUNIT_ASSERT( ca_cert.verify_cert(user_cert).succeed() );

	// build the resp_profile
	tls_profile_t	resp_profile;
	resp_profile.authtype		( tls_authtype_t::CERT 	);
	resp_profile.local_privkey	( user_privkey		);
	resp_profile.local_cert_arr()	.append(user_cert);
	// build the itor_profile
	tls_profile_t	itor_profile;
	itor_profile.authtype		( tls_authtype_t::CERT );
	// launch a standalone_cnx using those parameters
	nunit_res	= standalone_cnx(resp_profile, itor_profile);
	if( nunit_res.error().failed() )	return nunit_res;
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function - perform a tls connection 'in memory'
 */
nunit_res_t	tls_testclass_t::standalone_cnx(const tls_profile_t &resp_profile
					, const tls_profile_t &itor_profile)	throw()
{
	crypto_err_t	crypto_err;
	tls_itor_t *	tls_itor	= NULL;
	tls_resp_t *	tls_resp	= NULL;
	tls_full_t *	itor_tls_full	= NULL;
	tls_full_t *	resp_tls_full	= NULL;
	// log to debug
	KLOG_DBG("enter");
	
	// Start a tls_itor_t
	tls_itor	= nipmem_new tls_itor_t();
	crypto_err	= tls_itor->profile(itor_profile).start();
	NUNIT_ASSERT( crypto_err.succeed() );
	
	// Start a tls_resp_t
	tls_resp	= nipmem_new tls_resp_t();
	crypto_err	= tls_resp->profile(resp_profile).start();
	NUNIT_ASSERT( crypto_err.succeed() );

	// loop until itor *and* resp consider themselves connected
	pkt_t	pkt_inp;
	while( !itor_tls_full || !resp_tls_full ){
		// pass data from resp to itor - if itor_tls_full doenst exist
		// - first iteration, pkt_inp is empty and start tls_itor_t
		if( !itor_tls_full ){
			// notify a empty pkt_t to tls_itor_t to get it started
			crypto_err	= tls_itor->notify_recved_data(pkt_inp, &itor_tls_full);
			if( crypto_err.failed() )	KLOG_ERR(crypto_err);
			NUNIT_ASSERT( crypto_err.succeed() );
			KLOG_DBG("pkt_inp="	<< pkt_inp);
			KLOG_DBG("xmit_buffer=" << tls_itor->xmit_buffer());
			// copy tls_itor->xmit_buffer into pkt_inp - to pass it to resp
			pkt_inp		= tls_itor->xmit_buffer();
			// flush tls_itor->xmit_buffer();
			tls_itor->xmit_buffer().head_free( tls_itor->xmit_buffer().length() );		
		}
		
		// pass data from itor to resp - if resp_tls_full doesnt exist
		if( !resp_tls_full ){
			crypto_err	= tls_resp->notify_recved_data(pkt_inp, &resp_tls_full);
			if( crypto_err.failed() )	KLOG_ERR(crypto_err);
			NUNIT_ASSERT( crypto_err.succeed() );
			KLOG_DBG("pkt_inp="	<< pkt_inp);
			KLOG_DBG("xmit_buffer=" << tls_resp->xmit_buffer());
			// copy tls_resp->xmit_buffer into pkt_inp - to pass it to itor
			pkt_inp		= tls_resp->xmit_buffer();
			// flush tls_resp->xmit_buffer();
			tls_resp->xmit_buffer().head_free( tls_resp->xmit_buffer().length() );		
		}
	}
	// delete tls_itor_t
	nipmem_zdelete tls_itor;
	// delete tls_resp_t
	nipmem_zdelete tls_resp;

	
	/*************** Send data from itor_tls_full to resp_tls_full	*******/
	datum_t		i2r_data("dummy data itor to resp");	
	crypto_err	= itor_tls_full->notify_toxmit_data( i2r_data );
	NUNIT_ASSERT( crypto_err.succeed() );
	// pass data from itor_tls_full to resp_tls_full
	crypto_err	= resp_tls_full->notify_recved_data(itor_tls_full->xmit_buffer());
	NUNIT_ASSERT( crypto_err.succeed() );
	// sanity check - resp_tls_full->recv_buffer MUST be equal to i2r_data
	NUNIT_ASSERT( resp_tls_full->recv_buffer().to_datum() == i2r_data);
	
	/*************** Send data from resp_tls_full to itor_tls_full	*******/
	datum_t		r2i_data("dummy data resp to itor");	
	crypto_err	= resp_tls_full->notify_toxmit_data( r2i_data );
	NUNIT_ASSERT( crypto_err.succeed() );
	// pass data from resp_tls_full to itor_tls_full
	crypto_err	= itor_tls_full->notify_recved_data(resp_tls_full->xmit_buffer());
	NUNIT_ASSERT( crypto_err.succeed() );
	// sanity check - itor_tls_full->recv_buffer MUST be equal to r2i_data
	NUNIT_ASSERT( itor_tls_full->recv_buffer().to_datum() == r2i_data);

// TODO do something to test tls_full_t->notify_gracefull_shutdown()
	
	// delete tls_full_t
	nipmem_zdelete itor_tls_full;
	// delete tls_full_t
	nipmem_zdelete resp_tls_full;
	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

