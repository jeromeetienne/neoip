/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_nunit_suite.hpp"
#include "neoip_nunit_event.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
nunit_suite_t::nunit_suite_t(const nunit_path_t &local_testname)	throw()
{
	// copy the parameter
	this->local_testname	= local_testname;
	// zero some local variables
	cur_test_idx		= 0;
	test_inprogress		= false;
}

/** \brief Destructor
 */
nunit_suite_t::~nunit_suite_t()				throw()
{
	// call nunit_tester_end() just in case
	nunit_tester_end();
	// free all elements in subtest_db
	while( !subtest_db.empty() ){
		std::vector<nunit_tester_api_t *>::iterator	iter	= subtest_db.begin();
		// delete the object pointed by the first element
		nipmem_delete	*iter;
		// delete the element itself
		subtest_db.erase(iter);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           insert a nunit_tester_api_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a nunit_tester_api_t to this nunit_suite_t
 * 
 * @param tester_api	a pointer on a nunit_tester_api_t - it MUST be allocated
 *                      by a nipmem_new (aka NOT a new). and will be freed when
 *                      the nunit_suite_t will be destructed.
 * 
 * @return false if an error occured, true otherwise
 */
void	nunit_suite_t::append(nunit_tester_api_t *tester_api)	throw()
{
	// add the nunit_tester_api_t to the subtest_db
	subtest_db.push_back(tester_api);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  nunit_event_cb_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a nunit_res_t has to be notified
 */
bool	nunit_suite_t::neoip_nunit_event_cb(void *userptr, const nunit_path_t &subtest_path
					, const nunit_event_t &nunit_event)	throw()
{
	nunit_path_t	test_path	= local_testname / subtest_path;
	// log to debug
	KLOG_DBG("enter nunit_event=" << nunit_event << " test_path=" << test_path);
	// handle the event according to their type
	switch( nunit_event.get_value() ){
	case nunit_event_t::TEST_BEGIN:
	case nunit_event_t::TEST_END:
			// just forward the event
			return callback->neoip_nunit_event_cb(userptr, test_path, nunit_event);
	case nunit_event_t::TEST_NOMORE:
			// TODO test this case
//			EXP_ASSERT( nunit_event.get_test_nomore_result() != nunit_err_t::OK );
	
			// mark that the current test is completed
			test_inprogress	= false;
			// call tester_end() on the current test
			subtest_db[cur_test_idx]->nunit_tester_end();
			// goto the next test
			cur_test_idx++;
			// init zerotimer to run the next test
			zerotimer.append(this, NULL);	
			// return a tokeep	
			return true;
	default:	DBG_ASSERT( 0 );
	}
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  zerotimer_cb_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - this function launch the current tester_api_t if any. if not, it notify
 *   a nunit_event_t::TEST_NOMORE
 */
bool	nunit_suite_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// find the first nunit_tester_api_t which match the path_pattern
	for( ; cur_test_idx < subtest_db.size(); cur_test_idx++ ){
		// get the subtest_name
		nunit_path_t	subtest_name	= subtest_db[cur_test_idx]->nunit_tester_get_testname();
		// if the path_pattern match the subtest_name, notify a exit
		if( string_t::glob_match(path_pattern[0], subtest_name.to_string()) )	break;
	}


	// if there are no more tester_api_t to run, notify nunit_event_t::TEST_NOMORE and exit
	if( cur_test_idx == subtest_db.size() ){
		// notify a TEST_NOMORE
		return callback->neoip_nunit_event_cb(userptr, local_testname
						, nunit_event_t::build_test_nomore());
	}

	// Start the next tester_api
	subtest_db[cur_test_idx]->nunit_tester_begin(path_pattern.get_subpath(), this, NULL);

	// set the test_inprogress
	test_inprogress	= true;

	// return a 'tokeep'
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    nunit_tester_api_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Begin the tester
 */
void	nunit_suite_t::nunit_tester_begin(const nunit_path_t &path_pattern
				, nunit_event_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->path_pattern	= path_pattern;
	this->callback		= callback;
	this->userptr		= userptr;
	// zero some local variables
	cur_test_idx		= 0;
	test_inprogress		= false;
	// init zerotimer to start the testing and especialy avoid notifying in event from here
	zerotimer.append(this, NULL);
}

/** \brief End the tester
 * 
 * - this function may be called even is nunit_tester_begin(...) has not been called
 */
void	nunit_suite_t::nunit_tester_end()					throw()
{
	// if a tester_api_t test is in progress, stop it
	if( test_inprogress )	subtest_db[cur_test_idx]->nunit_tester_end();
	// zero some variable
	cur_test_idx	= 0;
	test_inprogress	= false;
}

/** \brief Return the list of all the testname of this nunit_tester_api_t and
 *         the ones below, if any.
 */
void	nunit_suite_t::nunit_tester_get_allname(const nunit_path_t &path_prefix
					, const nunit_path_t &path_pattern
					, std::list<nunit_path_t> &testname_db)	const throw()
{
	nunit_path_t	new_prefix = path_prefix / local_testname;
	// go thru all the nunit_tester_api_t of this nunit_suite_t
	for( size_t i = 0; i < subtest_db.size(); i++ )
		subtest_db[i]->nunit_tester_get_allname(new_prefix, path_pattern, testname_db);
}

/** \brief Return the name of this nunit_tester_api_t
 */
const nunit_path_t &	nunit_suite_t::nunit_tester_get_testname()	const throw()
{
	return	local_testname;
}



NEOIP_NAMESPACE_END




