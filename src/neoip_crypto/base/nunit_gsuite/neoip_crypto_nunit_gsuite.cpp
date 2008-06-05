/*! \file
    \brief Definition of the nunit_gsuite_t for the crypto_t layer

*/

/* system include */
/* local include */
#include "neoip_crypto_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of include for each testclass
#include "neoip_dh_nunit.hpp"
#include "neoip_x509_nunit.hpp"
#include "neoip_x509_privkey_nunit.hpp"
#include "neoip_x509_request_nunit.hpp"
#include "neoip_x509_cert_nunit.hpp"
#include "neoip_skey_ciph_nunit.hpp"
#include "neoip_skey_auth_nunit.hpp"
#include "neoip_tls_nunit.hpp"
#include "neoip_tls_nunit2.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void crypto_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	crypto_suite	= nipmem_new nunit_suite_t("crypto");		
	// log to debug
	KLOG_DBG("enter");
/********************* dh_t	***************************************/
	// init the testclass for the dh_t
	nunit_testclass_t<dh_testclass_t> *	dh_testclass;
	dh_testclass	= nipmem_new nunit_testclass_t<dh_testclass_t>("dh", nipmem_new dh_testclass_t());
	// add some test functions
	dh_testclass->append("general"	, &dh_testclass_t::general);
	// add the testclass to the nunit_suite
	crypto_suite->append(dh_testclass);


/********************* x509_privkey_t	***************************************/
	// init the testclass for the x509_privkey_t
	nunit_testclass_t<x509_privkey_testclass_t> *	x509_privkey_testclass;
	x509_privkey_testclass	= nipmem_new nunit_testclass_t<x509_privkey_testclass_t>("x509_privkey", nipmem_new x509_privkey_testclass_t());
	// add some test functions
	x509_privkey_testclass->append("serial_consistency"	, &x509_privkey_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	crypto_suite->append(x509_privkey_testclass);

/********************* x509_request_t	***************************************/
	// init the testclass for the x509_request_t
	nunit_testclass_t<x509_request_testclass_t> *	x509_request_testclass;
	x509_request_testclass	= nipmem_new nunit_testclass_t<x509_request_testclass_t>("x509_request", nipmem_new x509_request_testclass_t());
	// add some test functions
	x509_request_testclass->append("serial_consistency"	, &x509_request_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	crypto_suite->append(x509_request_testclass);
	
/********************* x509_cert_t	***************************************/
	// init the testclass for the x509_cert_t
	nunit_testclass_t<x509_cert_testclass_t> *	x509_cert_testclass;
	x509_cert_testclass	= nipmem_new nunit_testclass_t<x509_cert_testclass_t>("x509_cert", nipmem_new x509_cert_testclass_t());
	// add some test functions
	x509_cert_testclass->append("serial_consistency"	, &x509_cert_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	crypto_suite->append(x509_cert_testclass);

/********************* x509_t	***************************************/
	// init the testclass for the x509_t
	nunit_testclass_t<x509_testclass_t> *	x509_testclass;
	x509_testclass	= nipmem_new nunit_testclass_t<x509_testclass_t>("x509", nipmem_new x509_testclass_t());
	// add some test functions
	x509_testclass->append("general"		, &x509_testclass_t::general);
	// add the testclass to the nunit_suite
	crypto_suite->append(x509_testclass);

/********************* skey_ciph_t	***************************************/
	// init the testclass for the skey_ciph_t
	nunit_testclass_t<skey_ciph_testclass_t> *	skey_ciph_testclass;
	skey_ciph_testclass	= nipmem_new nunit_testclass_t<skey_ciph_testclass_t>("skey_ciph", nipmem_new skey_ciph_testclass_t());
	// add some test functions
	skey_ciph_testclass->append("aes_ecb"	, &skey_ciph_testclass_t::aes_ecb);
	skey_ciph_testclass->append("aes_cbc"	, &skey_ciph_testclass_t::aes_cbc);
	skey_ciph_testclass->append("arcfour"	, &skey_ciph_testclass_t::arcfour);
	// add the testclass to the nunit_suite
	crypto_suite->append(skey_ciph_testclass);

/********************* skey_auth_t	***************************************/
	// init the testclass for the skey_auth_t
	nunit_testclass_t<skey_auth_testclass_t> *	skey_auth_testclass;
	skey_auth_testclass	= nipmem_new nunit_testclass_t<skey_auth_testclass_t>("skey_auth", nipmem_new skey_auth_testclass_t());
	// add some test functions
	skey_auth_testclass->append("auth_type"	, &skey_auth_testclass_t::auth_type);
	skey_auth_testclass->append("md5_hmac"	, &skey_auth_testclass_t::md5_hmac);
	skey_auth_testclass->append("md5_nokey"	, &skey_auth_testclass_t::md5_nokey);
	skey_auth_testclass->append("sha1_nokey", &skey_auth_testclass_t::sha1_nokey);
	// add the testclass to the nunit_suite
	crypto_suite->append(skey_auth_testclass);

/********************* tls_t	***************************************/
	// init the testclass for the tls_t
	nunit_testclass_t<tls_testclass_t> *	tls_testclass;
	tls_testclass	= nipmem_new nunit_testclass_t<tls_testclass_t>("tls", nipmem_new tls_testclass_t());
	// add some test functions
	tls_testclass->append("cnx_anon2anon"	, &tls_testclass_t::cnx_anon2anon);
	tls_testclass->append("cnx_cert2cert"	, &tls_testclass_t::cnx_cert2cert);
	// add the testclass to the nunit_suite
	crypto_suite->append(tls_testclass);
	
/********************* tls2_t	***************************************/
	// init the testclass for the tls_t
	nunit_testclass_t<tls_testclass2_t> *	tls_testclass2;
	tls_testclass2	= nipmem_new nunit_testclass_t<tls_testclass2_t>("tls2", nipmem_new tls_testclass2_t());
	// add some test functions
	tls_testclass2->append("cnx_establishment"	, &tls_testclass2_t::cnx_establishement);
	// add the testclass to the nunit_suite
	crypto_suite->append(tls_testclass2);
	
	// add the crypto_suite to the nunit_suite
	nunit_suite.append(crypto_suite);	
}

// definition of the nunit_gsuite_t for crypto
NUNIT_GSUITE_DEFINITION(crypto_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(crypto_nunit_gsuite, 0, crypto_gsuite_fct);

NEOIP_NAMESPACE_END




