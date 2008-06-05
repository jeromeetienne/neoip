/*! \file
    \brief unit test for the netif_addr_watch_t unit test

\par Description
-# init the netif_addr_watch_t
-# init a netif_vdev_t and set it up
-# test succeed when the ip addr of the netif_vdev_t is notified

*/

/* system include */
/* local include */
#include "neoip_netif_addr_watch_nunit.hpp"
#include "neoip_netif_addr_watch.hpp"
#include "neoip_netif_vdev.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
netif_addr_watch_testclass_t::netif_addr_watch_testclass_t()	throw()
{
	// zero some field
	netif_addr_watch= NULL;
	netif_vdev	= NULL;
}

/** \brief Destructor
 */
netif_addr_watch_testclass_t::~netif_addr_watch_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	netif_addr_watch_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check
	DBG_ASSERT( netif_addr_watch == NULL );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	netif_addr_watch_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete the netif_vdev
	nipmem_zdelete	netif_vdev;
	// delete the netif_addr_watch
	nipmem_zdelete	netif_addr_watch;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	netif_addr_watch_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	netif_err_t	netif_err;
	// sanity check - the netif_addr_watch MUST be NULL
	DBG_ASSERT( netif_addr_watch == NULL );
	
	// start the netif_addr_watch_t
	netif_addr_watch= nipmem_new netif_addr_watch_t();
	netif_err	= netif_addr_watch->start(this, NULL);
	NUNIT_ASSERT( netif_err.succeed() );
	

	// start the netif_vdev_t
	netif_vdev	= nipmem_new netif_vdev_t();
	netif_err	= netif_vdev->start(this, NULL, netif_vdev_t::TYPE_TUN);
	NUNIT_ASSERT( netif_err.succeed() );
	// setup the ip_netaddr
	netif_err	= netif_vdev->set_netaddr("1.2.3.4/16");
	NUNIT_ASSERT( netif_err.succeed() );
	// set the netif UP
	netif_err	= netif_vdev->set_updown(netif_util_t::IF_UP);
	DBG_ASSERT( netif_err.succeed() );
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     netif_addr_watch callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when netif_addr_watch_t has an event to report
 */
bool netif_addr_watch_testclass_t::neoip_netif_addr_watch_cb(void *cb_userptr
						, netif_addr_watch_t &cb_netif_addr_watch
						, const netif_addr_arr_t &netif_addr_arr)	throw()
{
	// log to debug 
	KLOG_DBG("enter");
	// scan the whole notified netif_addr_arr_t
	for(size_t i = 0; i < netif_addr_arr.size(); i++){
		const netif_addr_t &	netif_addr	= netif_addr_arr[i];
		// if the just netif_vdev_t has been found, the test succeed
		if( netif_addr.get_netif_name() == netif_vdev->get_name()
					&& netif_addr.get_ip_netaddr() == ip_netaddr_t("1.2.3.4/16") ){
			// notify caller
			bool	tokeep	= nunit_ftor(NUNIT_RES_OK);
			if( !tokeep )	return false;
			// delete the netif_addr_watch_t and mark is unused
			nipmem_delete	netif_addr_watch;
			netif_addr_watch= NULL;
			// return 'dontkeep'
			return false;
		}
	}
	// return 'tokeep'
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     netif_vdev callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when netif_vdev_t has an event to report
 */
bool netif_addr_watch_testclass_t::neoip_netif_vdev_cb(void *cb_userptr, netif_vdev_t &cb_netif_vdev
							, uint16_t ethertype, pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// NOTE: this is just a NOP function
	
	// return 'tokeep'
	return true;
}

NEOIP_NAMESPACE_END
