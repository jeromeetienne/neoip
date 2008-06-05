/*! \file
    \brief Header of the nunit_tester_t callback
*/


#ifndef __NEOIP_NUNIT_TESTER_API_HPP__ 
#define __NEOIP_NUNIT_TESTER_API_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class nunit_event_cb_t;
class nunit_path_t;

/** \brief virtual api for the nunit_tester_t
 * 
 * - it is used by nunit_testclass_t and nunit_suite_t
 */
class nunit_tester_api_t {
public:
	/** \brief Begin the tester and execute all the test matching testname_pattern
	 *         reporting result in callback/userptr
	 */
	virtual void	nunit_tester_begin(const nunit_path_t &path_pattern
				, nunit_event_cb_t *callback, void *userptr)	throw() = 0;
	/** \brief End the tester
	 */
	virtual void	nunit_tester_end()	throw() = 0;
	
	/** \brief Return the list of all the testname of this nunit_tester_api_t and
	 *         the ones below, if any.
	 */
	virtual void	nunit_tester_get_allname(const nunit_path_t &path_prefix
				, const nunit_path_t &path_pattern
				, std::list<nunit_path_t> &testname_db_out)	const throw() = 0;

	/** \brief Return the name of this nunit_tester_api_t
	 */
	virtual const nunit_path_t &nunit_tester_get_testname()	const throw()	= 0;

	/*************** virtual destructor	*******************************/
	virtual ~nunit_tester_api_t()	throw()	{}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_TESTER_API_HPP__  */



