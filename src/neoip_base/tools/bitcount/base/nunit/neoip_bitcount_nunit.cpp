/*! \file
    \brief Definition of the unit test for the \ref bitcount_t

*/

/* system include */
#include <string>
/* local include */
#include "neoip_bitcount_nunit.hpp"
#include "neoip_bitcount.hpp"
#include "neoip_bitfield.hpp"
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
nunit_res_t	bitcount_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	bitcount_t	bitcount(6);
	// build a bitfield_t
	bitfield_t	bitfield1(6);
	for(size_t i = 0; i < bitfield1.size(); i += 2)
		bitfield1.set(i);

	// test that add/sub are consistent
	bitcount.inc( bitfield1 );
	bitcount.dec( bitfield1 );
	NUNIT_ASSERT( bitcount == bitcount_t(6) );

	// test the distributed_copy factor
	bitcount	= bitcount_t(6);
	for(size_t i = 0; i < bitcount.size(); i ++)	bitcount.inc(i);
	bitcount.inc(3);
	bitcount.inc(2);
	bitcount.inc(5);
	NUNIT_ASSERT( bitcount.coverage_factor() == 1.5 );
	
	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

