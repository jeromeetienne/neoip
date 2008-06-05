/*! \file
    \brief Definition of the unit test for the \ref neoip_crypto_rat0_limit

\par Brief description
This module performs an unit test for the neoip_crypto_rat0_limit classes

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_rat0_limit_utest.hpp"
#include "neoip_rat0_limit.hpp"
#include "neoip_timeout.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

using namespace neoip;

class rat0_limit_utest_t : private timeout_cb_t {
private:
	rat0_limit_t<uint64_t>	rat0_limit;
	timeout_t	timeout;	//!< to periodically send packet over the rat0_limit_full

	bool	neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw(){
#if 1
		if( rat0_limit.would_exceed(2*1024, 9*1024) ){
			KLOG_ERR("avg=" << rat0_limit.get_avg() << " EEEEEEEEEEEEEEEEXCEEDED");
			return true;
		}
#endif
		rat0_limit.count(2*1024);
		KLOG_ERR("****************** counted avg=" << rat0_limit.get_avg() );
		return true;
	}
public:
	rat0_limit_utest_t()	throw(){
		rat0_limit	= rat0_limit_t<uint64_t>(delay_t::from_sec(5), delay_t::from_msec(500));
		// start the timeout
		timeout.start(delay_t::from_msec(100), this, NULL);
	}
};

/** \brief unit test for the neoip_rat0_limit class
 */
int neoip_rat0_limit_utest()
{
	int		n_error = 0;
	rat0_limit_t<uint64_t>	rat0_limit(delay_t::from_sec(5), delay_t::from_sec(1));
	
	rat0_limit.count(1);
//	rat0_limit.count(1);
//	rat0_limit.count(1);
//	rat0_limit.count(1);

	KLOG_ERR("average=" << rat0_limit.get_avg() );

	if( n_error )	goto error;
	KLOG_ERR("RAT0_LIMIT_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("RAT0_LIMIT_UTEST FAILED!!!!");
	return -1;
}


rat0_limit_utest_t *	rat0_limit_utest	= NULL;

/** \brief unit test for the neoip_rat0_limit class
 */
int neoip_rat0_limit_utest_start()	throw()
{
	KLOG_ERR("enter");
	rat0_limit_utest	= nipmem_new rat0_limit_utest_t();
	return 0;
}

int neoip_rat0_limit_utest_end()	throw()
{
	KLOG_ERR("enter");
	nipmem_delete	rat0_limit_utest;
	return 0;
}

