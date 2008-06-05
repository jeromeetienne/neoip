/*! \file
    \brief Definition of the unit test for the \ref neoip_timer_policy and co

\par Brief description
This module performs an unit test for the neoip_timer_policy classes

*/

/* system include */
/* local include */
#include "neoip_timer_policy_utest.hpp"
#include "neoip_timer_probing.hpp"
#include "neoip_log.hpp"
#include "neoip_datum.hpp"

using namespace neoip;

/** \brief unit test for the neoip_timer_policy class
 */
int neoip_timer_policy_utest()
{
	int		n_error = 0;
	
	timer_probing_t	probing = timer_probing_t(delay_t::from_sec(2), delay_t::ALWAYS, delay_t::from_sec(2));
	
	KLOG_ERR("first=" << probing.first());
	KLOG_ERR("next=" << probing.next() );
	KLOG_ERR("next=" << probing.next() );
	KLOG_ERR("next=" << probing.next() );
	KLOG_ERR("next=" << probing.next() );
	KLOG_ERR("next=" << probing.next() );
	KLOG_ERR("next=" << probing.next() );
	KLOG_ERR("next=" << probing.next() );
	KLOG_ERR("next=" << probing.next() );
	
	if( n_error )	goto error;
	KLOG_ERR("timer_policy_t unit test PASSED!!!");
	return 0;

error:;	KLOG_ERR("timer_policy_t unit test FAILED!!!");
	return n_error;
}

