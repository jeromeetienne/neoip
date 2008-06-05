/*! \file
    \brief Definition of the nunit_runner_t

*/

/* system include */
/* local include */
#include "neoip_nunit_runner.hpp"
#include "neoip_nunit_event.hpp"
#include "neoip_nunit_tester_api.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
nunit_runner_t::nunit_runner_t()	throw()
{
}

/** \brief Destructor
 */
nunit_runner_t::~nunit_runner_t()	throw()
{
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          run function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start to run the tester_api
 * 
 * @param testname_pattern	The pattern of test name to run
 * @param tester_api		The nunit_tester_api_t to run. the pointer is still owned by the caller
 *                      	and wont be freed by the nunit_runner_t
 * @param callback		the callback to notify on completion
 * @param userptr		the userptr associated with the callback
 */
void	nunit_runner_t::start(const nunit_path_t &path_pattern, nunit_tester_api_t *tester_api
				, nunit_runner_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->tester_api	= tester_api;
	this->callback		= callback;
	this->userptr		= userptr;
	this->path_pattern	= path_pattern;
	
	// zero some fields
	nb_test_completed	= 0;
	nb_test_succeed		= 0;

	// init zerotimer to start the testing and especialy avoid notifying in event from here
	zerotimer.append(this, NULL);	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  zerotimer_cb_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - this function launch the tester_api_t if it matches the nunit_path_t pattern
 *   - else notify a nunit_even_t::TEST_NOMORE
 */
bool	nunit_runner_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// if the path_pattern doesnt match the subtest_name, notify a TEST_NOMORE
	nunit_path_t	subtest_name	= tester_api->nunit_tester_get_testname();
	if( string_t::glob_match(path_pattern[0], subtest_name.to_string()) == false ){
		// notify a TEST_NOMORE
		return callback->neoip_nunit_runner_completed_cb(userptr, *this);
	}

	// launch the tester_api
	tester_api->nunit_tester_begin(path_pattern.get_subpath(), this, NULL);

	// return a 'tokeep'
	return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  nunit_event_cb_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a nunit_res_t has to be notified
 */
bool	nunit_runner_t::neoip_nunit_event_cb(void *userptr, const nunit_path_t &subtest_path
					, const nunit_event_t &nunit_event)	throw()
{
	// log to debug
	KLOG_DBG("enter nunit_event=" << nunit_event << " test_path=" << subtest_path);
	// handle the event according to their type
	switch( nunit_event.get_value() ){
	case nunit_event_t::TEST_BEGIN:
			// display the TEST_BEGIN
			KLOG_STDOUT("Begin " << subtest_path << "...");
			return true;
	case nunit_event_t::TEST_END:{
			nunit_res_t	nunit_res	= nunit_event.get_test_end_result();
			// update the statistic
			nb_test_completed++;
			if( nunit_res.error() == nunit_err_t::OK )	nb_test_succeed++;
			// display the TEST_END
			KLOG_STDOUT(nunit_res << std::endl);
			return true;}
	case nunit_event_t::TEST_NOMORE:{
			// TODO test the result in case it failed
			if( nunit_event.get_test_nomore_result() != nunit_err_t::OK ){
				DBG_ASSERT( 0 );
			}
			// TEST_NOMORE
			KLOG_STDOUT("nunit_runner Completed.");
			// display the success ratio
			double	success_ratio;
			if( nb_test_completed )	success_ratio = (double)nb_test_succeed / nb_test_completed;
			else			success_ratio = 1;
			KLOG_STDOUT((success_ratio*100.0) << "% tests succeed (");
			KLOG_STDOUT( nb_test_succeed << " succeed/");
			KLOG_STDOUT((nb_test_completed - nb_test_succeed) << " failed)");
			KLOG_STDOUT( std::endl );
			// notify the completion callback
			return callback->neoip_nunit_runner_completed_cb(userptr, *this);}
	default:	DBG_ASSERT( 0 );
	}
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return false;
}

NEOIP_NAMESPACE_END




