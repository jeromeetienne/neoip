

#ifndef __NEOIP_ROUTER_PEER_NUNIT_HPP__ 
#define __NEOIP_ROUTER_PEER_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_timeout.hpp"
#include "neoip_host2ip_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	router_peer_t;

/** \brief Class which implement a nunit for the router_peer_t
 */
class router_peer_testclass_t : public nunit_testclass_api_t, private timeout_cb_t, private host2ip_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously

	router_peer_t *		router1;
	router_peer_t *		router2;

	
	/*************** lame tech to wait for bootstrap completion	*******/
	timeout_t		bstrap_timeout;	//!< the timeout to wait for the end of the bootstrap
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	

	/*************** host2ip_t	***************************************/
	host2ip_t *		host2ip;	//!< the host2ip_t which is tested	
	bool			neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
						, const inet_err_t &inet_err
						, const std::vector<ip_addr_t> &ipaddr_arr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	router_peer_testclass_t()	throw();
	~router_peer_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	wait_for_bstrap(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_PEER_NUNIT_HPP__  */



