/*! \file
    \brief Header of the unit test of the nslan_t layer
*/


#ifndef __NEOIP_NSLAN_NUNIT_HPP__ 
#define __NEOIP_NSLAN_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_nslan_query_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	nslan_listener_t;
class	nslan_peer_t;
class	nslan_publish_t;

/** \brief Class which implement a nunit for the nslan_listener_t
 */
class nslan_testclass_t : public nunit_testclass_api_t, private nslan_query_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	/*************** nslan	***************************************/
	nslan_listener_t *	nslan_listener;
	nslan_peer_t *		nslan_peer;
	nslan_publish_t *	nslan_publish;
	nslan_query_t *		nslan_query;
	bool neoip_nslan_query_cb(void *cb_userptr, nslan_query_t &cb_nslan_query
					, const nslan_event_t &nslan_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	nslan_testclass_t()	throw();
	~nslan_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	basic_query(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NSLAN_NUNIT_HPP__  */



