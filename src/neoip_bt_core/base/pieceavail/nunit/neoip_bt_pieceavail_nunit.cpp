/*! \file
    \brief Definition of the unit test for the \ref bt_pieceavail_t

*/

/* system include */
#include <string>
/* local include */
#include "neoip_bt_pieceavail_nunit.hpp"
#include "neoip_bt_pieceavail.hpp"
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
nunit_res_t	bt_pieceavail_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");


	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

