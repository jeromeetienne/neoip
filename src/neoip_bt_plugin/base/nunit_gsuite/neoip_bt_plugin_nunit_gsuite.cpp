/*! \file
    \brief Definition of the nunit_gsuite_t for the bt_tracker_t layer

*/

/* system include */
/* local include */
#include "neoip_bt_plugin_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void bt_plugin_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	bt_plugin_suite	= nipmem_new nunit_suite_t("bt_plugin");	
	// log to debug
	KLOG_DBG("enter");

	// add the bt_plugin_suite to the nunit_suite
	nunit_suite.append(bt_plugin_suite);
}

// definition of the nunit_gsuite_t for bt_tracker
NUNIT_GSUITE_DEFINITION(bt_plugin_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(bt_plugin_nunit_gsuite, 0, bt_plugin_gsuite_fct);

NEOIP_NAMESPACE_END




