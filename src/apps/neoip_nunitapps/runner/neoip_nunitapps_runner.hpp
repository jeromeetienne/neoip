/*! \file
    \brief Header of the \ref nunitapps_runner_t class

*/


#ifndef __NEOIP_NUNITAPPS_RUNNER_HPP__ 
#define __NEOIP_NUNITAPPS_RUNNER_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_runner.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	nunit_suite_t;
class	nunit_gsuite_t;
class	clineopt_arr_t;

/** \brief The unit test class to run a nunit_suite_
 */
class nunitapps_runner_t : private nunit_runner_cb_t {
private:
	nunit_suite_t *		nunit_suite;	//!< ptr on a the nunit_suite_t which is run

	/**************** nunit_runner_t	*******************************/	
	nunit_runner_t*		nunit_runner;	//!< the nunit_runner_t which run the suite
	bool neoip_nunit_runner_completed_cb(void *userptr, nunit_runner_t &nunit_runner)	throw();
public:
	/*************** ctor/dtor	***************************************/
	nunitapps_runner_t(const nunit_path_t &path_pattern, nunit_gsuite_t *nunit_gsuite)	throw();
	~nunitapps_runner_t()									throw();

	/*************** query function	***************************************/
	static clineopt_arr_t	clineopt_arr()		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNITAPPS_RUNNER_HPP__  */


 
