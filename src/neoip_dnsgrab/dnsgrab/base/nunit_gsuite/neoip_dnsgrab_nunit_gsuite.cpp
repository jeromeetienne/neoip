/*! \file
    \brief Definition of the nunit_gsuite_t for the dnsgrab_t layer

*/

/* system include */
/* local include */
#include "neoip_dnsgrab_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_dnsgrab_nunit.hpp"


NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void dnsgrab_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	dnsgrab_suite	= nipmem_new nunit_suite_t("dnsgrab");		
	// log to debug
	KLOG_DBG("enter");

/********************* dnsgrab_t	***************************************/
	// init the testclass for the dnsgrab_t
	nunit_testclass_t<dnsgrab_testclass_t> *	dnsgrab_testclass;
	dnsgrab_testclass	= nipmem_new nunit_testclass_t<dnsgrab_testclass_t>("dnsgrab"
							, nipmem_new dnsgrab_testclass_t());
	// add some test functions
	dnsgrab_testclass->append("test_thru_host2ip", &dnsgrab_testclass_t::test_thru_host2ip);
	// add the testclass to the nunit_suite
	dnsgrab_suite->append(dnsgrab_testclass);

	// add the dnsgrab_suite to the nunit_suite
	nunit_suite.append(dnsgrab_suite);	
}

// definition of the nunit_gsuite_t for dnsgrab
NUNIT_GSUITE_DEFINITION(dnsgrab_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(dnsgrab_nunit_gsuite, 0, dnsgrab_gsuite_fct);

NEOIP_NAMESPACE_END




