/*! \flv
    \brief Definition of the nunit_gsuite_t for the flv_t layer

*/

/* system include */
/* local include */
#include "neoip_rtmp_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_rtmp_resp_nunit.hpp"
#include "neoip_rtmp_build_nunit.hpp"
#include "neoip_rtmp_parse_nunit.hpp"
#include "neoip_amf0_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void rtmp_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	rtmp_suite	= nipmem_new nunit_suite_t("rtmp");
	// log to debug
	KLOG_DBG("enter");

/********************* amf0 stuff	***************************************/
	// init the testclass for the amf0_build_t
	nunit_testclass_t<amf0_testclass_t> *	amf0_testclass;
	amf0_testclass	= nipmem_new nunit_testclass_t<amf0_testclass_t>("amf0", nipmem_new amf0_testclass_t());
	// add some test functions
	amf0_testclass->append("general"	, &amf0_testclass_t::general);
	// add the testclass to the nunit_suite
	rtmp_suite->append(amf0_testclass);

/********************* rtmp_build_t	***************************************/
	// init the testclass for the rtmp_build_t
	nunit_testclass_t<rtmp_build_testclass_t> *	rtmp_build_testclass;
	rtmp_build_testclass	= nipmem_new nunit_testclass_t<rtmp_build_testclass_t>("build", nipmem_new rtmp_build_testclass_t());
	// add some test functions
	rtmp_build_testclass->append("general"	, &rtmp_build_testclass_t::general);
	// add the testclass to the nunit_suite
	rtmp_suite->append(rtmp_build_testclass);

/********************* rtmp_parse_t	***************************************/
	// init the testclass for the rtmp_parse_t
	nunit_testclass_t<rtmp_parse_testclass_t> *	rtmp_parse_testclass;
	rtmp_parse_testclass	= nipmem_new nunit_testclass_t<rtmp_parse_testclass_t>("parse", nipmem_new rtmp_parse_testclass_t());
	// add some test functions
	rtmp_parse_testclass->append("general"	, &rtmp_parse_testclass_t::general);
	// add the testclass to the nunit_suite
	rtmp_suite->append(rtmp_parse_testclass);

/********************* rtmp_resp_t	***************************************/
	// init the testclass for the rtmp_resp_t
	nunit_testclass_t<rtmp_resp_testclass_t> *	rtmp_resp_testclass;
	rtmp_resp_testclass	= nipmem_new nunit_testclass_t<rtmp_resp_testclass_t>("resp", nipmem_new rtmp_resp_testclass_t());
	// add some test functions
	rtmp_resp_testclass->append("general"	, &rtmp_resp_testclass_t::general);
	// add the testclass to the nunit_suite
	rtmp_suite->append(rtmp_resp_testclass);

	// add the rtmp_suite to the nunit_suite
	nunit_suite.append(rtmp_suite);
}

// definition of the nunit_gsuite_t for flv
NUNIT_GSUITE_DEFINITION(rtmp_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t
NUNIT_GSUITE_INSERTER(rtmp_nunit_gsuite, 0, rtmp_gsuite_fct);

NEOIP_NAMESPACE_END




