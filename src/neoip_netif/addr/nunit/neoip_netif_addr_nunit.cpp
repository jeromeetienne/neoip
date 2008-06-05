/*! \file
    \brief unit test for the netif_addr_t unit testt

*/

/* system include */
/* local include */
#include "neoip_netif_addr_nunit.hpp"
#include "neoip_netif_addr.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 * 
 * - check if lo is present and have "127.0.0.1/8"
 */
nunit_res_t	netif_addr_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// get all the netif address
	netif_addr_arr_t	all_addr = netif_addr_t::get_all_netif();
	// log to debug
	KLOG_DBG("all_addr.size()=" << all_addr.size() );
	// check the localhost is found and have 127.0.0.1/8 as netaddr
	for(size_t i = 0; i < all_addr.size(); i++){
		// log to debug
		KLOG_DBG("netif_addr " << i << "th =" << all_addr[i]);	
		// if this netif_addr_t is not the localhost, skip it
		if( all_addr[i].get_netif_name() != "lo" )	continue;
		if( all_addr[i].get_ip_netaddr() == ip_netaddr_t("127.0.0.1/8") )
			return NUNIT_RES_OK;
		break;
	}
	// return error
	return NUNIT_RES_ERROR;
}


NEOIP_NAMESPACE_END
