/*! \flv
    \brief Definition of the nunit_gsuite_t for the flv_t layer

*/

/* system include */
/* local include */
#include "neoip_rtmp_cam_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_rtmp_cam_full_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void rtmp_cam_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	rtmp_cam_suite	= nipmem_new nunit_suite_t("rtmp_cam");
	// log to debug
	KLOG_DBG("enter");

/********************* rtmp_cam_full_t	***************************************/
	// init the testclass for the rtmp_cam_full_t
	nunit_testclass_t<rtmp_cam_full_testclass_t> *	rtmp_cam_full_testclass;
	rtmp_cam_full_testclass	= nipmem_new nunit_testclass_t<rtmp_cam_full_testclass_t>("full", nipmem_new rtmp_cam_full_testclass_t());
	// add some test functions
	rtmp_cam_full_testclass->append("general"	, &rtmp_cam_full_testclass_t::general);
	// add the testclass to the nunit_suite
	rtmp_cam_suite->append(rtmp_cam_full_testclass);

	// add the rtmp_cam_suite to the nunit_suite
	nunit_suite.append(rtmp_cam_suite);
}

// definition of the nunit_gsuite_t for flv
NUNIT_GSUITE_DEFINITION(rtmp_cam_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t
NUNIT_GSUITE_INSERTER(rtmp_cam_nunit_gsuite, 0, rtmp_cam_gsuite_fct);

NEOIP_NAMESPACE_END




