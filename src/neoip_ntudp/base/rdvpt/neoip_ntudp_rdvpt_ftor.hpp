/*! \file
    \brief Header of the ntudp_rdvpt_t callback
*/


#ifndef __NEOIP_NTUDP_RDVPT_FTOR_HPP__ 
#define __NEOIP_NTUDP_RDVPT_FTOR_HPP__ 
/* system include */
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_rdvpt_t;

/** \brief the callback class filtering out ntudp_rdvpt_t
 */
class ntudp_rdvpt_ftor_cb_t {
public:
	/** \brief return true if the ntudp_rdvpt_t MUST be rejected, false if it MUST be accepted
	 */
	virtual bool ntudp_rdvpt_ftor_cb(void *userptr, const ntudp_rdvpt_t &ntudp_rdvpt)throw() = 0;
	virtual ~ntudp_rdvpt_ftor_cb_t() {};
};

/** \brief Functor for filtering out ntudp_rdvpt_t
 * 
 * - NOTE: this object MUST be copiable
 */
class ntudp_rdvpt_ftor_t : NEOIP_COPY_CTOR_ALLOW {
private:
	ntudp_rdvpt_ftor_cb_t*	callback;		//!< the callback to notify when the functor is called
	void *			userptr;	//!< the userptr associated with the above callback
public:
	/** \brief Constructor of the functor
	 */
	ntudp_rdvpt_ftor_t(ntudp_rdvpt_ftor_cb_t *callback = NULL, void *userptr = NULL) throw()
	{
		this->callback	= callback;
		this->userptr	= userptr;
	}

	/** \brief return true if the ntudp_rdvpt_t MUST be rejected, false if it MUST be accepted
	 * 
	 * - overload the () operator (as it is a functor)
	 * - this function simply calls a callback with a userptr
	 * - it is used to have a better abstraction of the procedure
	 */
	bool operator()(const ntudp_rdvpt_t &ntudp_rdvpt)	const throw() {
		// if it is not set, accept all ntudp_rdvpt_t
		if( !callback )	return false;
		// call the callback in the ntudp_rdvpt_t template
		return callback->ntudp_rdvpt_ftor_cb(userptr, ntudp_rdvpt);
	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RDVPT_FTOR_HPP__  */



