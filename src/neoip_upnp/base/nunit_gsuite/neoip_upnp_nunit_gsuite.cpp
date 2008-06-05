/*! \file
    \brief Definition of the nunit_gsuite_t for the upnp_t layer

*/

/* system include */
/* local include */
#include "neoip_upnp_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #incldue for each nunit
#include "neoip_upnp_watch_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void upnp_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	upnp_suite	= nipmem_new nunit_suite_t("upnp");		
	// log to debug
	KLOG_DBG("enter");

/********************* upnp_watch_t	***************************************/
	// init the testclass for the upnp_watch_t
	nunit_testclass_t<upnp_watch_testclass_t> *	upnp_watch_testclass;
	upnp_watch_testclass	= nipmem_new nunit_testclass_t<upnp_watch_testclass_t>("watch", nipmem_new upnp_watch_testclass_t());
	// add some test functions
	upnp_watch_testclass->append("general"		, &upnp_watch_testclass_t::general);
	// add the testclass to the nunit_suite
	upnp_suite->append(upnp_watch_testclass);

	// add the upnp_suite to the nunit_suite
	nunit_suite.append(upnp_suite);	
}

// definition of the nunit_gsuite_t for upnp
NUNIT_GSUITE_DEFINITION(upnp_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(upnp_nunit_gsuite, 0, upnp_gsuite_fct);

NEOIP_NAMESPACE_END




