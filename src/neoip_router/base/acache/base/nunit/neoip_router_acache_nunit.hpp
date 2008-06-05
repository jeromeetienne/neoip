

#ifndef __NEOIP_ROUTER_ACACHE_NUNIT_HPP__ 
#define __NEOIP_ROUTER_ACACHE_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_timeout.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	router_acache_t;

/** \brief Class which implement a nunit for the router_acache_t
 */
class router_acache_testclass_t : public nunit_testclass_api_t, private timeout_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously

	router_acache_t *	router_acache;
	bytearray_t		acache_bytearray;
	
	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	router_acache_testclass_t()	throw();
	~router_acache_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	item_presence(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	item_expiration(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ACACHE_NUNIT_HPP__  */



