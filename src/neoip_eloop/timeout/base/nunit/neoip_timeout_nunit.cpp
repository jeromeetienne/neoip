/*! \file
    \brief Definition of the unit test for the \ref timeout_t

*/

/* system include */
/* local include */
#include "neoip_timeout_nunit.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief callback called when the timeout_t expire
 */
bool 	timeout_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// timeout_t MUST be considered running during the callback notification
	if( !timeout.is_running() )	return nunit_ftor(NUNIT_RES_ERROR);
	
	// stop the timeout
	timeout.stop();

	// timeout_t MUST NOT be considered running after a timeout_t::stop()
	if( timeout.is_running() )	return nunit_ftor(NUNIT_RES_ERROR);

	// report the result
	return nunit_ftor(NUNIT_RES_OK);
}

/** \brief Build a fake test which always report asynchronously
 * 
 * - it starts a timeout of 1sec, return a delayed result and notify
 *   the actual result once the timeout expires
 */
nunit_res_t	timeout_testclass_t::expiration(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// Start the timeout
	timeout.start(delay_t::from_sec(1), this, NULL);

	// check if the timeout is running	
	NUNIT_ASSERT( timeout.is_running() );
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief Build a fake test which always report asynchronously
 * 
 * - it starts a timeout of 1sec, return a delayed result and notify
 *   the actual result once the timeout expires
 */
nunit_res_t	timeout_testclass_t::special_delay(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	timeout_t	timeout1;
	// log to debug
	KLOG_DBG("enter");
	// Start the timeout
	timeout1.start(delay_t::ALWAYS, this, NULL);
	// check if the timeout is running	
	NUNIT_ASSERT( timeout1.is_running() );
	// check if the timeout expire_delay is ALWAYS
	NUNIT_ASSERT( timeout1.get_expire_delay() == delay_t::ALWAYS );
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

