/*! \file
    \brief Header of the nunit_testclass_t callback
*/


#ifndef __NEOIP_NUNIT_TESTCLASS_FTOR_HPP__ 
#define __NEOIP_NUNIT_TESTCLASS_FTOR_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_nunit_res.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief the callback class for reporting nunit_res_t in case of async reply in 
 *         test function.
 */
class nunit_testclass_ftor_cb_t {
public:
	/** \brief called when a nunit_res_t has to be notified async from a test function
	 * 
	 * @return tokeep aka true if the object still exist after the callback, false if it has
	 *         been deleted.
	 */
	virtual bool nunit_testclass_ftor_cb(void *userptr, const nunit_res_t &nunit_res)throw() = 0;
	virtual ~nunit_testclass_ftor_cb_t() {};
};

/** \brief Functor for asynchronous notification of result from a nunit_testclass_api_t 
 *         implementation to a nunit_testclass_t object.
 * 
 * - NOTE: this object MUST be copiable
 */
class nunit_testclass_ftor_t : NEOIP_COPY_CTOR_ALLOW {
private:
	nunit_testclass_ftor_cb_t *	callback;	//!< the callback to notify when the functor is called
	void *				userptr;	//!< the userptr associated with the above callback
public:
	/** \brief Constructor of the functor
	 */
	nunit_testclass_ftor_t(nunit_testclass_ftor_cb_t *callback = NULL, void *userptr = NULL) throw()
	{
		this->callback	= callback;
		this->userptr	= userptr;
	}

	/** \brief overload the () operator (as it is a functor)
	 * 
	 * - this function simply calls a nunit_event_cb_t with a userptr
	 * - it is used to have a better abstraction of the procedure
	 *   - it may be usefull later e.g. if a given testclass call several testfunction
	 *     simulataneously, it would need a kind of slot_id_t 
	 */
	bool operator()(const nunit_res_t &nunit_res)	const throw() {
		// call the callback in the nunit_testclass_t template
		return callback->nunit_testclass_ftor_cb(userptr, nunit_res);
	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_TESTCLASS_FTOR_HPP__  */



