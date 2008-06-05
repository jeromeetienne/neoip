/*! \file
    \brief unit test for the netif_vdev_t unit testt

\par Description
-# this inits a virtual network interface
-# send a ICMP echo request to localhost
-# test succeed when the reply is received

*/

/* system include */
#include <netinet/ip_icmp.h>
/* local include */
#include "neoip_netif_vdev_nunit.hpp"
#include "neoip_netif_vdev.hpp"
#include "neoip_ippkt_util.hpp"
#include "neoip_pkt.hpp"
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
netif_vdev_testclass_t::netif_vdev_testclass_t()	throw()
{
	// zero some field
	netif_vdev	= NULL;
}

/** \brief Destructor
 */
netif_vdev_testclass_t::~netif_vdev_testclass_t()	throw()
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
nunit_err_t	netif_vdev_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check
	DBG_ASSERT( !netif_vdev );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	netif_vdev_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// stop the rxmit_timeout if needed
	if( rxmit_timeout.is_running() )	rxmit_timeout.stop();
	// delete the netif_vdev
	nipmem_zdelete	netif_vdev;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	netif_vdev_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	netif_err_t	netif_err;
	// start the netif_vdev_t
	netif_vdev	= nipmem_new netif_vdev_t();
	netif_err	= netif_vdev->start(this, NULL, netif_vdev_t::TYPE_TUN);
	NUNIT_ASSERT( netif_err.succeed() );

	// setup the 
	netif_netaddr	= "1.2.3.4/16";
	echo_payload	= datum_t("hello world from the neoip netif_vdev_t nunit");
	
	// setup the ip_netaddr
	netif_err	= netif_vdev->set_netaddr( netif_netaddr );
	NUNIT_ASSERT( netif_err.succeed() );
	
	// set the netif UP
	netif_err	= netif_vdev->set_updown(netif_util_t::IF_UP);
	NUNIT_ASSERT( netif_err.succeed() );

	// init delaygen for the itor phase
	delaygen_arg_t	delaygen_arg	= delaygen_expboff_arg_t().min_delay(delay_t::from_sec(1))
							.max_delay(delay_t::from_sec(3))
							.timeout_delay(delay_t::from_sec(10));
	rxmit_delaygen = delaygen_t(delaygen_arg);
	// start the timer immediatly
	rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool netif_vdev_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	netif_err_t	netif_err;
	// log to debug
	KLOG_DBG("enter");

	// notify the expiration of the timeout to the delaygen
	rxmit_delaygen.notify_expiration();
	// if the delaygen is is_timedout, notify an ERROR
	if( rxmit_delaygen.is_timedout() )	return nunit_ftor(NUNIT_RES_ERROR);

	// build the packet to send
	pkt_t	pkt	= ippkt_util_t::build_icmp4_pkt(netif_netaddr.get_base_addr()+1
						, netif_netaddr.get_base_addr()
						, ICMP_ECHO, 0, 0, echo_payload);
	// some sanity check acting as nunit for ippkt_util_t
	DBG_ASSERT( ippkt_util_t::get_src_addr(pkt)	== netif_netaddr.get_base_addr()+1 );
	DBG_ASSERT( ippkt_util_t::get_dst_addr(pkt)	== netif_netaddr.get_base_addr() );
	DBG_ASSERT( ippkt_util_t::get_ip_version(pkt)	== 4 );
	// send the packet
	netif_err	= netif_vdev->send_pkt(netif_vdev_t::TYPE_IP4, pkt);
	DBG_ASSERT( netif_err.succeed() );

	// set the next timer
	rxmit_timeout.change_period(rxmit_delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     netif_vdev callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when netif_vdev_t has an event to report
 */
bool netif_vdev_testclass_t::neoip_netif_vdev_cb(void *cb_userptr, netif_vdev_t &cb_netif_vdev
							, uint16_t ethertype, pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("recevied ethertype=0x" << std::hex << ethertype << std::dec << " pkt=" << pkt );
	// if the incoming packet is not large enought to contain the echo_payload, discard it
	if( pkt.get_len() < echo_payload.get_len() )	return true;
	// remove all but the data at the end
	pkt.head_consume( pkt.get_len() - echo_payload.get_len() );
	// check the packet
	if( pkt.to_datum() == echo_payload ){
		// TODO this is a bug in the nunit layer... this should be called automatically
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// return 'tokeep'
	return true;
}


NEOIP_NAMESPACE_END
