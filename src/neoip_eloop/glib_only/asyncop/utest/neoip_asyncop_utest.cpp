/*! \file
    \brief Definition of the unit test for the \ref asyncop_client_t and asyncop_server_t

\par Brief description
This module performs an unit test for the asyncop classes

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_asyncop_utest.hpp"
#include "neoip_asyncop.hpp"
#include "neoip_timeout.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

using namespace neoip;


NEOIP_NAMESPACE_BEGIN;

/** \brief Callback notified to do the work
 * 
 * - IMPORTANT: this function is NOT running in the 'main' thread
 * 
 * @return a pointer on the result if one is provided, NULL otherwise
 */
static void *asyncop_utest_do_work_cb(void *userptr)	throw() 
{
	KLOG_ERR("enter - The secondary thread is " << g_thread_self() );
	for( size_t i = 0; i < 5; i++ ){
		// sleep for one second
		sleep(1);
		// log 
		KLOG_ERR("slept for " << i+1 << " second");
	}
	return strdup("Hello world! this is a asyncop result");
}

/** \brief Callback notified to free a potential the work
 * 
 * - IMPORTANT: this function IS running in the 'main' thread
 */
static void asyncop_utest_free_work_cb(void *work_result, void *userptr)	throw() 
{
	char *	str	= (char *)work_result;
	KLOG_ERR("enter with string result=" << str);
	free(str);
}

class asyncop_utest_t : private asyncop_completed_cb_t , private timeout_cb_t {
private:
	asyncop_t *	asyncop;
	
	/** \brief asyncop callback notified when the 
	 */
	void neoip_asyncop_completed_cb(void *cb_userptr, asyncop_t &asyncop, void *work_result)throw()
	{
		KLOG_ERR("The main thread is " << g_thread_self() );
		KLOG_ERR("enter work_result" << (char *)work_result);	
	}
	
	timeout_t	timeout;		//!< to rxmit the request
	/** \brief callback called when the neoip_timeout expire
	 */
	bool neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
	{
		// log to debug
		KLOG_ERR("enter - delete the asyncop - so no notification will happen after this point.");
		// delete the asyncop and mark it unused
		nipmem_delete	asyncop;
		asyncop	= NULL;
		// return 'dontkeep'
		return false;
	}
public:
	/** \brief Constructor
	 */
	asyncop_utest_t()		throw()
	{
		// create the asyncop_t
		asyncop	= nipmem_new asyncop_t();
		// start the asyncop_t
		bool	failed = asyncop->start(this, NULL, asyncop_utest_do_work_cb
						, asyncop_utest_free_work_cb);
		DBG_ASSERT( !failed );
		// start the timeout to test the asyncop_t deletion during the thread
		timeout.start(delay_t::from_sec(10), this, NULL);
		
		// log
		KLOG_ERR("The main thread is " << g_thread_self() );
		
	}
	/** \brief Destructor
	 */
	~asyncop_utest_t()		throw()
	{
		// delete the asyncop_t
		if( asyncop )	nipmem_delete asyncop;	
	}
};

NEOIP_NAMESPACE_END

/** \brief unit test for the neoip_asyncop class
 */
int neoip_asyncop_utest()
{
	int		n_error		= 0;

	if( n_error )	goto error;
	KLOG_ERR("asyncop_t utest PASSED");
	return 0;
	
error:;	KLOG_ERR("asyncop_t utest FAILED!!!!");
	return -1;
}


asyncop_utest_t *	asyncop_utest	= NULL;

/** \brief unit test for the asyncop_t class
 */
int neoip_asyncop_utest_start()
{
	KLOG_ERR("enter");
	asyncop_utest	= nipmem_new asyncop_utest_t();
	return 0;
}

int neoip_asyncop_utest_end()
{
	KLOG_ERR("enter");
	nipmem_delete	asyncop_utest;
	return 0;
}

