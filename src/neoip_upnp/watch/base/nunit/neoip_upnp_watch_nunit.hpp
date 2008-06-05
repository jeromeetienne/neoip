/*! \file
    \brief Header of the unit test of the upnp_watch_t layer
*/


#ifndef __NEOIP_UPNP_WATCH_NUNIT_HPP__ 
#define __NEOIP_UPNP_WATCH_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_upnp_watch_cb.hpp"
#include "neoip_upnp_bindport_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the upnp_watch_listener_t
 */
class upnp_watch_testclass_t : public nunit_testclass_api_t, private upnp_watch_cb_t
						, private upnp_bindport_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	/*************** upnp_watch_t	***************************************/
	upnp_watch_t *	upnp_watch;
	bool 		neoip_upnp_watch_cb(void *cb_userptr, upnp_watch_t &cb_upnp_watch
					, const upnp_watch_event_t &watch_event)	throw();

	/*************** upnp_bindport_t	*******************************/
	upnp_bindport_t*upnp_bindport;
	bool		neoip_upnp_bindport_cb(void *cb_userptr, upnp_bindport_t &cb_upnp_bindport
				, const upnp_err_t &upnp_err, const ipport_addr_t &ipport_pview)	throw();
public:
	/*************** ctor/dtor	***************************************/
	upnp_watch_testclass_t()	throw();
	~upnp_watch_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_WATCH_NUNIT_HPP__  */



