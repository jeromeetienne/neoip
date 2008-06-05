/*! \file
    \brief Definition of the unit test for the router_peer_t
    
*/

/* system include */
/* local include */
#include "neoip_router_peer_nunit.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_host2ip.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_netif_addr.hpp"
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
router_peer_testclass_t::router_peer_testclass_t()	throw()
{
	// zero some field
	router1		= NULL;
	router2		= NULL;
	host2ip		= NULL;
}

/** \brief Destructor
 */
router_peer_testclass_t::~router_peer_testclass_t()	throw()
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
nunit_err_t	router_peer_testclass_t::neoip_nunit_testclass_init()	throw()
{
	router_err_t		router_err;
	ip_netaddr_arr_t	ip_netaddr_arr;
	router_profile_t	profile1;
	router_lident_t		lident1;
	router_rident_arr_t	rident1_arr;
	router_acache_t		acache1;
	router_profile_t	profile2;
	router_lident_t		lident2;
	router_rident_arr_t	rident2_arr;
	router_acl_t		router_acl;

	// setup the ip_netaddr_t handled by the router_peer_t
	ip_netaddr_arr	= ip_netaddr_arr_t();
	ip_netaddr_arr	+= "5.0.0.0/8";
	
	// build a router_acl_t which allow everybody
	router_acl.append(router_acl_type_t::ACCEPT, "*");
	
	// get the local identity for router
	lident1		= router_lident_t::generate(router_peerid_t::build_random()
					, router_name_t("router1.*"), 512, x509_cert_t(), x509_privkey_t());
	lident2		= router_lident_t::generate(router_peerid_t::build_random()
					, router_name_t("router2.*"), 512, x509_cert_t(), x509_privkey_t());
	
	// log to debug
	KLOG_DBG("lident1 peerid=" << lident1.peerid());
	KLOG_DBG("lident2 peerid=" << lident2.peerid());

	// the dnsgrab_t priority of router1 MUST be less than the router2 one, to ensure this is 
	// router1 which will intercept the agethostbyname_t before the router2
	profile1.dnsgrab_arg().reg_priority(998);
	profile2.dnsgrab_arg().reg_priority(999);
	
	// set the router_rident_arr_t - each rident contain the other lident version
	rident1_arr	+= lident2.to_rident();
	rident2_arr	+= lident1.to_rident();
	
	// set the outter ipport_listen1
	ipport_aview_t	ipport_listen1	= ipport_aview_t().lview("127.0.0.1:4000").pview("127.0.0.1:4000");
	// init and start router_peer_t
	router1		= nipmem_new router_peer_t();
	router_err	= router1->set_profile(profile1).start(ipport_listen1, lident1, rident1_arr
						, router_rootca_arr_t(), router_acl, ip_netaddr_arr);
	if( router_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, router_err.to_string());

	// set the acache for router1
	acache1.update(lident2.dnsname() / profile1.selfsigned_domain(), "5.0.0.21", "5.0.0.22"
							, delay_t::from_min(30));
	router1->set_acache(acache1);

	// set the outter ipport_listen2
	ipport_aview_t	ipport_listen2	= ipport_aview_t().lview("127.0.0.1:4001").pview("127.0.0.1:4001");
	// init and start router_peer_t
	router2		= nipmem_new router_peer_t();
	router_err	= router2->set_profile(profile2).start(ipport_listen2, lident2, rident2_arr
						, router_rootca_arr_t(), router_acl, ip_netaddr_arr);
	if( router_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, router_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	router_peer_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// stop the bstrap_timeout if needed
	if( bstrap_timeout.is_running() )	bstrap_timeout.stop();
	
	// delete the host2ip
	nipmem_zdelete	host2ip;
	// delete router_peer_t if needed
	nipmem_zdelete	router1;
	// delete router_peer_t if needed
	nipmem_zdelete	router2;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to wait for bootstrap
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a fake test which always report asynchronously
 * 
 * - it starts a router of 1sec, return a delayed result and notify
 *   the actual result once the router expires
 */
nunit_res_t	router_peer_testclass_t::wait_for_bstrap(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");
	// Start the timeout
	bstrap_timeout.start(delay_t::from_sec(1), this, NULL);
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback called when the timeout_t expire
 */
bool 	router_peer_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// stop the timeout
	bstrap_timeout.stop();	
	// report the result
	return nunit_ftor(NUNIT_RES_OK);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     testclass function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the router_peer_t
 */
nunit_res_t	router_peer_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	inet_err_t	inet_err;
	// start the agethostbyname_t
	host2ip		= nipmem_new host2ip_t();
	inet_err	= host2ip->start("router2.neoip", this, NULL);
	NUNIT_ASSERT( inet_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 host2ip_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by host2ip_t when the result is known
 */
bool	router_peer_testclass_t::neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
			, const inet_err_t &inet_err, const std::vector<ip_addr_t> &ipaddr_arr)	throw()
{
	// display the result
	KLOG_ERR("host2ip returned err=" << inet_err << " with " << ipaddr_arr.size() << " ip_addr_t"
					<< " for hostname=" << host2ip->hostname());

	// delete host2ip_t
	nipmem_zdelete host2ip;

	// if the host2ip_t doesnt return an ip_addr_t, this nunit failed
	if( inet_err.failed() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// check that the returned ip_addr_t is "5.0.0.22"
	// - it MUST be as it is present in the router_acache_t
	ip_addr_t	ip_addr = ipaddr_arr[0];
	if( ip_addr == "5.0.0.22" )	nunit_ftor(NUNIT_RES_OK);
	else				nunit_ftor(NUNIT_RES_ERROR);
	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END

