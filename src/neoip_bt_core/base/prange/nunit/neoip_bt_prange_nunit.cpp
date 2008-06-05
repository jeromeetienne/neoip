/*! \file
    \brief Definition of the unit test for the \ref bt_prange_t

*/

/* system include */
/* local include */
#include "neoip_bt_prange_nunit.hpp"
#include "neoip_bt_prange.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_prange_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// test is_null
	NUNIT_ASSERT(  bt_prange_t().is_null() );
	NUNIT_ASSERT( !bt_prange_t(1,2,3).is_null() );

	// test field query
	NUNIT_ASSERT( bt_prange_t(1,2,3).piece_idx()	== 1);
	NUNIT_ASSERT( bt_prange_t(1,2,3).offset()	== 2);
	NUNIT_ASSERT( bt_prange_t(1,2,3).length()	== 3);

	// test field set
	NUNIT_ASSERT( bt_prange_t(1,2,3).piece_idx(42).piece_idx()	== 42);
	NUNIT_ASSERT( bt_prange_t(1,2,3).offset(42).offset()		== 42);
	NUNIT_ASSERT( bt_prange_t(1,2,3).length(42).length()		== 42);

	// report no error
	return NUNIT_RES_OK;
}

/** \brief Test function
 */
nunit_res_t	bt_prange_testclass_t::comparison(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// test field query
	NUNIT_ASSERT( bt_prange_t( 1, 2000, 1024) == bt_prange_t( 1, 2000, 1024) );
	NUNIT_ASSERT( bt_prange_t( 1, 2000, 1024) >  bt_prange_t( 1, 2000,  100) );
	NUNIT_ASSERT( bt_prange_t( 1, 2000, 1024) <  bt_prange_t( 1, 2000, 1500) );
	NUNIT_ASSERT( bt_prange_t( 1, 2000, 1024) != bt_prange_t( 1, 2000, 1500) );
	// report no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

