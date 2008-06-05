/*! \file
    \brief Header of the nunit_testclass_t
*/


#ifndef __NEOIP_NUNIT_TESTCLASS_HPP__ 
#define __NEOIP_NUNIT_TESTCLASS_HPP__ 
/* system include */
#include <string>
#include <vector>
/* local include */
#include "neoip_nunit_err.hpp"
#include "neoip_nunit_res.hpp"
#include "neoip_nunit_event.hpp"
#include "neoip_nunit_path.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_nunit_tester_api.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
class nunit_event_cb_t;

/** \brief class to store all the testfunctions in a given testclass
 * 
 * - NOTE: the testclass_ptr MUST be allocated by nipmem_new (aka NOT by new)
 * - NOTE: this class is a template as it needs to store and call pointer to member
 *         of class implementing nunit_testclass_api_t
 */
template <typename T> class nunit_testclass_t : public nunit_tester_api_t, private zerotimer_cb_t 
						, private nunit_testclass_ftor_cb_t
						, NEOIP_COPY_CTOR_DENY {
public:
	typedef  nunit_res_t (T::*testfct_ptr_t)(const nunit_testclass_ftor_t &testclass_ftor);//!< define the type of the type function
	struct	testfct_t {
		// TODO make a real class out of this
		testfct_ptr_t	testfct_ptr;
		nunit_path_t	testfct_name;
	};
	
	/*************** zerotimer stuff to avoid notifying in nunit_tester_begin() *******/
	zerotimer_t	zerotimer;
	bool 		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw();

private:
	T *			testclass_ptr;		//!< pointer on the testclass object
	nunit_path_t		local_testname;		//!< the name of the testclass object
	std::vector<testfct_t>	subtest_db;		//!< to store all the testfct in this testclass object
	
	/**************** variable used ONLY during testing	***************/
	nunit_event_cb_t *	callback;		//!< the callback to notify the test result
	void *			userptr;		//!< the userptr associated with the callback
	nunit_path_t		path_pattern;		//!< the name's pattern of the tests to execute
	size_t			cur_test_idx;		//!< the index of the current test in subtest_db
	bool			delayed_inprogress;	//!< true is a test is inprogress, false otherwise
	
	/*************** callback to received async result from ftor	*******/
	bool	 nunit_testclass_ftor_cb(void *userptr, const nunit_res_t &nunit_res)	throw();
public:
	/**************** ctor/dtor	***************************************/
	nunit_testclass_t(const nunit_path_t &local_testname, T *testclass_ptr)	throw();
	~nunit_testclass_t()	throw();


	/**************** setup function	*******************************/
	nunit_testclass_t &	append(const std::string &testfctname, testfct_ptr_t testfct_ptr)throw();
	size_t			size()			const throw()	{ return subtest_db.size();	}
	const std::string &	operator[](int idx)	const throw()	{ return subtest_db[idx].testfct_name;	}

	/**************** nunit_tester_api_t implementation	***************/
	void	nunit_tester_begin(const nunit_path_t &testname_pattern
				, nunit_event_cb_t *callback, void *userptr)	throw();
	void	nunit_tester_end()						throw();
	void	nunit_tester_get_allname(const nunit_path_t &path_prefix, const nunit_path_t &path_pattern
					, std::list<nunit_path_t> &testname_db)	const throw();
	const	nunit_path_t &	nunit_tester_get_testname()			const throw();
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 * 
 * @param testname	The name of this nunit_testclass_t
 * @param testclass_ptr		a pointer on the testclass implementation - it MUST be allocated
 *                              by nipmem_new (aka NOT by new) and will be freed when the 
 *                              nunit_testclass_t will be destructed.
 */
template <typename T>
nunit_testclass_t<T>::nunit_testclass_t(const nunit_path_t &local_testname, T *testclass_ptr)	throw()
{
	// copy the paramters
	this->testclass_ptr	= testclass_ptr;
	this->local_testname	= local_testname;
}

/** \brief Destructor		
 */
template <typename T>
nunit_testclass_t<T>::~nunit_testclass_t()	throw()
{
	// call nunit_tester_end() just in case
	nunit_tester_end();
	// delete the class object
	nipmem_delete testclass_ptr;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        append function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T>
nunit_testclass_t<T> &nunit_testclass_t<T>::append(const std::string &testfct_name
						, testfct_ptr_t testfct_ptr)	throw()
{
	// log to debug
	KLOG_DBG("Add testfct name=" << testfct_name << " with ptr=" << testfct_ptr);
	// build the testfct_t
	testfct_t	testfct;
	testfct.testfct_ptr	= testfct_ptr;
	testfct.testfct_name	= testfct_name;
	// add the testfct_t to the array
	subtest_db.push_back(testfct);
	// return the object itself
	return *this;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    zerotimer_cb_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
template <typename T>
bool	nunit_testclass_t<T>::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	// sanity check - the delayed_inprogress MUST be false
	DBG_ASSERT(delayed_inprogress == false);

	// if it is the first time this function is called, init the testclass_api_t
	if( cur_test_idx == 0 ){
		nunit_err_t	nunit_err = testclass_ptr->neoip_nunit_testclass_init();
		// if the init failed, nomore a TEST_NOMORE immediatly
		if( nunit_err != nunit_err_t::OK ){
			// notify a TEST_NOMORE event
			nunit_event_t	nunit_event	= nunit_event_t::build_test_nomore(nunit_err);
			return callback->neoip_nunit_event_cb(userptr, local_testname, nunit_event);
		}
	}

	// loop for all the remaining the test functions
	while( true ){
		// if there are nomore testfunction to call
		if( cur_test_idx == subtest_db.size() ){
			// notify a nunit_event_t::TEST_NOMORE event
			nunit_event_t	nunit_event	= nunit_event_t::build_test_nomore();
			return callback->neoip_nunit_event_cb(userptr, local_testname, nunit_event);
		}
	
		// get the current test function
		testfct_t &	testfct		= subtest_db[cur_test_idx];
		// if the path_pattern doesnt match the subtest_name, skip it
		if( !string_t::glob_match(path_pattern[0], testfct.testfct_name.to_string()) ){
			cur_test_idx++;
			continue;	
		}
		// notify a nunit_event_t::TEST_BEGIN
		bool tokeep=callback->neoip_nunit_event_cb(userptr, local_testname / testfct.testfct_name
							, nunit_event_t::build_test_begin());
		if( !tokeep )	return false;
		// build the functor
		nunit_testclass_ftor_t		ftor(this, NULL);
		// call the test function itself
		nunit_res_t	nunit_res	= (testclass_ptr->*testfct.testfct_ptr)(ftor);
		// handle the delayed case
		if( nunit_res.error() == nunit_err_t::DELAYED ){
			delayed_inprogress	= true;
			return true;
		}
		// notify a nunit_event_t::TEST_END
		tokeep = callback->neoip_nunit_event_cb(userptr, local_testname / testfct.testfct_name
							, nunit_event_t::build_test_end(nunit_res));
		if( !tokeep )	return false;
		// goto the next test
		cur_test_idx++;		
	}


	// return a 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    nunit_testclass_ftor callback implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a nunit_res_t has to be notified async from a test function
 */
template <typename T>
bool	nunit_testclass_t<T>::nunit_testclass_ftor_cb(void *userptr, const nunit_res_t &nunit_res)throw()
{
	testfct_t &	testfct		= subtest_db[cur_test_idx];
	nunit_path_t	test_name	= local_testname / testfct.testfct_name;	
	bool		tokeep;
	// log to debug
	KLOG_DBG("enter");

	// sanity check
	DBG_ASSERT( delayed_inprogress );
	// mark that the test is nomore in progress
	delayed_inprogress	= false;

	// notify a nunit_event_t::TEST_END with the nunit_res_t
	nunit_event_t	nunit_event;
	nunit_event	= nunit_event_t::build_test_end(nunit_res);
	tokeep		= callback->neoip_nunit_event_cb(userptr, test_name, nunit_event);
	if( !tokeep )	return false;

	// goto the next test
	cur_test_idx++;
	
	// launch the zerotimer to run the next test
	zerotimer.append(this, NULL);
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    nunit_tester_api_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testing phase
 */
template <typename T>
void		nunit_testclass_t<T>::nunit_tester_begin(const nunit_path_t &path_pattern
				, nunit_event_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->path_pattern	= path_pattern;
	this->callback		= callback;
	this->userptr		= userptr;
	// zero some field
	cur_test_idx		= 0;
	delayed_inprogress	= false;
	// init zerotimer to start the testing and especialy avoid notifying in event from here
	zerotimer.append(this, NULL);
}


/** \brief DeInit the testing phase
 */
template <typename T>
void	nunit_testclass_t<T>::nunit_tester_end()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// deinit the testclass_ptr
	testclass_ptr->neoip_nunit_testclass_deinit();
	// zero some parameters - not really required - just to be 'cleaner'
	path_pattern	= nunit_path_t();
	callback	= NULL;
	userptr		= NULL;
	cur_test_idx	= 0;
	delayed_inprogress	= false;
}

/** \brief Return the list of all the testname of this nunit_tester_api_t and
 *         the ones below, if any.
 */
template <typename T>
void	nunit_testclass_t<T>::nunit_tester_get_allname(const nunit_path_t &path_prefix
					, const nunit_path_t &path_pattern
					, std::list<nunit_path_t> &testname_db)	const throw()
{
	nunit_path_t	test_basename	= path_prefix / local_testname;
	// go thru all the testfct_t of this nunit_testclass_t
	for( size_t i = 0; i < subtest_db.size(); i++ ){
		const testfct_t &	testfct	= subtest_db[i];
		// TODO to code the filter
		if( 0 /* the name doesnt match the pattern */ )	continue;
		// add the testname to testname_db
		testname_db.push_back( test_basename / testfct.testfct_name );
	}
}

/** \brief Return the name of this nunit_tester_api_t
 */
template <typename T>
const nunit_path_t &	nunit_testclass_t<T>::nunit_tester_get_testname()	const throw()
{
	return	local_testname;
}

	
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_TESTCLASS_HPP__  */



