/*! \file
    \brief Header of the \ref nunit_runner_t callback

*/


#ifndef __NEOIP_NUNIT_RUNNER_CB_HPP__ 
#define __NEOIP_NUNIT_RUNNER_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class nunit_runner_t;

/** \brief the callback class for reporting the end of a nunit_runner_t
 */
class nunit_runner_cb_t {
public:
	/** \brief called when a nunit_runner_t is completed
	 * 
	 * @return a 'tokeep'
	 */
	virtual bool neoip_nunit_runner_completed_cb(void *userptr, nunit_runner_t &nunit_runner)	throw() = 0;
	virtual ~nunit_runner_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_RUNNER_CB_HPP__  */



