/*! \file
    \brief Definition of the nunit_gsuite_t for the netif_t layer

*/

/* system include */
/* local include */
#include "neoip_netif_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of include for all the testclass
#include "neoip_netif_util_nunit.hpp"
#include "neoip_netif_vdev_nunit.hpp"
#include "neoip_netif_addr_nunit.hpp"
#include "neoip_netif_addr_watch_nunit.hpp"
#include "neoip_netif_stat_nunit.hpp"
#include "neoip_netif_stat_watch_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void netif_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	netif_suite	= nipmem_new nunit_suite_t("netif");		
	// log to debug
	KLOG_DBG("enter");
	
/********************* netif_util_t	***************************************/
	// init the testclass for the netif_util_t
	nunit_testclass_t<netif_util_testclass_t> *	netif_util_testclass;
	netif_util_testclass	= nipmem_new nunit_testclass_t<netif_util_testclass_t>("netif_util", nipmem_new netif_util_testclass_t());
	// add some test functions
	netif_util_testclass->append("general"		, &netif_util_testclass_t::general);
	// add the testclass to the nunit_suite
	netif_suite->append(netif_util_testclass);

/********************* netif_vdev_t	***************************************/
	// init the testclass for the netif_vdev_t
	nunit_testclass_t<netif_vdev_testclass_t> *	netif_vdev_testclass;
	netif_vdev_testclass	= nipmem_new nunit_testclass_t<netif_vdev_testclass_t>("netif_vdev", nipmem_new netif_vdev_testclass_t());
	// add some test functions
	netif_vdev_testclass->append("general"		, &netif_vdev_testclass_t::general);
	// add the testclass to the nunit_suite
	netif_suite->append(netif_vdev_testclass);

/********************* netif_addr_t	***************************************/
	// init the testclass for the netif_addr_t
	nunit_testclass_t<netif_addr_testclass_t> *	netif_addr_testclass;
	netif_addr_testclass	= nipmem_new nunit_testclass_t<netif_addr_testclass_t>("netif_addr", nipmem_new netif_addr_testclass_t());
	// add some test functions
	netif_addr_testclass->append("general"		, &netif_addr_testclass_t::general);
	// add the testclass to the nunit_suite
	netif_suite->append(netif_addr_testclass);

/********************* netif_addr_watch_t	***************************************/
	// init the testclass for the netif_addr_watch_t
	nunit_testclass_t<netif_addr_watch_testclass_t> *	netif_addr_watch_testclass;
	netif_addr_watch_testclass	= nipmem_new nunit_testclass_t<netif_addr_watch_testclass_t>("netif_addr_watch", nipmem_new netif_addr_watch_testclass_t());
	// add some test functions
	netif_addr_watch_testclass->append("general"		, &netif_addr_watch_testclass_t::general);
	// add the testclass to the nunit_suite
	netif_suite->append(netif_addr_watch_testclass);

/********************* netif_stat_t	***************************************/
	// init the testclass for the netif_stat_t
	nunit_testclass_t<netif_stat_testclass_t> *	netif_stat_testclass;
	netif_stat_testclass	= nipmem_new nunit_testclass_t<netif_stat_testclass_t>("netif_stat", nipmem_new netif_stat_testclass_t());
	// add some test functions
	netif_stat_testclass->append("general"		, &netif_stat_testclass_t::general);
	// add the testclass to the nunit_suite
	netif_suite->append(netif_stat_testclass);

/********************* netif_stat_watch_t	***************************************/
	// init the testclass for the netif_stat_watch_t
	nunit_testclass_t<netif_stat_watch_testclass_t> *	netif_stat_watch_testclass;
	netif_stat_watch_testclass	= nipmem_new nunit_testclass_t<netif_stat_watch_testclass_t>("netif_stat_watch"
							, nipmem_new netif_stat_watch_testclass_t());
	// add some test functions
	netif_stat_watch_testclass->append("general"		, &netif_stat_watch_testclass_t::general);
	// add the testclass to the nunit_suite
	netif_suite->append(netif_stat_watch_testclass);

	// add the netif_suite to the nunit_suite
	nunit_suite.append(netif_suite);	
}

// definition of the nunit_gsuite_t for netif
NUNIT_GSUITE_DEFINITION(netif_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(netif_nunit_gsuite, 0, netif_gsuite_fct);

NEOIP_NAMESPACE_END




