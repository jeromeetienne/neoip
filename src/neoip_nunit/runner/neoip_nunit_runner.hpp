/*! \file
    \brief Header of the nunit_testclass_t
*/


#ifndef __NEOIP_NUNIT_RUNNER_HPP__ 
#define __NEOIP_NUNIT_RUNNER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_nunit_runner_cb.hpp"
#include "neoip_nunit_event_cb.hpp"
#include "neoip_nunit_path.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
class nunit_tester_api_t;

/** \brief class to store and run nunit_test_api_t implementation
 */
class nunit_runner_t : private nunit_event_cb_t, private zerotimer_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	nunit_runner_cb_t *	callback;	//!< the callback to notifiy when the nunit_runner_t is completed
	void *			userptr;	//!< the userptr associated with the above callback
	nunit_tester_api_t *	tester_api;	//!< the nunit_tester_api_t to run
	nunit_path_t		path_pattern;	//!< the pattern of nunit_path_t to run
	
	size_t		nb_test_completed;	//!< the number of test which reported a result
	size_t		nb_test_succeed;	//!< the number of test which reported a success
	
	/*************** the event callback to received result	***************/
	bool		neoip_nunit_event_cb(void *userptr, const nunit_path_t &subtest_path
					, const nunit_event_t &nunit_event)	throw();

	/*************** zerotimer stuff to avoid notifying in nunit_tester_begin() *******/
	zerotimer_t	zerotimer;
	bool 		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer
							, void *userptr) 	throw();
public:
	/**************** ctor/dtor	***************************************/
	nunit_runner_t()	throw();
	~nunit_runner_t()	throw();

	/**************** setup function	*******************************/
	void	start(const nunit_path_t &path_pattern, nunit_tester_api_t *tester_api
				, nunit_runner_cb_t *callback, void *userptr)	throw();
};

	
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_RUNNER_HPP__  */



