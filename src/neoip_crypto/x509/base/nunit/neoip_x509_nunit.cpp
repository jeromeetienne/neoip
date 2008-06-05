/*! \file
    \brief unit test for the x509 unit test

- the whole x509 layer could be refactored much better
  - all the generate() stuff could be replace by static ::from()
  - all the returning bool for error could be replaced by x509_err_t
  - moreover a lot of it is poorly written
    - to rewrite it would allow a much clean API and code
- the nunit are poor too
  - there is only one and a big one
  - it should be splitted into several small one 
  - all the serialization should be tested
  - as well as the comparison operator
  - and that for all the 3 main objects aka privkey, request, cert
  - ok so one nunit for each main object and one global ?
- ca_cert.verify_cert(ca_cert) fails
  - i dunno the reason, likely a misconfiguration of the x509 certificate
  - i could easily work around this by "if subject cert is equal to issuer cert, subject cert is trusted"
    - this only requires a comparison which is already implemented
- there is a half-backed stuff for encryption
  - it is not used and i have no intention to complete it
  - so remove it

*/

/* system include */
/* local include */
#include "neoip_x509_nunit.hpp"
#include "neoip_x509.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_file.hpp"	// only to read/write in file and test compatibility with external tool 
#include "neoip_bytearray.hpp"	// only to read/write in file and test compatibility with external tool

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	x509_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	x509_privkey_t	ca_privkey;
	x509_request_t	ca_request;
	x509_cert_t	ca_cert;
	x509_privkey_t	user_privkey;
	x509_request_t	user_request;
	x509_cert_t	user_cert;
	crypto_err_t	crypto_err;
	char *		data = "bonjour";
	datum_t		data_datum = datum_t(data, strlen(data));
	datum_t		encrypted_data, decrypted_data;
	datum_t		signature;
	
#if 0
{
	x509_cert_t	test_cert;
	datum_t		cert_datum;
	// load the file
	file_err_t	file_err;
	file_err	= file_sio_t::readall("/tmp/ca-cert.der", cert_datum);
	NUNIT_ASSERT( file_err.succeed() );
	
	test_cert	= x509_cert_t::from_der_datum(cert_datum);
	KLOG_ERR("test_cert=" << test_cert);
}
#endif
	
/*
 *  GENERATE THE CA CERTFICATE (a selfsigned one)
 */
	crypto_err	= ca_privkey.generate(512);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= ca_request.generate("main_ca", ca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= ca_cert.generate_selfsigned(ca_request, ca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );
	// TODO apparantly selfsigned certificate can't authenticate itself... it is an issue
	// - maybe i dont init the certificate properly
	// - see https://trac.prelude-ids.org/browser/trunk/libprelude/prelude-adduser/tls-register.c?rev=6540
	//   for example of gnutls x509 certificate
//	NUNIT_ASSERT( ca_cert.verify_cert(ca_cert) );

	KLOG_ERR("ca_cert=" << ca_cert);

/*
 *  TEST SIGNATURE CONSISTENCY for ca (aka a produced signature must be verifiable)
 */	
	signature = ca_privkey.sign_data(data_datum);
	NUNIT_ASSERT( !signature.is_null() && ca_cert.verify_data(data_datum, signature).succeed() );

/*
 *  GENERATE THE USER CERTFICATE (authenticated by the CA certificate)
 */
	crypto_err	= user_privkey.generate(512);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= user_request.generate("netcat2.4afree.net", user_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= user_cert.generate_from_request(user_request, ca_cert, ca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );
	// check that the user_cert can be authenticate with the ca_cert
	NUNIT_ASSERT( ca_cert.verify_cert(user_cert).succeed() );

	KLOG_ERR("user_cert=" << user_cert);

#if 0
{
	x509_cert_t	test_cert;
	// load the file
	file_err_t	file_err;
	file_err	= file_sio_t::writeall("/tmp/test-user_cert.der", user_cert.to_der_datum());
	NUNIT_ASSERT( file_err.succeed() );
	file_err	= file_sio_t::writeall("/tmp/test-ca_cert.der", ca_cert.to_der_datum());
	NUNIT_ASSERT( file_err.succeed() );
}
#endif
/*
 *  TEST SIGNATURE CONSISTENCY for user (aka a produced signature must be verifiable)
 */	
	signature = user_privkey.sign_data(data_datum);
	NUNIT_ASSERT( !signature.is_null() && user_cert.verify_data(data_datum, signature).succeed() );
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END
