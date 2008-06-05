/*! \file
    \brief unit test for the x509_request_t unit test

*/

/* system include */
/* local include */
#include "neoip_x509_request_nunit.hpp"
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
nunit_res_t	x509_request_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	x509_privkey_t	ca_privkey;
	x509_request_t	ca_request;
	crypto_err_t	crypto_err;
	// generate the x509_request
	crypto_err	= ca_privkey.generate(512);
	NUNIT_ASSERT( crypto_err.succeed() );
	crypto_err	= ca_request.generate("main_ca", ca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );

	// do the serial_consistency test
	x509_request_t	request_toserial = ca_request;
	x509_request_t	request_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << request_toserial;
	serial >> request_unserial;
	// test the serialization consistency
	NUNIT_ASSERT(request_toserial == request_unserial);

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END
