

#ifndef __NEOIP_DNSGRAB_NUNIT_HPP__ 
#define __NEOIP_DNSGRAB_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_dnsgrab_cb.hpp"
#include "neoip_host2ip_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;

/** \brief Class which implement a nunit for the dnsgrab_t
 */
class dnsgrab_testclass_t : public nunit_testclass_api_t, private dnsgrab_cb_t, private host2ip_cb_t  {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously
	
	/*************** dnsgrab_t stuff	*******************************/
	dnsgrab_t *	dnsgrab;
	bool		neoip_dnsgrab_cb(void *cb_userptr, dnsgrab_t &cb_dnsgrab, dnsgrab_request_t &request)	throw();

	/*************** host2ip_t	***************************************/
	host2ip_t *	host2ip;	//!< the host2ip_t which is tested
	bool		neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
						, const inet_err_t &inet_err
						, const std::vector<ip_addr_t> &result_arr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	dnsgrab_testclass_t()	throw();
	~dnsgrab_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	test_thru_host2ip(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DNSGRAB_NUNIT_HPP__  */



