/*! \file
    \brief Header of the \ref zerotimer_t

*/


#ifndef __NEOIP_ZEROTIMER_HPP__ 
#define __NEOIP_ZEROTIMER_HPP__ 

/* system include */
#include <iostream>
#include <string>
#include <list>
/* local include */
#include "neoip_zerotimer_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store zero timers
 * 
 * - the callback are called in order of insertion
 * - TODO:
 *   - this class is only used to get a run in the next loop, and a zero timer
 *     may have a different order depending on how the core loop handle the thing
 */
class zerotimer_t : NEOIP_COPY_CTOR_ALLOW, public timeout_cb_t {
private:
	/*************** timeout_t	***************************************/
	timeout_t		zero_timeout;
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();

	/*************** callback	***************************************/
	class			cbuserptr_t;
	std::list<cbuserptr_t>	cbuserptr_list;
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	zerotimer_t()						throw()	{}
	~zerotimer_t()						throw()	{}
	
	/*************** query function	***************************************/
	size_t	size()	const throw()	{ return cbuserptr_list.size();		}
	bool	empty()	const throw()	{ return cbuserptr_list.empty();	}
	bool	contain(zerotimer_cb_t * callback, void *userptr)	const throw();

	/*************** action function	*******************************/
	void	append(zerotimer_cb_t * callback, void *userptr)	throw();
	void	remove(zerotimer_cb_t * callback, void *userptr)	throw();
};

/** \brief handle a individual callback in the zerotimer_t
 */
class zerotimer_t::cbuserptr_t : NEOIP_COPY_CTOR_ALLOW {
public:
	zerotimer_cb_t *	callback;	//!< the callback to call
	void *			userptr;	//!< the userptr associated with the callback
	cbuserptr_t(zerotimer_cb_t * callback, void *userptr)	throw()
		: callback(callback), userptr(userptr) {}
	bool operator == (const zerotimer_t::cbuserptr_t & other) const throw() {
		if( callback != other.callback )	return false;
		if( userptr  != other.userptr  )	return false;
		return true;
	}
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_ZEROTIMER_HPP__  */



