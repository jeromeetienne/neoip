/*! \file
    \brief unit test for the netif_util_t unit testt

*/

/* system include */
/* local include */
#include "neoip_netif_util_nunit.hpp"
#include "neoip_netif_util.hpp"
#include "neoip_netif_vdev.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	netif_util_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	netif_vdev_t	netif_vdev;
	netif_err_t	netif_err;
	std::string	netif_name;	
	// init the netif_vdev_t to play with
	netif_err	= netif_vdev.start(NULL, NULL, netif_vdev_t::TYPE_TAP);
	NUNIT_ASSERT( netif_err.succeed() );
	// get its netif_name
	netif_name	= netif_vdev.get_name();

	// test that the get_mtu return 1500 - this is the usual value for linux... 
	NUNIT_ASSERT( netif_util_t::get_mtu(netif_name) == 1500 );
	// set the mtu to 1000 and check if it worked
	netif_err	= netif_util_t::set_mtu(netif_name, 1000);
	NUNIT_ASSERT( netif_err.succeed() );	
	NUNIT_ASSERT( netif_util_t::get_mtu(netif_name) == 1000 );

	// set an ip_addr to the vdev
	netif_err	= netif_util_t::set_netif_ip_addr(netif_name, "1.2.3.4");
	NUNIT_ASSERT( netif_err.succeed() );

	// get the hwaddr
	datum_t		hw_addr = netif_util_t::get_hw_addr(netif_name);
	NUNIT_ASSERT( !hw_addr.is_null() );
	// modify the hwaddr - MUST be done before setting the netif up
	((char *)hw_addr.get_data())[0] = 0x42;

	// set the hwaddr back
	netif_err	= netif_util_t::set_hw_addr(netif_name, hw_addr);
	NUNIT_ASSERT( netif_err.succeed() );
	// check the get_hw_addr return the proper one
	NUNIT_ASSERT( hw_addr == netif_util_t::get_hw_addr(netif_name) );
		
	// set the netif UP
	netif_err	= netif_util_t::set_updown(netif_name, netif_util_t::IF_UP);
	DBG_ASSERT( netif_err.succeed() );

	// add some route
	netif_err	= netif_util_t::add_route(netif_name, "1.2.3.4/16");
	NUNIT_ASSERT( netif_err.succeed() );
	// delete the same route
	netif_err	= netif_util_t::del_route(netif_name, "1.2.3.4/16");
	NUNIT_ASSERT( netif_err.succeed() );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END
