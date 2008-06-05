/*! \file
    \brief Definition of the unit test for the \ref file_range_t

*/

/* system include */
/* local include */
#include "neoip_file_range_nunit.hpp"
#include "neoip_file_range.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_range_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// test is_null
	NUNIT_ASSERT(  file_range_t().is_null() );
	NUNIT_ASSERT( !file_range_t(0, 4).is_null() );

	// test beg()/end() query
	NUNIT_ASSERT(  file_range_t(7, 10).beg() ==  7 );
	NUNIT_ASSERT(  file_range_t(7, 10).end() == 10 );

	// test beg()/end() set
	NUNIT_ASSERT(  file_range_t(7, 10).beg( 5).beg() ==  5 );
	NUNIT_ASSERT(  file_range_t(7, 10).end(15).end() == 15 );

	// test len()
	NUNIT_ASSERT(  file_range_t(7,  7).len() ==  1 );
	NUNIT_ASSERT(  file_range_t(7, 10).len() ==  4 );

	// report no error
	return NUNIT_RES_OK;
}

/** \brief Test function
 */
nunit_res_t	file_range_testclass_t::range_op(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// test fully_include
	NUNIT_ASSERT(  file_range_t(10, 21).fully_include( file_range_t(12, 15) ) );
	NUNIT_ASSERT( !file_range_t(10, 21).fully_include( file_range_t(9 , 15) ) );
	NUNIT_ASSERT( !file_range_t(10, 21).fully_include( file_range_t(9 , 22) ) );
	NUNIT_ASSERT( !file_range_t(10, 21).fully_include( file_range_t(15, 27) ) );

	// test is_distinct
	NUNIT_ASSERT( !file_range_t(10, 21).is_distinct( file_range_t(12, 15) ) );
	NUNIT_ASSERT( !file_range_t(10, 21).is_distinct( file_range_t( 9, 15) ) );
	NUNIT_ASSERT( !file_range_t(10, 21).is_distinct( file_range_t( 9, 22) ) );
	NUNIT_ASSERT( !file_range_t(10, 21).is_distinct( file_range_t(15, 17) ) );
	NUNIT_ASSERT(  file_range_t(10, 21).is_distinct( file_range_t( 5,  7) ) );
	NUNIT_ASSERT(  file_range_t(10, 21).is_distinct( file_range_t(25, 27) ) );

	// test is_contiguous
	NUNIT_ASSERT(  file_range_t(10, 21).is_contiguous( file_range_t( 5,  9) ) );
	NUNIT_ASSERT(  file_range_t(10, 21).is_contiguous( file_range_t(22, 30) ) );
	NUNIT_ASSERT( !file_range_t(10, 21).is_contiguous( file_range_t( 5, 10) ) );
	NUNIT_ASSERT( !file_range_t(10, 21).is_contiguous( file_range_t(21, 30) ) );
	
	// report no error
	return NUNIT_RES_OK;
}

/** \brief Test function
 */
nunit_res_t	file_range_testclass_t::comparison(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	NUNIT_ASSERT(file_range_t(10, 21) >  file_range_t( 5, 10));
	NUNIT_ASSERT(file_range_t(10, 21) == file_range_t(10, 21));
	NUNIT_ASSERT(file_range_t(10, 21) >  file_range_t(10, 15));
	NUNIT_ASSERT(file_range_t(10, 21) != file_range_t(10, 25));
	
	// report no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

