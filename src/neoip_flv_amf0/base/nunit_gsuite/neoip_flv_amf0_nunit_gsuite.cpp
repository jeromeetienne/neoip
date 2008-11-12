/*! \flv
    \brief Definition of the nunit_gsuite_t for the flv_t layer

*/

/* system include */
/* local include */
#include "neoip_flv_amf0_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_amf0_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void flv_amf0_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	flv_amf0_suite	= nipmem_new nunit_suite_t("flv_amf0");
	// log to debug
	KLOG_DBG("enter");

/********************* amf0 stuff	***************************************/
	// init the testclass for the amf0_build_t
	nunit_testclass_t<amf0_testclass_t> *	amf0_testclass;
	amf0_testclass	= nipmem_new nunit_testclass_t<amf0_testclass_t>("amf0", nipmem_new amf0_testclass_t());
	// add some test functions
	amf0_testclass->append("general"	, &amf0_testclass_t::general);
	// add the testclass to the nunit_suite
	flv_amf0_suite->append(amf0_testclass);

	// add the flv_amf0_suite to the nunit_suite
	nunit_suite.append(flv_amf0_suite);
}

// definition of the nunit_gsuite_t for flv
NUNIT_GSUITE_DEFINITION(flv_amf0_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t
NUNIT_GSUITE_INSERTER(flv_amf0_nunit_gsuite, 0, flv_amf0_gsuite_fct);

NEOIP_NAMESPACE_END




