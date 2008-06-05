/*! \file
    \brief Definition of the nunit_gsuite_t for the ndiag_t layer

*/

/* system include */
/* local include */
#include "neoip_ndiag_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #incldue for each nunit
#include "neoip_ndiag_cacheport_nunit.hpp"
#include "neoip_ndiag_aview_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void ndiag_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	ndiag_suite	= nipmem_new nunit_suite_t("ndiag");		
	// log to debug
	KLOG_DBG("enter");

/********************* ndiag_cacheport_t	***************************************/
	// init the testclass for the ndiag_cacheport_t
	nunit_testclass_t<ndiag_cacheport_testclass_t> *	ndiag_cacheport_testclass;
	ndiag_cacheport_testclass	= nipmem_new nunit_testclass_t<ndiag_cacheport_testclass_t>("cacheport", nipmem_new ndiag_cacheport_testclass_t());
	// add some test functions
	ndiag_cacheport_testclass->append("init"		, &ndiag_cacheport_testclass_t::init);
	ndiag_cacheport_testclass->append("contain"	, &ndiag_cacheport_testclass_t::contain);
	ndiag_cacheport_testclass->append("update_remove", &ndiag_cacheport_testclass_t::update_remove);
	// add the testclass to the nunit_suite
	ndiag_suite->append(ndiag_cacheport_testclass);


/********************* ndiag_aview_t	***************************************/
	// init the testclass for the ndiag_aview_t
	nunit_testclass_t<ndiag_aview_testclass_t> *	ndiag_aview_testclass;
	ndiag_aview_testclass	= nipmem_new nunit_testclass_t<ndiag_aview_testclass_t>("aview", nipmem_new ndiag_aview_testclass_t());
	// add some test functions
	ndiag_aview_testclass->append("general"		, &ndiag_aview_testclass_t::general);
	// add the testclass to the nunit_suite
	ndiag_suite->append(ndiag_aview_testclass);

	// add the ndiag_suite to the nunit_suite
	nunit_suite.append(ndiag_suite);	
}

// definition of the nunit_gsuite_t for ndiag
NUNIT_GSUITE_DEFINITION(ndiag_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(ndiag_nunit_gsuite, 0, ndiag_gsuite_fct);

NEOIP_NAMESPACE_END




