/*! \file
    \brief Definition of the nunit_gsuite_t for the bt_ez_tracker_t layer

*/

/* system include */
/* local include */
#include "neoip_bt_ez_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_bt_ezswarm_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void bt_ez_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	bt_ez_suite	= nipmem_new nunit_suite_t("bt_ez");	
	// log to debug
	KLOG_DBG("enter");

#if 0
/********************* bt_ezswarm_t	***************************************/
	// init the testclass for the bt_ezswarm_t
	nunit_testclass_t<bt_ezswarm_testclass_t> *	bt_ezswarm_testclass;
	bt_ezswarm_testclass	= nipmem_new nunit_testclass_t<bt_ezswarm_testclass_t>("ezswarm"
							, nipmem_new bt_ezswarm_testclass_t());
	// add some test functions
	bt_ezswarm_testclass->append("general"	, &bt_ezswarm_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_ez_suite->append(bt_ezswarm_testclass);
#endif
	// add the bt_ez_suite to the nunit_suite
	nunit_suite.append(bt_ez_suite);
}

// definition of the nunit_gsuite_t for bt_ez_tracker
NUNIT_GSUITE_DEFINITION(bt_ez_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(bt_ez_nunit_gsuite, 0, bt_ez_gsuite_fct);

NEOIP_NAMESPACE_END




