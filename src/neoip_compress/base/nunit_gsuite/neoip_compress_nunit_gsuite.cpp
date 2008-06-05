/*! \file
    \brief Definition of the nunit_gsuite_t for the compress_t layer

*/

/* system include */
/* local include */
#include "neoip_compress_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"
#include "neoip_compress_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void compress_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	compress_suite	= nipmem_new nunit_suite_t("compress");		
	// log to debug
	KLOG_DBG("enter");
/********************* compress_t	***************************************/
	// init the testclass for the compress_t
	nunit_testclass_t<compress_testclass_t> *	compress_testclass;
	compress_testclass	= nipmem_new nunit_testclass_t<compress_testclass_t>("compress_t"
							, nipmem_new compress_testclass_t());
	// add some test functions
	compress_testclass->append("test_identity", &compress_testclass_t::test_identity);
	// add the testclass to the nunit_suite
	compress_suite->append(compress_testclass);

	// add the compress_suite to the nunit_suite
	nunit_suite.append(compress_suite);	
}

// definition of the nunit_gsuite_t for compress
NUNIT_GSUITE_DEFINITION(compress_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(compress_nunit_gsuite, 0, compress_gsuite_fct);

NEOIP_NAMESPACE_END




