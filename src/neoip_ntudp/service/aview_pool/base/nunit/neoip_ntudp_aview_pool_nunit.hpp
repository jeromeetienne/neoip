

#ifndef __NEOIP_NTUDP_AVIEW_POOL_NUNIT_HPP__ 
#define __NEOIP_NTUDP_AVIEW_POOL_NUNIT_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ntudp_npos_saddrecho_cb.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_aview_pool_t;

/** \brief Class which implement a nunit for the ntudp_aview_pool_t
 */
class ntudp_aview_pool_testclass_t : public nunit_testclass_api_t, private event_hook_cb_t
						, private ntudp_npos_saddrecho_cb_t {
private:
	ntudp_peer_t *		ntudp_peer1;	//!< the ntudp_peer_t which act as server for clients
	ntudp_peer_t *		ntudp_peer2;	//!< the ntusp_peer_t which act as server for clients
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	ntudp_aview_pool_t *	ntudp_aview_pool;

	/*************** callback for npos_saddrecho	***********************/
	std::list<ntudp_npos_saddrecho_t *>	saddrecho_db;	//!< all the saddrecho from the pool
	bool 		neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
						, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();	

	/*************** callback for ntudp_aview_pool_t	***************/
	bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level) throw();

public:
	/*************** ctor/dtor	***************************************/
	ntudp_aview_pool_testclass_t()		throw();
	~ntudp_aview_pool_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	get_5_aview(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_AVIEW_POOL_NUNIT_HPP__  */



