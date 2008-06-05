/*! \file
    \brief Definition of the unit test for the \ref bitflag_t

*/

/* system include */
/* local include */
#include "neoip_bitflag_nunit.hpp"
#include "neoip_bitflag.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// all the declaration - typically in a .hpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)					\
		COMMAND(bitflag_test_t, uint64_t, flag1, FLAG1, 0)		\
		COMMAND(bitflag_test_t, uint64_t, flag2, FLAG2, 1)		\
		COMMAND(bitflag_test_t, uint64_t, flag3, FLAG3, 2)		\
		COMMAND(bitflag_test_t, uint64_t, flag4, FLAG4, 3)

NEOIP_BITFLAG_DECLARATION_START	(bitflag_test_t, uint64_t, NEOIP_BITFLAG_COMMAND_LIST)
NEOIP_BITFLAG_DECLARATION_END	(bitflag_test_t, uint64_t, NEOIP_BITFLAG_COMMAND_LIST)
#undef NEOIP_BITFLAG_COMMAND_LIST


// al the definitions - typically in a .cpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)					\
		COMMAND(bitflag_test_t, uint64_t, flag1, FLAG1, 0)		\
		COMMAND(bitflag_test_t, uint64_t, flag2, FLAG2, 1)		\
		COMMAND(bitflag_test_t, uint64_t, flag3, FLAG3, 2)		\
		COMMAND(bitflag_test_t, uint64_t, flag4, FLAG4, 3)

NEOIP_BITFLAG_DEFINITION	(bitflag_test_t, uint64_t, NEOIP_BITFLAG_COMMAND_LIST)
#undef NEOIP_BITFLAG_COMMAND_LIST


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bitflag_testclass_t::basic_op(const nunit_testclass_ftor_t &testclass_ftor)	throw()
{
	// log to debug
	KLOG_DBG("enter");	
	// check that the default value is 0
	NUNIT_ASSERT( bitflag_test_t().value() == 0 );
	// test the is_optname() function
	NUNIT_ASSERT( bitflag_test_t::FLAG2.is_flag2() );
	// test a is_distinct()
	NUNIT_ASSERT( bitflag_test_t::FLAG2.is_distinct(bitflag_test_t::FLAG3) );
	// test a fully_include()
	NUNIT_ASSERT( (bitflag_test_t::FLAG3|bitflag_test_t::FLAG2).fully_include(bitflag_test_t::FLAG3));
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
nunit_res_t	bitflag_testclass_t::boolean_op(const nunit_testclass_ftor_t &testclass_ftor)	throw()
{
	bitflag_test_t	bitflag1	= bitflag_test_t::FLAG1 | bitflag_test_t::FLAG2;
	bitflag_test_t	bitflag2	= bitflag_test_t::FLAG2 | bitflag_test_t::FLAG4;
	// test the AND operator
	NUNIT_ASSERT( (bitflag1 & bitflag2) == bitflag_test_t::FLAG2 );	
	// test the OR operator
	NUNIT_ASSERT( (bitflag1 | bitflag2) == (bitflag_test_t::FLAG1|bitflag_test_t::FLAG2|bitflag_test_t::FLAG4));
	// test the XOR operator
	NUNIT_ASSERT( (bitflag1 ^ bitflag2) == (bitflag_test_t::FLAG1|bitflag_test_t::FLAG4) );
	// test the NOT operator
	NUNIT_ASSERT( (~bitflag2).is_distinct(bitflag_test_t::FLAG2 | bitflag_test_t::FLAG4) );
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
nunit_res_t	bitflag_testclass_t::comparison(const nunit_testclass_ftor_t &testclass_ftor)	throw()
{
	bitflag_test_t	bitflag1	= bitflag_test_t::FLAG3;
	bitflag_test_t	bitflag2	= bitflag_test_t::FLAG2;
	// test various comparison
	NUNIT_ASSERT( bitflag1 == bitflag1 );
	NUNIT_ASSERT( bitflag1 <= bitflag1 );
	NUNIT_ASSERT( bitflag1 >= bitflag1 );
	NUNIT_ASSERT( bitflag1 != bitflag2 );
	NUNIT_ASSERT( bitflag1 >  bitflag2 );
	NUNIT_ASSERT( bitflag2 <  bitflag1 );
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
nunit_res_t	bitflag_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// init some variable
	bitflag_test_t	bitflag_toserial = bitflag_test_t::FLAG2 | bitflag_test_t::FLAG3;
	bitflag_test_t	bitflag_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << bitflag_toserial;
	serial >> bitflag_unserial;
	// check that both are equal
	NUNIT_ASSERT( bitflag_toserial == bitflag_unserial );
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

