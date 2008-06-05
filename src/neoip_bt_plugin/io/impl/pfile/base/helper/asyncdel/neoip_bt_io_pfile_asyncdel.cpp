/*! \file
    \brief Class to handle the bt_io_pfile_asyncdel_t


\par Notes
- About the ctor/dtor order, it MUST be initialized after the bt_swarm_t and deleted before.
- Implement various policy to determine which piece to delete
  - the information available to make the decision are 
    - bt_prarity_t
    - bt_pieceprec_arr_t
    - bt_pieceavail_t

\par TODO
- not sure this is the good way to handle deletion
  - the timer thing is poor
  - in fact this is equivalent to a poll vs intr
  - imagine what happen for very large number of piece
- implement other policy
  - define what are some usefull policy
  - 2 stages: should i delete ? which piece to delete ?
  - a http offload ala dijjer, aka an internal caching
    - aka the data of this bt_swarm_t remain in the disk from one iteration to another
  - a http offload with a backqueue
    - aka the data are shared only during the duration of the local connection
    - is there 2 cases ?
      1. one for the stream where is it absolutly required as the data are shared ONLY
         inside the backqueue and frontqueue
      2. one for the offload static 
  - a bt_swarm caching ala cachelogic
    - aka it aims to run on a big bw box and reduce the global bandwidth

*/

/* system include */
/* local include */
#include "neoip_bt_io_pfile_asyncdel.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_pfile_asyncdel_t::bt_io_pfile_asyncdel_t()		throw()
{
	// zero some field
	bt_swarm	= NULL;
}

/** \brief Destructor
 */
bt_io_pfile_asyncdel_t::~bt_io_pfile_asyncdel_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_io_pfile_asyncdel_t &	bt_io_pfile_asyncdel_t::set_profile(const bt_io_pfile_asyncdel_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}
/** \brief Start the operation
 */
bt_err_t	bt_io_pfile_asyncdel_t::start(const bt_io_pfile_asyncdel_policy_t &asyncdel_policy
						, bt_swarm_t *bt_swarm)	throw()
{
	// copy the parameter
	this->asyncdel_policy	= asyncdel_policy;
	this->bt_swarm		= bt_swarm;

	// init and start the timeout
	deletor_delaygen = delaygen_t(profile.deletor_delaygen());
	deletor_timeout.start(deletor_delaygen.current(), this, NULL);
	
	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool	bt_io_pfile_asyncdel_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_WARN("enter");	
	// notify the expiration of the timeout to the delaygen
	deletor_delaygen.notify_expiration();
	// sanity check - delaygen MUST NOT timeout
	DBG_ASSERT( !deletor_delaygen.is_timedout() );

	// launch a deletor iteration
	switch(asyncdel_policy.get_value()){
	case bt_io_pfile_asyncdel_policy_t::ALL_NOTREQUIRED:
			delete_all_notrequired();
			break;
	default:	DBG_ASSERT(0);
	}

	// set the next timer
	deletor_timeout.change_period(deletor_delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Delete all piece which are locally available and notneeded
 * 
 * - queue all the pieceidx to delete in the del_pieceidx_db
 * - the pieceidx will be deleted one at a time via zerotimer_t because the 
 *   piece deletion may be blocking 
 */
void	bt_io_pfile_asyncdel_t::delete_all_notrequired()	throw()
{
	bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
	const bt_pieceavail_t &	local_pavail	= bt_swarm->local_pavail();
	// log to debug
	KLOG_DBG("enter");
	// go thru each piece
	for(size_t pieceidx = 0; pieceidx < local_pavail.size(); pieceidx++){
		// if this pieceidx is not available, go to the next
		if( !local_pavail.is_avail(pieceidx) )			continue;
		// if this pieceidx is needed, go to the next
		if( !pselect_vapi->pieceprec(pieceidx).is_notneeded() )	continue;
		// queue this pieceidx into the del_pieceidx_db
		del_pieceidx_db.push_back(pieceidx);
	}
	// if del_pieceidx_db is not empty, relaunch the del_piece_zerotimer
	if( !del_pieceidx_db.empty() && del_pieceidx_zerotimer.empty() )
		del_pieceidx_zerotimer.append(this, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       err_zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - used to delete only one pieceidx in a given loop iteration
 * - this is important as bt_swarm->declare_piece_nomore_avail may do a blocking
 *   delete on the disk
 */
bool	bt_io_pfile_asyncdel_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	const bt_pieceavail_t &	local_pavail	= bt_swarm->local_pavail();
	bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the del_pieceidx_db MUST NOT be empty at this point
	DBG_ASSERT( !del_pieceidx_db.empty() );
	// get the first pieceidx from the del_pieceidx_db
	size_t	pieceidx	= del_pieceidx_db.front();
	del_pieceidx_db.pop_front();
	// if del_pieceidx_db is not empty, relaunch the del_piece_zerotimer
	if( !del_pieceidx_db.empty() )	del_pieceidx_zerotimer.append(this, NULL);

	/* start checking if the pieceidx is still to be deleted.
	 * - ok it was to be deleted when it has been inserted in the database
	 * - but condition may have changed between then and now
	 * - so recheck to avoid any race condition
	 */
	// if this pieceidx is unavailable, go to the next
	if( local_pavail.is_unavail(pieceidx) )			return true;
	// if this pieceidx is needed, go to the next
	if( pselect_vapi->pieceprec(pieceidx).is_needed() )	return true;
		
	// actually delete this pieceidx
	bt_swarm->declare_piece_nomore_avail(pieceidx);
	// return tokeep
	return true;	
}


NEOIP_NAMESPACE_END





