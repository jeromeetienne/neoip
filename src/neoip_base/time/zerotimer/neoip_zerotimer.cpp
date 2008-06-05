/*! \file
    \brief Definition of the \ref zerotimer_t.cpp

*/

/* system include */
/* local include */
#include "neoip_zerotimer.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_assert.hpp"
#include "neoip_string.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if there is already a 'callback, userptr) already queued in the zerotimer_t
 */
bool	zerotimer_t::contain(zerotimer_cb_t * callback, void *userptr)	const throw()
{
	std::list<cbuserptr_t>::const_iterator	iter;
	// go thry the whole cbuserptr_list
	for(iter = cbuserptr_list.begin(); iter != cbuserptr_list.end(); iter++){
		// if this element is equal to the one in parameter, return true
		if( *iter == cbuserptr_t(callback, userptr) )	return true;
	}
	// if the whole loop happened without returning, it is not contained
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         insert/remove
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief insert a new callback to call when the zerotimer_t is triggered
 */
void zerotimer_t::append(zerotimer_cb_t * callback, void *userptr)	throw()
{
	// init the timeout if it isnt already pending
	if( !zero_timeout.is_running() )	zero_timeout.start(delay_t(0), this, NULL);

	// append the callback/userptr to the list
	cbuserptr_list.push_back(cbuserptr_t(callback, userptr));
}

/** \brief remove the first occurance of this callback/userptr from the zerotimer_t
 */
void zerotimer_t::remove(zerotimer_cb_t * callback, void *userptr)	throw()
{
	// remove the callback/userptr from the list
	cbuserptr_list.remove(cbuserptr_t(callback, userptr));
	
	// if zero_timeout is pending and the list is now empty, delete the timer and mark it unused
	if( cbuserptr_list.size() == 0 && zero_timeout.is_running() )
		zero_timeout.stop();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         timeout expiration
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool zerotimer_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
// copy the list to allow modification of it during the callback
	// copy the list in a temporary var
	std::list<cbuserptr_t>	tmp_list = cbuserptr_list;
	// clear cbuserptr_list 
	cbuserptr_list.clear();
	// stop the timeout_t 
	// - MUST be done before the callback thus the callback may add elements and restart the timer
	zero_timeout.stop();
	
	// notify all the callback from the list
	std::list<cbuserptr_t>::iterator	iter;
	for( iter = tmp_list.begin(); iter != tmp_list.end(); iter++ ){
		const cbuserptr_t &	cbuserptr = *iter;
		// backup the tokey_check_t context to check after the callback notification
		TOKEEP_CHECK_BACKUP_DFL(*cbuserptr.callback);	
		// notify the callback
		bool tokeep	= cbuserptr.callback->neoip_zerotimer_expire_cb(*this, cbuserptr.userptr);
		// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
		TOKEEP_CHECK_MATCH_DFL(tokeep);
		// stop the notification if callback return true
		if( !tokeep )	return false;
	}
	// return tokeey
	return true;
}

NEOIP_NAMESPACE_END




