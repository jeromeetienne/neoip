/*! \file
    \brief Class to handle the bt_ezswarm_t
    
\par Brief Description
\ref bt_ezswarm_t is a helper on top of bt_swarm_t which handle all the 
'sattelites' classes usually used around a bt_swarm_t.

\par Note about state
Each bt_ezswarm_state_t is handle by a specific stateobj (e.g. bt_ezswarm_alloc_t
handle bt_ezswarm_state_t::ALLOC). After a bt_ezswarm_t::start(), the bt_ezswarm_t
is in bt_ezswarm_state_t::STOPPED, and wont move from there until explicitly asked
to by the caller.

State transition may occurs from inside the stateobj or from the bt_ezwarm_t's caller.

stateobj may be endless or not. an endless state is one which will never be 
completed except in case of error, or in case of explicitly asked to by an 
external request. It is possible to test if a state is endless or not by 
the function bt_ezswarm_state_t::is_endless().

\par change_state required by the stateobj
stateobj notifies completion only thru two functions:
-# bt_ezswarm_t::stateobj_notify_failure(bt_err) to notify a failed completion.
  - It includes a bt_err_t which describe the occured error
  - It cause the bt_ezswarm_t to change_state_now to bt_ezswarm_state_t::ERROR
-# bt_ezswarm_t::stateobj_notify_sucess() to notify a successful completion.
  - It cause the bt_ezswarm_t to change_state_now to the next state using the
    normal routing
- NOTE: stateobj_notify_failure and stateobj_notify_success both cause bt_ezswarm_t
  to change_state_now, so the notifier stateobj will be delete during this call.

\par change_state required by the caller
- bt_ezswarm_t::graceful_change_state(new_state) will cause the bt_ezswarm_t
  to gracefully try ot reach the new_state.
  - here gracefully means wihtout interrupting the current state and going
    thru all the 'normal' states to reach the destination.
  - this is handles by a 'routing table', currently implemented in 
    bt_ezswarm_t::cpu_new_state(). TODO the implementation is currently halfbacked.
  - As it routes thru all the state in a graceful manner, it may take a long
    time before the end_state is reached.
- bt_ezswarm_t::nextiter_change_state(new_state) will cause the bt_ezswarm_t
  to change state at the next iteration of the event loop.
  - this may cause the current state to ungracefully interrupted
  - it garantee that the state
  
\par About the state routing table
the state routing table represents the 'normal' succession of state for a 
bt_ezswarm_t. TODO this part is half-backed

Example of state transition:
-# bt_ezswarm_t start at STOPPED
-# the caller asks for a graceful_change_state(SHARE)
-# if swarm_resumedata.mfile_allocated(), bt_ezswarm_t switch to CHECK
   else it switch to ALLOC
-# once it has been successfully completed, it switch to SHARE. and will stay
   there until explicitly asked to move.
-# the caller decides to stop the bt_ezswarm_t for a while and asks for 
   a graceful_change_state(STOPPED).
-# the bt_ezswarm_t switch to STOPPING, and wait until STOPPING is completed.
   once STOPPING is completed, it switch to STOPPED and sit there.

- If a state transition is not allowed

\par TODO
- there is a bug with the bt_swarm_resumedata_t
  - because it is set once and for all at the creation time
  - suppose a bt_ezswarm_t start with firstboot resumedata.mfile_allocated() == false
  - then it is allocated successfully, then the ezswarm is stopped, and started again
  - the bt_ezswarm_t.swarm_resumedata().mfile_allocated() IS STILL false
  - so it goes into bt_ezswarm_alloc_t but the files are already allocated, so it goes
    in error !!!!
  - possible solution: to update the bt_swarm_resumedata_t locally
    - this is what i have done but my implementation is not clean

*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezswarm_alloc.hpp"
#include "neoip_bt_ezswarm_check.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_ezswarm_stopping.hpp"
#include "neoip_bt_ezswarm_stopped.hpp"
#include "neoip_bt_ezswarm_error.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_pfile.hpp"
#include "neoip_bt_io_sfile.hpp"
#include "neoip_bt_peersrc_http.hpp"	// to forward the update_listen_pview to it
#include "neoip_bt_peersrc_kad.hpp"	// to forward the update_listen_pview to it
#include "neoip_bt_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_ezswarm_t::bt_ezswarm_t()		throw()
{
	// zero some field
	m_ezsession	= NULL;
	m_io_vapi	= NULL;
	ezswarm_alloc	= NULL;
	ezswarm_check	= NULL;
	ezswarm_share	= NULL;
	ezswarm_stopping= NULL;
	ezswarm_stopped	= NULL;
	ezswarm_error	= NULL;
}

/** \brief Destructor
 */
