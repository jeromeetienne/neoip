/*! \flv
    \brief Definition of the nunit_gsuite_t for the flv_t layer

*/

/* system include */
/* local include */
#include "neoip_flv_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_flv_parse_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void flv_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	flv_suite	= nipmem_new nunit_suite_t("flv");
	// log to debug
	KLOG_DBG("enter");

/********************* flv_parse_t	***************************************/
	// init the testclass for the flv_parse_t
	nunit_testclass_t<flv_parse_testclass_t> *	flv_parse_testclass;
	flv_parse_testclass	= nipmem_new nunit_testclass_t<flv_parse_testclass_t>("parse", nipmem_new flv_parse_testclass_t());
	// add some test functions
	flv_parse_testclass->append("general"	, &flv_parse_testclass_t::general);
	// add the testclass to the nunit_suite
	flv_suite->append(flv_parse_testclass);

	// add the flv_suite to the nunit_suite
	nunit_suite.append(flv_suite);
}

// definition of the nunit_gsuite_t for flv
NUNIT_GSUITE_DEFINITION(flv_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t
NUNIT_GSUITE_INSERTER(flv_nunit_gsuite, 0, flv_gsuite_fct);

NEOIP_NAMESPACE_END




