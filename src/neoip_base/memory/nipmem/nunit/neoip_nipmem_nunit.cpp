/*! \file
    \brief Definition of the unit test for the \ref nipmem_t

- TODO to remove 
  - it is more to test the nipmem while coding it that performing a test
  - so to remove not to keep dead code
*/

/* system include */
/* local include */
#include "neoip_nipmem_nunit.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief function to test a nipmem_t
 */
nunit_res_t	nipmem_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	int *p	= nipmem_new	int;
	nipmem_delete p;
//	if( p );
	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

