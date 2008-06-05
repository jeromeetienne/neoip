/*! \file
    \brief unit test for the rate_estim_pool_t
*/

/* system include */
#include <math.h>
/* local include */
#include "neoip_rate_estim_nunit.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rate_estim_testclass_t::rate_estim_testclass_t()	throw()
{
}

/** \brief Destructor
 */
rate_estim_testclass_t::~rate_estim_testclass_t()	throw()
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
nunit_err_t	rate_estim_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	rate_estim_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the serial consistency
 */
nunit_res_t	rate_estim_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	delaygen_arg_t	delaygen_arg;
	// log to debug
	KLOG_DBG("enter");

	// init some parameter
	xmit_rate_estim	= rate_estim_t<size_t>(delay_t::from_sec(20), delay_t::from_sec(2));
	xmit_nb_elem	= 500;
	delaygen_arg	= delaygen_regular_arg_t().period(delay_t::from_msec(100))
//								.random_range(0.2)
								.timeout_delay(delay_t::from_sec(5))
								.first_delay(delay_t::from_msec(50))
								;
	// init delaygen for the packet rxmit
	xmit_delaygen	= delaygen_t(delaygen_arg);
	// start the timer immediatly
	xmit_timeout.start(xmit_delaygen.current(), this, NULL);

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return no error
	return NUNIT_RES_DELAYED;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool rate_estim_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// update the rate estimator
	xmit_rate_estim.update(xmit_nb_elem);
	// log to debug
	KLOG_ERR("average=" << xmit_rate_estim.average());

	// notify the expiration of the timeout to the delaygen
	xmit_delaygen.notify_expiration();
	// if the delaygen is is_timedout, notify the faillure
	if( xmit_delaygen.is_timedout() ){
		xmit_timeout.stop();
		if( fabs(xmit_rate_estim.average() - 5000.0) <= 501 )	return nunit_ftor(NUNIT_RES_OK);
		else							return nunit_ftor(NUNIT_RES_ERROR);
	}

	// set the next timer
	xmit_timeout.change_period(xmit_delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}

NEOIP_NAMESPACE_END
