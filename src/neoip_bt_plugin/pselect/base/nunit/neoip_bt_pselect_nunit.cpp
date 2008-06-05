/*! \file
    \brief Definition of the unit test for the \ref bt_pselect_vapi_t

- FIXME make a real nunit and for both disk and buff
*/

/* system include */
/* local include */
#include "neoip_bt_pselect_nunit.hpp"
#include "neoip_bt_pselect_fixed.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_pselect_testclass_t::pselect_fixed(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// test is_null()
	NUNIT_ASSERT(  bt_pselect_fixed_t().is_null() );
	NUNIT_ASSERT( !bt_pselect_fixed_t(4).is_null() );

	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

