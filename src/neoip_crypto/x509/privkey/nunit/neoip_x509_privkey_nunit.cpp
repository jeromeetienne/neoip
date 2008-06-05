/*! \file
    \brief unit test for the x509_privkey_t unit test

*/

/* system include */
/* local include */
#include "neoip_x509_privkey_nunit.hpp"
#include "neoip_x509.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	x509_privkey_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	x509_privkey_t	ca_privkey;
	crypto_err_t	crypto_err;
	// generate the x509_cert_T
	crypto_err	= ca_privkey.generate(1024);
	NUNIT_ASSERT( crypto_err.succeed() );

	// do the serial_consistency test
	x509_privkey_t	privkey_toserial = ca_privkey;
	x509_privkey_t	privkey_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << privkey_toserial;
	serial >> privkey_unserial;
	// test the serialization consistency
	NUNIT_ASSERT(privkey_toserial == privkey_unserial);

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END
