/*! \file
    \brief unit test for the netif_stat_t unit test

*/

/* system include */
/* local include */
#include "neoip_netif_stat_nunit.hpp"
#include "neoip_netif_stat.hpp"
#include "neoip_netif_stat_helper.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	netif_stat_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");
	
	netif_stat_t	netif_stat;
	netif_stat	= netif_stat_helper_t::one_netif_stat("eth0");
	KLOG_ERR("netif_stat=" << netif_stat);
	netif_stat	= netif_stat_helper_t::one_netif_stat("lo");
	KLOG_ERR("netif_stat=" << netif_stat);

	netif_stat_t	netif_stat1	= netif_stat_helper_t::one_netif_stat("eth0");
	netif_stat_t	netif_stat2	= netif_stat_helper_t::one_netif_stat("eth0");
	KLOG_ERR("delta=" << netif_stat2 - netif_stat1);

	KLOG_ERR("all netif_stat=" << netif_stat_helper_t::all_netif_stat() );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END