bt_ezswarm_t::~bt_ezswarm_t()		throw()
{
	// unlink this bt_ezswarm_t from the bt_ezsession_t
	if( m_ezsession )	ezsession()->ezswarm_unlink(this);
	// delete the state object
	cur_stateobj_dtor();
	// delete the bt_io_vapi_t
	nipmem_zdelete	m_io_vapi;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_ezswarm_t &	bt_ezswarm_t::set_profile(const bt_ezswarm_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation when there is a bt_swarm_resumedata_t
 * 
 * - it always start with a bt_swarm_resumedata_t even on firtboot
 *   - if it is the firstboot and only the bt_mfile_t is available, it is up to
 *     the caller to create a bt_swarm_resumedata_t::from_mfile(bt_mfile)
 */
bt_err_t	bt_ezswarm_t::start(const bt_swarm_resumedata_t &p_swarm_resumedata
					, const bt_ezswarm_opt_t &ezswarm_opt
					, bt_ezsession_t *m_ezsession
					, bt_ezswarm_cb_t *callback, void *userptr)	throw()
{
	bt_err_t	bt_err;
	// copy the parameters
	this->m_swarm_resumedata= p_swarm_resumedata;
	this->m_opt		= ezswarm_opt;
	this->m_ezsession	= m_ezsession;
	this->callback		= callback;
	this->userptr		= userptr;
	// dolink this bt_ezswarm_t to the bt_ezsession_t
	ezsession()->ezswarm_dolink(this);
	
#ifdef	_WIN32
	// NOTE: currently udp_full_t is bugged on WIN32, so force no peersrc_kad as it
	// use udp_full_t
	// TODO: is this still valid ?
	if( opt().is_peersrc_kad() ){
		KLOG_ERR("Forcing a disable peersrc_kad because udp_full_t is bugged under WIN32");
		this->m_opt	^= bt_ezswarm_opt_t::PEERSRC_KAD;		
	}
#endif


	// sanity check - the bt_swarm_resumedata_t MUST be check().succeed()
	DBG_ASSERT( swarm_resumedata().check().succeed() );
	// sanity check - the bt_mfile MUST be fully init
	DBG_ASSERT( mfile().is_fully_init() );
	// sanity check - at least one bt_io_vapi_t is set in the bt_ezswarm_opt_t
	DBG_ASSERT( opt().is_io_sfile() || opt().is_io_pfile() );
	// sanity check - only one bt_io_vapi_t is set in the bt_ezswarm_opt_t
	DBG_ASSERT( opt().is_io_sfile() != opt().is_io_pfile() );
	// sanity check - if bt_io_pfile_t is configured, profile().io_pfile() MUST be sne
	if( opt().is_io_pfile() )	DBG_ASSERT( profile().io_pfile().check().succeed() );
	// sanity check - if bt_peersrc_kad_t is configured, profile().peersrc_kad_peer() MUST be set
	if( opt().is_peersrc_kad() )	DBG_ASSERT( profile().peersrc_kad_peer() );
	// sanity check - at least one external bt_peersrc_vapi_t MUST be set
	// - to find the first remote peer. aka bootstrapping
	// - if only bt_peersrc_utpex_t, the first remote peer can not be found
	DBG_ASSERT( opt().is_peersrc_kad() || opt().is_peersrc_http() || opt().is_peersrc_nslan());


	// build the bt_io_vapi_t for this bt_ezswarm_t
	bt_err	= bt_io_vapi_ctor();
	if( bt_err.failed() )	return bt_err;

	// set the current state as bt_ezswarm_state_t::STOPPED
	// - this is hardcoded without using change_state_now to avoid the 
	//   callbacks which may create a nested notificate
	// - bt_ezswarm_t is in_stopped() just after start() and dont move from there
	//   until it is explicitly asked to.
	m_cur_state	= bt_ezswarm_state_t::STOPPED;
	bt_err		= cur_stateobj_ctor();
	if( bt_err.failed() )	return bt_err;
	
	// return no error
	return bt_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Construct the bt_io_vapi_t according to the current bt_mfile_t and bt_ezswarm_opt_t
 */
bt_err_t	bt_ezswarm_t::bt_io_vapi_ctor()					throw()
{
	bt_err_t	bt_err;
	// sanity check - m_io_vapi MUST be NULL
	DBG_ASSERT( !m_io_vapi ); 
	// sanity check - at least one bt_io_vapi_t is set in the bt_ezswarm_opt_t
	DBG_ASSERT( opt().is_io_sfile() || opt().is_io_pfile() );
	// sanity check - only one bt_io_vapi_t is set in the bt_ezswarm_opt_t
	DBG_ASSERT( opt().is_io_sfile() != opt().is_io_pfile() );

	// if the bt_ezswarm_opt_t has IO_SFILE set
	if( opt().is_io_sfile() ){
		bt_io_sfile_t *	io_sfile;
		io_sfile	= nipmem_new bt_io_sfile_t();
		bt_err		= io_sfile->start(mfile());
		m_io_vapi	= io_sfile;
		if( bt_err.failed() )	return bt_err;
	}
	// if the bt_ezswarm_opt_t has IO_PFILE set
	if( opt().is_io_pfile() ){
		bt_io_pfile_t *	io_pfile;
		io_pfile	= nipmem_new bt_io_pfile_t();
		bt_err		= io_pfile->profile(profile().io_pfile()).start(mfile());
		m_io_vapi	= io_pfile;
		if( bt_err.failed() )	return bt_err;
	}
	// if bt_ezsession_t has io_cache_pool, do a bt_io_cache_t with m_io_vapi as subio_vapi
	if( ezsession()->io_cache_pool() ){
		bt_io_cache_t *	io_cache;
		io_cache	= nipmem_new bt_io_cache_t();
		bt_err		= io_cache->start(ezsession()->io_cache_pool(), m_io_vapi);		
		m_io_vapi	= io_cache;
		if( bt_err.failed() )	return bt_err;
	}
	// return noerror
	return bt_err_t::OK;
}

/** \brief Function used to update the listen_pview *DURING* bt_session_t run
 * 
 * - this function updates ONLY the 'sattelites' class
 *   - DO NOT touch bt_swarm_t as it is already updated by the bt_session_t updates
 */
void	bt_ezswarm_t::update_listen_pview(const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter NOT YET IMPLEMENTED new_listen_pview=" << new_listen_pview);
	// TODO check what are the stuff to update 
	// - it depends on the state
	// - in share, the peersrc kad/http and maybe utpex may need to be notified
	
	// NOTE: rather ugly hardcoded way to forward the new_listen_pview
	// - TODO better comment when brain comes back

#if 1
	// if the bt_ezswarm_t is not in_share(), do nothing
	if( !in_share() )	return;
	// forward the new_listen_pview to bt_peersrc_http_t and bt_peersrc_kad_t
	if( share()->peersrc_http() )	share()->peersrc_http()->update_listen_pview(new_listen_pview);
	if( share()->peersrc_kad() )	share()->peersrc_kad()->update_listen_pview(new_listen_pview);
#endif
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			state transition stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the new end_state
 * 
 * - it may trigger a nextiter_change_state if the cur_state().is_endless
 */
void	bt_ezswarm_t::graceful_change_state(const bt_ezswarm_state_t &new_state)throw()
{
	// log to debug
	KLOG_ERR("enter new_state=" << new_state);
	// set the variable
	m_end_state	= new_state;
	// if the cur_state is endless, trigger an nextiter_change_state
	if( cur_state().is_endless() )	nextiter_change_state(bt_ezswarm_state_t::NONE);
}

/** \brief goto a new bt_ezswarm_state_t in the event loop next iteration
 */
void	bt_ezswarm_t::nextiter_change_state(const bt_ezswarm_state_t &new_state)	throw()
{
	// queue a next_state_zerotiemr and pass the new_state as userptr
	next_state_zerotimer.append(this, (void *)new_state.get_value());
}

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - this is used by the nextiter_change_state to allow change state from the caller
 *   while avoiding some nested callback notification.
 */
bool	bt_ezswarm_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	bt_ezswarm_state_t ezswarm_state = (bt_ezswarm_state_t::errtype_enum)(int)(userptr);
	// log to debug
	KLOG_DBG("ezswarm_state=" << ezswarm_state);
	// actually change the state
	return change_state_now(ezswarm_state);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief goto a new bt_ezswarm_state_t NOW
 */
bool	bt_ezswarm_t::change_state_now(const bt_ezswarm_state_t &asked_state)	throw()
{
	bt_ezswarm_state_t	old_state	= cur_state();
	bt_ezswarm_event_t	ezswarm_event;
	bool			tokeep;
	// log to debug
	KLOG_ERR("enter cur_state=" << cur_state() << " asked_state=" << asked_state);
	// sanity check - the cur_state MUST NEVER be null
	DBG_ASSERT( !cur_state().is_null() );

	// if cur_state.is_share(), update the local swarm_resumedata
	// - supposedly to get the uptodate one just before leaving bt_ezswarm_share_t
	// - TODO this is kludgy and poorly coded
	// - what about the alloc/check and share i should update the swarm_resumedata too
	// - where to put this code
	if( cur_state().is_share() ){
		m_swarm_resumedata = ezswarm_share->bt_swarm()->get_current_resumedata();
	}
	
	// notify a leave_state_pre
	ezswarm_event	= bt_ezswarm_event_t::build_leave_state_pre(cur_state());
	tokeep		= notify_callback(ezswarm_event);
	if( !tokeep )	return false;	
	
	// leave the current state (the old one)
	cur_stateobj_dtor();

	// notify a leave_state_post
	ezswarm_event	= bt_ezswarm_event_t::build_leave_state_post(cur_state());
	tokeep		= notify_callback(ezswarm_event);
	if( !tokeep )	return false;	
	
	// sanity check - at this point,all state context MUST be NULL
	DBG_ASSERT( !ezswarm_alloc && !ezswarm_check && !ezswarm_share );
	DBG_ASSERT( !ezswarm_stopping && !ezswarm_stopped );

	// set the current state to the asked_state
	m_cur_state	= cpu_new_state(asked_state);

	// if the cur_state IS NOT NONE, notify a enter_state_pre
	ezswarm_event	= bt_ezswarm_event_t::build_enter_state_pre(cur_state());
	tokeep		= notify_callback(ezswarm_event);
	if( !tokeep )	return false;	
	
	// construct the stateobj for the current state (the new one)
	bt_err_t bt_err	= cur_stateobj_ctor();
	
	// if starting the stateobj failed, switch immediatly to bt_ezswarm_state_t::ERROR
	// - this can be caused only by a very serious error occured
	// - NOTE: it cause to have a ENTER_STATE_PRE for one state and a 
	//   ENTER_STATE_POST for the error state.
	if( bt_err.failed() ){
		// destroy the current stateobj
		cur_stateobj_dtor();
		// change the cur_state to ERROR with the proper reason
		std::string	reason = "starting "+ cur_state().to_string() + " failed due to " + bt_err.to_string();
		m_cur_state	= bt_ezswarm_state_t(bt_ezswarm_state_t::ERROR, reason);
		// start the bt_ezswarm_error_t
		DBG_ASSERT( !ezswarm_error );
		ezswarm_error	= nipmem_new bt_ezswarm_error_t();
		bt_err		= ezswarm_error->start(this);
		DBG_ASSERT( bt_err.succeed() );	// this is ok as bt_ezswarm_error_t never fails
	}

	// notify a enter_state_post
	ezswarm_event	= bt_ezswarm_event_t::build_enter_state_post(cur_state());
	tokeep		= notify_callback(ezswarm_event);
	if( !tokeep )	return false;
	
	// reset the end_state if it has been entered
	// - thus one the end_state is completed, bt_ezswarm_t automatically
	//   goes back to bt_ezswarm_state_t::STOPPED
	if( end_state() == cur_state() )	m_end_state = bt_ezswarm_state_t();

	// if cur_state.is_share(), update the swarm_resumedata
	// - TODO this is kludgy and poorly coded
	// - what about the alloc/check and share i should update the swarm_resumedata too
	// - where to put this code
	if( cur_state().is_share() ){
		m_swarm_resumedata = ezswarm_share->bt_swarm()->get_current_resumedata();
	}

	// return tokeep
	return true;
}

/** \brief Compute the new bt_ezswarm_state_t 
 * 
 * - TODO poor nameing of the function
 * - TODO very half backed... currrently fit the need in neoip-get and neoip-btcli
 *   but no good
 */
bt_ezswarm_state_t bt_ezswarm_t::cpu_new_state(const bt_ezswarm_state_t &asked_state)	throw()
{
	// if there a asked_state, return this one
	if( !asked_state.is_null() )	return asked_state;
	// if there are no end_state, goto bt_ezswarm_state_t::STOPPED
	if( end_state().is_null() )	return bt_ezswarm_state_t::STOPPED;

	// this is the routing table for the graceful_change_state
	// TODO this is coded like shit to say the least
	// - this is ugly code
	// - a lot of case are not handled
	if( cur_state().is_alloc() ){
		if( end_state().is_share() )	return bt_ezswarm_state_t::SHARE;
	}else if( cur_state().is_check() ){
		if( end_state().is_share() )	return bt_ezswarm_state_t::SHARE;
	}else if( cur_state().is_share() ){
		if( end_state().is_stopped() )	return bt_ezswarm_state_t::STOPPING;
	}else if( cur_state().is_stopping() ){
		// NOTE: dont use the constant in order to keep the reason of bt_ezswarm_state_t
		if( end_state().is_stopped() )	return end_state();
	}else if( cur_state().is_stopped() ){
		if( end_state().is_share() ){
			if( swarm_resumedata().mfile_allocated() )
				return bt_ezswarm_state_t::CHECK;
			else{
#if 1
				// TODO damn this is ugly
				// - if the opt().is_io_sfile(), do a bt_alloc_t
				// - else go directory to SHARE
				// - the issue between alloc and bt_io_vapi_t is in many other
				//   place too.. :(
				if( opt().is_io_sfile() )	return bt_ezswarm_state_t::ALLOC;
				else if( opt().is_io_pfile() )	return bt_ezswarm_state_t::SHARE;
				else				DBG_ASSERT( 0 );
#else
				return bt_ezswarm_state_t::ALLOC;
#endif
			}
		}
	}else if( cur_state().is_error() ){
		// TODO so i never leave error ? :)
	}
	
	// if this point is reached, an error in the state transition occured
	std::string	reason	= "BUG: state transition impossible from " 
				+ cur_state().to_string() + " to " + end_state().to_string();
	return bt_ezswarm_state_t(bt_ezswarm_state_t::ERROR, reason);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			stateobj ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief construct the stateobj for the cur_state()
 * 
 * - TODO this function is ugly, clean it up
 */
bt_err_t	bt_ezswarm_t::cur_stateobj_ctor()				throw()
{
	bt_err_t	bt_err;
	// act depending on the cur_state
	switch(cur_state().get_value()){
	case bt_ezswarm_state_t::ALLOC:	DBG_ASSERT( !ezswarm_alloc );
					ezswarm_alloc	= nipmem_new bt_ezswarm_alloc_t();
					bt_err		= ezswarm_alloc->start(this);
					break;
	case bt_ezswarm_state_t::CHECK:	DBG_ASSERT( !ezswarm_check );
					ezswarm_check	= nipmem_new bt_ezswarm_check_t();
					bt_err		= ezswarm_check->start(this);
					break;
	case bt_ezswarm_state_t::SHARE:	DBG_ASSERT( !ezswarm_share );
					ezswarm_share	= nipmem_new bt_ezswarm_share_t();
					bt_err		= ezswarm_share->start(this);			
					break;
	case bt_ezswarm_state_t::STOPPING:DBG_ASSERT( !ezswarm_stopping );
					ezswarm_stopping= nipmem_new bt_ezswarm_stopping_t();
					bt_err		= ezswarm_stopping->start(this);
					break;
	case bt_ezswarm_state_t::STOPPED:DBG_ASSERT( !ezswarm_stopped );
					ezswarm_stopped	= nipmem_new bt_ezswarm_stopped_t();
					bt_err		= ezswarm_stopped->start(this);
					break;
	case bt_ezswarm_state_t::ERROR:	DBG_ASSERT( !ezswarm_error );
					ezswarm_error	= nipmem_new bt_ezswarm_error_t();
					bt_err		= ezswarm_error->start(this);
					break;
	default:			DBG_ASSERT(0);
	}
	return bt_err;
}

/** \brief Destroy the stateobj for the cur_state();
 */
void	bt_ezswarm_t::cur_stateobj_dtor()					throw()
{
	// log to debug
	KLOG_DBG("enter cur_state=" << cur_state() );
	// act depending on the cur_state
	switch(cur_state().get_value()){
	case bt_ezswarm_state_t::ALLOC:		DBG_ASSERT( ezswarm_alloc );
						nipmem_zdelete ezswarm_alloc;	break;
	case bt_ezswarm_state_t::CHECK:		DBG_ASSERT( ezswarm_check );
						nipmem_zdelete ezswarm_check;	break;
	case bt_ezswarm_state_t::SHARE:		DBG_ASSERT( ezswarm_share );
						nipmem_zdelete ezswarm_share;	break;
	case bt_ezswarm_state_t::STOPPING:	DBG_ASSERT( ezswarm_stopping );
						nipmem_zdelete ezswarm_stopping;break;
	case bt_ezswarm_state_t::STOPPED:	DBG_ASSERT( ezswarm_stopped );
						nipmem_zdelete ezswarm_stopped;	break;
	case bt_ezswarm_state_t::ERROR:		DBG_ASSERT( ezswarm_error );
						nipmem_zdelete ezswarm_error;	break;
	case bt_ezswarm_state_t::NONE:		break;
	default:				DBG_ASSERT(0);
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			notification from the stateobj
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called by the stateobj to notify a successfull completion
 * 
 * - the notifier stateobj WILL ALWAYS be deleted by this function as it means it
 *   has been successfully completed
 * 
 * @return false all the time for tokeep convenience
 */
bool	bt_ezswarm_t::stateobj_notify_success()				throw()
{
	// go immediatly in the next state
	change_state_now(bt_ezswarm_state_t());
	// return false all the time for tokeep convenience
	// - so the notifier stateobj MUST have been deleted by the change_state_now()
	return false;
}

/** \brief called by the stateobj to notify a failled completion
 * 
 * - the notifier stateobj WILL ALWAYS be deleted by this function as it means it
 *   has been completed
 * 
 * @return false all the time for tokeep convenience
 */
bool	bt_ezswarm_t::stateobj_notify_failure(const bt_err_t &bt_err)	throw()
{
	// go immediatly in bt_ezswarm_state_t::ERROR with the reason properly set
	change_state_now(bt_ezswarm_state_t(bt_ezswarm_state_t::ERROR, bt_err.to_string()));
	// return false all the time for tokeep convenience
	// - so the notifier stateobj MUST have been deleted by the change_state_now()
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_ezswarm_event_t
 */
bool bt_ezswarm_t::notify_callback(const bt_ezswarm_event_t &ezevent)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_ezswarm_cb(userptr, *this, ezevent);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





