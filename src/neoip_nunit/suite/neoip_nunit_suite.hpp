/*! \file
    \brief Header of the nunit_testclass_t
*/


#ifndef __NEOIP_NUNIT_SUITE_HPP__ 
#define __NEOIP_NUNIT_SUITE_HPP__ 
/* system include */
#include <string>
#include <vector>
/* local include */
#include "neoip_nunit_tester_api.hpp"
#include "neoip_nunit_event_cb.hpp"
#include "neoip_nunit_path.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_log.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief class to store and run nunit_test_api_t implementation
 */
class nunit_suite_t : public nunit_tester_api_t, private nunit_event_cb_t, private zerotimer_cb_t
							, NEOIP_COPY_CTOR_DENY {
private:
	nunit_path_t				local_testname;	//!< the name of this suite
	std::vector<nunit_tester_api_t *>	subtest_db;	//!< all the nunit_tester_api_t of this nunit_tester_api_t

	bool	neoip_nunit_event_cb(void *userptr, const nunit_path_t &subtest_path
					, const nunit_event_t &nunit_event)	throw();

	/*************** zerotimer stuff to avoid notifying in nunit_tester_begin() *******/
	zerotimer_t	zerotimer;
	bool 		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw();
	
	/**************** variable used ONLY during testing	***************/
	nunit_event_cb_t *	callback;		//!< the event_cb to which report the nunit_event_t
	void *			userptr;		//!< the userptr associated with the above callback
	nunit_path_t		path_pattern;		//!< the pattern of the name to run
	size_t			cur_test_idx;		//!< the current test idx
	bool			test_inprogress;	//!< true if a test is currently in progress, false otherwise
public:
	/**************** ctor/dtor	***************************************/
	nunit_suite_t(const nunit_path_t &local_testname)	throw();
	~nunit_suite_t()					throw();

	void	append(nunit_tester_api_t *tester_api)		throw();

	/**************** nunit_tester_api_t implementation	***************/
	void	nunit_tester_begin(const nunit_path_t &path_pattern
				, nunit_event_cb_t *callback, void *userptr)	throw();
	void	nunit_tester_end()						throw();
	void	nunit_tester_get_allname(const nunit_path_t &path_prefix, const nunit_path_t &path_pattern
					, std::list<nunit_path_t> &testname_db)	const throw();
	const nunit_path_t &nunit_tester_get_testname()				const throw();
};

	
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_SUITE_HPP__  */



