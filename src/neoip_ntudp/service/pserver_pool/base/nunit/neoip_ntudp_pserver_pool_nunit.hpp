

#ifndef __NEOIP_NTUDP_PSERVER_POOL_NUNIT_HPP__ 
#define __NEOIP_NTUDP_PSERVER_POOL_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_timeout.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_pserver_pool_t;

/** \brief Class which implement a nunit for the ntudp_pserver_pool_t
 */
class ntudp_pserver_pool_testclass_t : public nunit_testclass_api_t, private event_hook_cb_t
								, private timeout_cb_t {
private:
	ntudp_peer_t *		ntudp_peer1;	//!< the ntudp_peer_t which act as server for clients
	ntudp_peer_t *		ntudp_peer2;	//!< the ntudp_peer_t which act as server for clients
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	
	/*************** pserver_pool stuff	*******************************/
	ntudp_pserver_pool_t *	pserver_pool;	//!< the ntudp_pserver_pool_t which is tested
	bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level) throw();	

	/*************** timeout stuff	***************************************/
	timeout_t	timeout;	//!< the timeout before launch ntudp_peer2
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_pool_testclass_t()	throw();
	~ntudp_pserver_pool_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_POOL_NUNIT_HPP__  */



