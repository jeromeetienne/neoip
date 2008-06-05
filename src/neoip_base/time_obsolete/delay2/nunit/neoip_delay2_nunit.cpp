/*! \file
    \brief Definition of the unit test for the \ref delay2_t

*/

/* system include */
/* local include */
#include "neoip_delay2_nunit.hpp"
#include "neoip_delay2.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a delay2_arg_t
 */
nunit_res_t	delay2_testclass_t::basic_query(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// test is_null()
	NUNIT_ASSERT( delay2_t().is_null() );
	// test is_infinite()
	NUNIT_ASSERT( delay2_t::INFINITE.is_infinite() );

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
nunit_res_t	delay2_testclass_t::comparison_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	delay2_t	delay1	= delay2_t::from_sec(1);
	delay2_t	delay2	= delay2_t::from_sec(2);
	// test comparison operator
	NUNIT_ASSERT( delay1 == delay1 );
	NUNIT_ASSERT( delay1 != delay2 );
	NUNIT_ASSERT( delay1 <  delay2 );
	NUNIT_ASSERT( delay1 <= delay1 );
	NUNIT_ASSERT( delay2 >  delay1 );
	NUNIT_ASSERT( delay2 >= delay1 );
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
nunit_res_t	delay2_testclass_t::arithmetic_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// test arithmetic operator for non infinit operand
	NUNIT_ASSERT( delay2_t::from_sec(1) + delay2_t::from_sec(2) == delay2_t::from_sec(3) );
	NUNIT_ASSERT( delay2_t::from_sec(2) - delay2_t::from_sec(1) == delay2_t::from_sec(1) );
	// test arithmetic operator when one operand is infinite
	NUNIT_ASSERT( delay2_t::INFINITE - delay2_t::from_sec(1) == delay2_t::INFINITE );
	NUNIT_ASSERT( delay2_t::from_sec(1) + delay2_t::INFINITE == delay2_t::INFINITE );
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
nunit_res_t	delay2_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// init some variable
	delay2_t	delay_toserial = delay2_t::from_sec(42);
	delay2_t	delay_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << delay_toserial;
	serial >> delay_unserial;
	// return the result
	NUNIT_ASSERT(delay_toserial == delay_unserial);
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

