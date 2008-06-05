/*! \file
    \brief Definition of the unit test for the \ref file_size_t

*/

/* system include */
/* local include */
#include "neoip_file_size_nunit.hpp"
#include "neoip_file_size.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test file_size
 */
nunit_res_t	file_size_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// test the serialization consistency
	NUNIT_ASSERT(  file_size_t().is_null() );
	NUNIT_ASSERT( !file_size_t(4242).is_null() );
	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the serial consistency
 */
nunit_res_t	file_size_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	file_size_t	file_size_toserial = 0x1122334455667788ULL;
	file_size_t	file_size_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << file_size_toserial;
	serial >> file_size_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( file_size_toserial == file_size_unserial );
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

