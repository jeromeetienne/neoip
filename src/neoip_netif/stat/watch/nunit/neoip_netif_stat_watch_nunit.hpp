/*! \file
    \brief Header of the unit test of the netif_stat_watch_t layer
*/


#ifndef __NEOIP_NETIF_STAT_WATCH_NUNIT_HPP__ 
#define __NEOIP_NETIF_STAT_WATCH_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_netif_stat_watch_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the gen_id_t
 */
class netif_stat_watch_testclass_t : public nunit_testclass_api_t, private netif_stat_watch_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	/*************** netif_stat_watch_t	*******************************/
	netif_stat_watch_t *	netif_stat_watch;//!< the netif_stat_watch_t which is tested	
	bool neoip_netif_stat_watch_cb(void *cb_userptr, netif_stat_watch_t &cb_netif_stat_watch
						, const netif_stat_arr_t &netif_stat_arr)	throw();	
public:
	/*************** ctor/dtor	***************************************/
	netif_stat_watch_testclass_t()	throw();
	~netif_stat_watch_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_STAT_WATCH_NUNIT_HPP__  */



