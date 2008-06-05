/*! \file
    \brief Definition of the unit test for the \ref base64_t

*/

/* system include */
/* local include */
#include "neoip_base64_nunit.hpp"
#include "neoip_base64.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	base64_testclass_t::norm_alphabet(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");

	// define the alphabet to use in the base64_t operation
	std::string alphabet	= base64_t::norm_alphabet;
	// some 'test vector' from the rfc3548
	char *	testvect[]	= {  	"\x14\xfb\x9c\x03\xd9\x7e",
					"\x14\xfb\x9c\x03\xd9",
					"\x14\xfb\x9c\x03"};
	char *	testres[]	= {	"FPucA9l+",
					"FPucA9k=",
					"FPucAw=="};
	// go thru all the test vectors
	for(size_t i = 0 ; i < sizeof(testvect)/sizeof(testvect[0]); i++ ){
		// do the convertion from the binary to the base64
		std::string	result	= base64_t::encode(testvect[i], strlen(testvect[i]), alphabet);
		// compare that the result is the supposed one
		if( strcmp(testres[i], result.c_str()) ){
			KLOG_ERR("Test vector " << i << " produced " << result
						<< " instead of " << testres[i]);
			NUNIT_ASSERT( 0 );
		}
		// decode the encoded string
		datum_t	decoded = base64_t::decode(result, alphabet);
		// check the result is equal to the original
		if( std::string(testvect[i]) != string_t::from_datum(decoded) ){
			KLOG_ERR("Decoded vector " << i << " produced invalid result");
			NUNIT_ASSERT( 0 );
		}
	}

	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	base64_testclass_t::safe_alphabet(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");


	// define the alphabet to use in the base64_t operation
	std::string alphabet	= base64_t::safe_alphabet;
	// some 'test vector' from the rfc3548
	char *	testvect[]	= {  	"\x14\xfb\x9c\x03\xd9\x7e",
					"\x14\xfb\x9c\x03\xd9",
					"\x14\xfb\x9c\x03"};
	char *	testres[]	= {	"FPucA9l-",		// NOTE: modified to fit safe_alphabet
					"FPucA9k=",
					"FPucAw=="};
	// go thru all the test vectors
	for(size_t i = 0 ; i < sizeof(testvect)/sizeof(testvect[0]); i++ ){
		// do the convertion from the binary to the base64
		std::string	result	= base64_t::encode(testvect[i], strlen(testvect[i]), alphabet);
		// compare that the result is the supposed one
		if( strcmp(testres[i], result.c_str()) ){
			KLOG_ERR("Test vector " << i << " produced " << result
						<< " instead of " << testres[i]);
			NUNIT_ASSERT( 0 );
		}
		// decode the encoded string
		datum_t	decoded = base64_t::decode(result, alphabet);
		// check the result is equal to the original
		if( std::string(testvect[i]) != string_t::from_datum(decoded) ){
			KLOG_ERR("Decoded vector " << i << " produced invalid result");
			NUNIT_ASSERT( 0 );
		}
	}

	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	base64_testclass_t::self_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	datum_t	datum;
	// log to debug
	KLOG_DBG("enter");

// test for base64_t::norm_alphabet
	// define the alphabet to use in the base64_t operation
	std::string alphabet	= base64_t::norm_alphabet;
	// test on dummy sentence
	datum	= datum_t("helloooo");
	NUNIT_ASSERT( base64_t::decode(base64_t::encode(datum, alphabet), alphabet) == datum);
	// test on dummy sentence	
	datum	= datum_t("hello world! im born today !");
	NUNIT_ASSERT( base64_t::decode(base64_t::encode(datum, alphabet), alphabet) == datum);

// test for base64_t::safe_alphabet
	// define the alphabet to use in the base64_t operation
	alphabet	= base64_t::safe_alphabet;
	// test on dummy sentence
	datum	= datum_t("helloooo");
	NUNIT_ASSERT( base64_t::decode(base64_t::encode(datum, alphabet), alphabet) == datum);
	// test on dummy sentence	
	datum	= datum_t("hello world! im born today !");
	NUNIT_ASSERT( base64_t::decode(base64_t::encode(datum, alphabet), alphabet) == datum);
	

	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

