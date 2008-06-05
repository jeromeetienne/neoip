/*! \file
    \brief Definition of the unit test for the \ref nunit_t stuff

*/

/* system include */
#include <iostream>
#include <iomanip>
/* local include */
#include "neoip_nunit_utest.hpp"
#include "neoip_nunit.hpp"
#include "neoip_nunit_gsuite.hpp"
#include "neoip_nunit_runner.hpp"
#include "neoip_nunit_path.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

using namespace neoip;


NEOIP_NAMESPACE_BEGIN;

/** \brief Example of a nunit_testclass_api_t
 */
class nunit_testclass_utest_t : public nunit_testclass_api_t, private timeout_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify asynchronously 

	timeout_t	timeout;	//!< the timeout to test the async notification of the nunit_res_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
	{
		// log to debug
		KLOG_DBG("enter");
		// report the result
		nunit_ftor(NUNIT_RES_OK);
		// return 'dontkeep'
		return false;
	}
public:
	~nunit_testclass_utest_t()	throw()	{}
	/** \brief DeInit the testclass implementation
	 */
	void	neoip_nunit_class_deinit()	throw()	
	{
		// log to debug
		KLOG_DBG("enter");
		// stop the timeout if it is running
		if( timeout.is_running() )	timeout.stop();
	}
	

	/** \brief Build a fake test which always succeed
	 */
	nunit_res_t	synctest_succeed(const nunit_testclass_ftor_t &testclass_ftor)	throw()
	{
		// log to debug
		KLOG_ERR("enter " << NUNIT_RES_OK);
		// return a NUNIT_RES_OK
		return NUNIT_RES_OK;
	}

	/** \brief Build a fake test which always fails
	 */
	nunit_res_t	synctest_failed(const nunit_testclass_ftor_t &testclass_ftor)		throw()
	{
		// log to debug
		KLOG_DBG("enter");
		// return a NUNIT_RES_ERROR
		return NUNIT_RES_ERROR;
	}

	/** \brief Build a fake test which always report asynchronously
	 * 
	 * - it starts a timeout of 1sec, return a delayed result and notify
	 *   the actual result once the timeout expires
	 */
	nunit_res_t	asynctest(const nunit_testclass_ftor_t &testclass_ftor)		throw()
	{
		// log to debug
		KLOG_ERR("enter");
		// Start the timeout
		timeout.start(delay_t::from_sec(1), this, NULL);
		// copy the functor for later report
		nunit_ftor	= testclass_ftor;
		// return a delayed nunit_res_t
		return NUNIT_RES_DELAYED;
	}
};


/** \brief Implement a nunit_gsuite_fct_t 
 */
static void nunit_utest_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// log to debug
	KLOG_DBG("enter");
	// init the testclass
	nunit_testclass_t<nunit_testclass_utest_t> *	testclass;
	testclass	= nipmem_new nunit_testclass_t<nunit_testclass_utest_t>("nunit_utest_testclass"
							, nipmem_new nunit_testclass_utest_t());
	// add some test functions
	testclass->append("sync_succeed", &nunit_testclass_utest_t::synctest_succeed);
	testclass->append("sync_failed"	, &nunit_testclass_utest_t::synctest_failed);
	testclass->append("async_test"	, &nunit_testclass_utest_t::asynctest);
	
	// add the testclass to the nunit_suite
	nunit_suite.append(testclass);
}

// example of nunit_gsuite_t declaration - should be in a .hpp
NUNIT_GSUITE_DECLARATION(nunit_gsuite_slota);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(nunit_gsuite_slota, 0, nunit_utest_gsuite_fct);

// example of nunit_gsuite_t definition - should be in a .cpp
NUNIT_GSUITE_DEFINITION(nunit_gsuite_slota);


/** \brief The unit test class to run a nunit_suite_
 */
class nunit_utest_t : private nunit_runner_cb_t {
private:
	nunit_runner_t	nunit_runner;
	nunit_suite_t *	nunit_suite;
	
	/** \brief called when a nunit_runner_t is completed
	 */
	bool neoip_nunit_runner_completed_cb(void *userptr, nunit_runner_t &nunit_runner)	throw()
	{
		KLOG_ERR("nunit_runner completed");
		return true;
	}
	
public:
	/** \brief Constructor
	 */
	nunit_utest_t()		throw()
	{
		// log to debug
		KLOG_DBG("enter");
		
		// init the nunit_suite
		nunit_suite	= nipmem_new nunit_suite_t("nunit_all");
		// populate the nunit_suite from the nunit_gsuite_t
		nunit_gsuite_slota->populate(*nunit_suite);
		
		/**************** test the testname_db	***********************/
		std::list<nunit_path_t>	testname_db;
		nunit_suite->nunit_tester_get_allname("", "*", testname_db);
		// display the list of all testname
		KLOG_ERR("Display all the testname of this nunit_suite_t:");
		std::list<nunit_path_t>::iterator	iter;
		for( iter = testname_db.begin(); iter != testname_db.end(); iter++ ){
			KLOG_ERR("testname=" << *iter);
		}

		// Start a nunit_runner_t on the just built nunit_suite
//		nunit_path_t	path_pattern	= "*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*";
		nunit_path_t	path_pattern	= "nunit_all/nunit_utest_testclass/*";
		nunit_runner.start(path_pattern, nunit_suite, this, NULL);
	}
	
	/** \brief Destructor
	 */
	~nunit_utest_t()	throw()
	{
		// log to debug
		KLOG_DBG("enter");
		// deinit the nunit_suite
		nipmem_delete	nunit_suite;
	}
};


NEOIP_NAMESPACE_END

/** \brief unit test for the neoip_utest class
 */
int neoip_nunit_utest()
{
	int		n_error		= 0;

#if 0
	// play a bit with the nunit_path_t just to see if it works
	nunit_path_t	nunit_path;
	nunit_path	/= "nunit_all";
	nunit_path	/= "nunit_utest_testclass";
	KLOG_ERR("nunit_path=" << nunit_path);
#endif
	
	if( n_error )	goto error;
	KLOG_ERR("nunit_t PASSED");
	return 0;
	
error:;	KLOG_ERR("nunit_t FAILED!!!!");
	return -1;
}

static nunit_utest_t *	nunit_utest	= NULL;

/** \brief unit test for the neoip_utest class
 */
int neoip_nunit_utest_start()
{
	// create the nunit_utest
	nunit_utest	= nipmem_new nunit_utest_t();
	// return no error
	return 0;
}

int neoip_nunit_utest_end()
{
	// Destroy the nunit_utest
	nipmem_delete	nunit_utest;
	// return no error
	return 0;
}


