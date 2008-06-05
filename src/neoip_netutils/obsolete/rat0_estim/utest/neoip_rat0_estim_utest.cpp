/*! \file
    \brief Definition of the unit test for the \ref neoip_crypto_rat0_estim

\par Brief description
This module performs an unit test for the neoip_crypto_rat0_estim classes

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_rat0_estim_utest.hpp"
#include "neoip_rat0_estim.hpp"
#include "neoip_timeout.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

using namespace neoip;

class rat0_estim_utest_t : private timeout_cb_t {
private:
	rat0_estim_t<uint64_t>	rat0_estim;
	timeout_t	timeout;	//!< to periodically send packet over the rat0_estim_full

	bool	neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw(){
		rat0_estim.count(2*1024);
		KLOG_ERR("average=" << rat0_estim.get_avg() );
		return true;
	}
public:
	rat0_estim_utest_t()	throw(){
		rat0_estim	= rat0_estim_t<uint64_t>(delay_t::from_sec(10), delay_t::from_msec(500));
		// start the timeout
		timeout.start(delay_t::from_msec(200), this, NULL);
	}
};

/** \brief unit test for the neoip_rat0_estim class
 */
int neoip_rat0_estim_utest()
{
	int		n_error = 0;
	rat0_estim_t<uint64_t>	rat0_estim(delay_t::from_sec(5), delay_t::from_sec(1));
	
	rat0_estim.count(1);
//	rat0_estim.count(1);
//	rat0_estim.count(1);
//	rat0_estim.count(1);

	KLOG_ERR("average=" << rat0_estim.get_avg() );

	if( n_error )	goto error;
	KLOG_ERR("RAT0_ESTIM_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("RAT0_ESTIM_UTEST FAILED!!!!");
	return -1;
}


rat0_estim_utest_t *	rat0_estim_utest	= NULL;

/** \brief unit test for the neoip_rat0_estim class
 */
int neoip_rat0_estim_utest_start()	throw()
{
	KLOG_ERR("enter");
	rat0_estim_utest	= nipmem_new rat0_estim_utest_t();
	return 0;
}

int neoip_rat0_estim_utest_end()	throw()
{
	KLOG_ERR("enter");
	nipmem_delete	rat0_estim_utest;
	return 0;
}

