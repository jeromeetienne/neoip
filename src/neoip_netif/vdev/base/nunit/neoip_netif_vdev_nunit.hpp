/*! \file
    \brief Header of the unit test of the netif_vdev_t layer
*/


#ifndef __NEOIP_NETIF_VDEV_NUNIT_HPP__ 
#define __NEOIP_NETIF_VDEV_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_netif_vdev_cb.hpp"
#include "neoip_ip_netaddr.hpp"
#include "neoip_datum.hpp"
#include "neoip_pkt.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the gen_id_t
 */
class netif_vdev_testclass_t : public nunit_testclass_api_t, private netif_vdev_cb_t
							, private timeout_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	/*************** netif_vdev	***************************************/
	netif_vdev_t *	netif_vdev;	//!< the netif_vdev_t which is tested	
	bool neoip_netif_vdev_cb(void *cb_userptr, netif_vdev_t &cb_netif_vdev, uint16_t ethertype
								, pkt_t &pkt)	throw();	

	ip_netaddr_t	netif_netaddr;
	datum_t		echo_payload;
	
	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;	//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;	//!< to periodically send packet over the udp_client_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	netif_vdev_testclass_t()	throw();
	~netif_vdev_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_VDEV_NUNIT_HPP__  */



