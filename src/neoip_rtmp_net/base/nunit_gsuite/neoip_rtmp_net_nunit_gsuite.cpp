/*! \flv
    \brief Definition of the nunit_gsuite_t for the flv_t layer

*/

/* system include */
/* local include */
#include "neoip_rtmp_net_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_rtmp_resp_nunit.hpp"
NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void rtmp_net_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	rtmp_net_suite	= nipmem_new nunit_suite_t("rtmp_net");
	// log to debug
	KLOG_DBG("enter");

/********************* rtmp_resp_t	***************************************/
	// init the testclass for the rtmp_resp_t
	nunit_testclass_t<rtmp_resp_testclass_t> *	rtmp_resp_testclass;
	rtmp_resp_testclass	= nipmem_new nunit_testclass_t<rtmp_resp_testclass_t>("resp", nipmem_new rtmp_resp_testclass_t());
	// add some test functions
	rtmp_resp_testclass->append("general"	, &rtmp_resp_testclass_t::general);
	// add the testclass to the nunit_suite
	rtmp_net_suite->append(rtmp_resp_testclass);

	// add the rtmp_net_suite to the nunit_suite
	nunit_suite.append(rtmp_net_suite);
}

// definition of the nunit_gsuite_t for flv
NUNIT_GSUITE_DEFINITION(rtmp_net_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t
NUNIT_GSUITE_INSERTER(rtmp_net_nunit_gsuite, 0, rtmp_net_gsuite_fct);

NEOIP_NAMESPACE_END




