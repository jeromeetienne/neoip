/*! \file
    \brief Header of the test of rate_estim_pool_t
*/


#ifndef __NEOIP_RATE_ESTIM_NUNIT_HPP__ 
#define __NEOIP_RATE_ESTIM_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_rate_estim.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the rate_estim_pool_t
 */
class rate_estim_testclass_t : public nunit_testclass_api_t, private timeout_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	rate_estim_t<size_t>	xmit_rate_estim;
	size_t			xmit_nb_elem;
	/*************** xmit timer	***************************************/
	delaygen_t		xmit_delaygen;	//!< the delay_t generator for the rxmit of packet
	timeout_t		xmit_timeout;	//!< to periodically send packet over the udp_client_t
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	rate_estim_testclass_t()	throw();
	~rate_estim_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_ESTIM_NUNIT_HPP__  */



