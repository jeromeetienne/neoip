/*! \file
    \brief Definition of the nunit_gsuite_t for the neoip_bt_cast layer

*/

/* system include */
/* local include */
#include "neoip_bt_cast_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_bt_httpi_nunit.hpp"
#include "neoip_bt_cast_mdata_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void bt_cast_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	bt_cast_suite	= nipmem_new nunit_suite_t("bt_cast");	
	// log to debug
	KLOG_DBG("enter");

#if 0	// NOTE: this one is more a testbed than a unittest
/********************* bt_httpi_t	***************************************/
	// init the testclass for the bt_httpi_t
	nunit_testclass_t<bt_httpi_testclass_t> *	bt_httpi_testclass;
	bt_httpi_testclass	= nipmem_new nunit_testclass_t<bt_httpi_testclass_t>("httpi"
							, nipmem_new bt_httpi_testclass_t());
	// add some test functions
	bt_httpi_testclass->append("general"	, &bt_httpi_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_cast_suite->append(bt_httpi_testclass);
#endif
	
/********************* bt_cast_mdata_server_t	***************************************/
	// init the testclass for the bt_cast_mdata_server_t
	nunit_testclass_t<bt_cast_mdata_testclass_t> *	bt_cast_mdata_testclass;
	bt_cast_mdata_testclass	= nipmem_new nunit_testclass_t<bt_cast_mdata_testclass_t>("mdata"
							, nipmem_new bt_cast_mdata_testclass_t());
	// add some test functions
	bt_cast_mdata_testclass->append("general"	, &bt_cast_mdata_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_cast_suite->append(bt_cast_mdata_testclass);


	// add the bt_cast_suite to the nunit_suite
	nunit_suite.append(bt_cast_suite);
}

// definition of the nunit_gsuite_t for bt_tracker
NUNIT_GSUITE_DEFINITION(bt_cast_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(bt_cast_nunit_gsuite, 0, bt_cast_gsuite_fct);

NEOIP_NAMESPACE_END




