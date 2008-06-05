/*! \file
    \brief Declaration of the upnp_watch_t

\par Brief Description
upnp_watch_t watchs the various upnp parts
- it detects the presence/absence of upnp routers 
  - and notify changes via event
- if upnp is available, it determine the public view of the ip address
  - and notify changes via event
- if upnp is available, it keeps the upnp discovery result in upnp_disc_res_t
  - which is used for other type of call that the caller may want to do
- it allows to bindport in the upnp router in a simple manner
  - the delport in the upnp_router is handled gracefully by upnp_watch_t
    letting the caller ignoring it.
- as many/most upnp router doesnt support a lease time while doing addport, 
  there is a layer, called upnp_portcleaner_t, which piggy back the lease
  expiration in upnp_router.
  - this is very important in case of apps because abrutely unreachable
    in order to leave permanent crubs in the upnp router

\par About lib_session_exit_t
- as upnp_watch_t allocates rescources in remote locations, aka adding port
  forwarding in upnp router, it needs to deallocate them on exit
- this is handled here in the lib_session_exit_t 

\par about call_delport_db
- in upnp_bindport_t dtor, a upnp_call_delport_t MUST be issued to remove the 
  binding in the upnp router.
- it is stored in upnp_watch_t as the original upnp_bindport_t is deleted :)
- lib_session_exit_t waits for call_delport_db to be empty before exiting

\par Brief Implementation note
- implement a upnp subset to do the portmapping only
  - based on http://zbowling.com/projects/upnp/
  - interesting link 
    - http://gaim.sourceforge.net/summerofcode/adam/ wich
      does similar thing in gaim2.0/libgaim/upnp.[ch]
    - ktorrent does similar thing in ktorrent/plugin/upnp
  - somebody who did a miniupnp IGD
    - http://miniupnp.free.fr/
    - this one seems rather good

*/

/* system include */
/* local include */
#include "neoip_upnp_watch.hpp"
#include "neoip_upnp_watch_event.hpp"
#include "neoip_upnp_disc.hpp"
#include "neoip_upnp_call.hpp"
#include "neoip_upnp_call_delport.hpp"
#include "neoip_upnp_call_extipaddr.hpp"
#include "neoip_upnp_portcleaner.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_nudp.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_res.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_watch_t::upnp_watch_t()		throw()
{
	// zero some fields
	upnp_disc	= NULL;
	call_extipaddr	= NULL;
	upnp_portcleaner= NULL;
	lib_session_exit= NULL;
}

/** \brief Desstructor
 */
upnp_watch_t::~upnp_watch_t()		throw()
{
	// sanity check - bindport_db MUST empty at this point
	DBG_ASSERT( bindport_db.empty() );
	// delete the lib_session_exit_t if needed
	nipmem_zdelete	lib_session_exit;
	// delete the upnp_portcleaner_t if needed
	nipmem_zdelete	upnp_portcleaner;
	// delete the upnp_call_extipaddr_t if needed
	nipmem_zdelete	call_extipaddr;
	// delete the upnp_disc_t if needed
	nipmem_zdelete	upnp_disc;
	// if call_delport_db is NOT empty, this will leave allocated port in upnp router
	// - so log the event
	if( !call_delport_db.empty() ){
		KLOG_ERR("Deleted upnp_watch_t with " << call_delport_db.size()	<< " pending upnp_call_delport_t. "
				<< "This will leave dangling allocation in upnp_router");
	}
	// close all pending upnp_call_delport_t
	// - NOTE: upnp_call_delport_t is a special case which can be pending
	//   due to its use in upnp_bindport_t destructor
	while( !call_delport_db.empty() ){
		// get the front element from the database
		upnp_call_delport_t *	call_delport	= call_delport_db.front();
		call_delport_db.pop_front();
		// delete the upnp_call_delport_t
		nipmem_delete call_delport;
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
upnp_watch_t &upnp_watch_t::set_profile(const upnp_watch_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == upnp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief start the actions
 */
upnp_err_t	upnp_watch_t::start(upnp_watch_cb_t *callback, void * userptr)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;

	// init the upnp_disc_t timeout stuff
	disc_delaygen	= delaygen_t(profile.disc_delaygen_arg());
	disc_timeout.start(disc_delaygen.current(), this, NULL);

	// start the lib_session_exit_t
	lib_session_exit	= nipmem_new lib_session_exit_t();
	lib_session_exit->start(lib_session_get(), lib_session_exit_t::EXIT_ORDER_UPNP_WATCH
						, this, NULL);

	// return no error
	return upnp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_watch_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	if( &cb_timeout == &disc_timeout )	return disc_timeout_cb(userptr, cb_timeout);
	if( &cb_timeout == &extipaddr_timeout )	return extipaddr_timeout_cb(userptr, cb_timeout);
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_disc_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_watch_t::disc_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// sanity check - upnp_disc MUST be null while disc_timeout is running
	DBG_ASSERT( upnp_disc == NULL );

	// notify the expiration of the timeout to the delaygen
	disc_delaygen.notify_expiration();
	// if the delaygen is is_timedout, dont relaunch the timeout
	if( disc_delaygen.is_timedout() ){
		disc_timeout.stop();
		return true;
	}
	
	// launch a upnp_disc_t
	// - TODO should handle the switch between WANIPConnection and WANPPPConnection	
	//   - sure but i dunno how to organize it
	//   - test WANIPConnection first
	//   - if it fails, switch to WANPPPConnection and restart immediatly
	//   - if it succeed, keep the state and start the retry timer
	//   - on retry timer expiration, restart the same service which succeed last time
	//     - if the retry failed, switch to the other service immedialty
	//     - if the other server failed too, start the retry timer
	// - the actual algo is still unclear
	upnp_err_t	upnp_err;
	upnp_disc	= nipmem_new upnp_disc_t();
	upnp_err	= upnp_disc->set_profile(profile.disc())
					.start("WANIPConnection", this, NULL);
	DBG_ASSERT( upnp_err.succeed() );	// TODO poor error management

	// stop the disc_timeout while upnp_disc_t is pending - will be relaunch after its completion
	disc_timeout.stop();
	
	// return a 'tokeep'
	return true;
}

/** \brief callback notified by \ref upnp_disc_t on completio\n
 */
bool	upnp_watch_t::neoip_upnp_disc_cb(void *cb_userptr, upnp_disc_t &cb_disc
				, const upnp_err_t &cb_upnp_err
				, const upnp_disc_res_t &notified_disc_res)	throw()
{
	upnp_err_t	upnp_err	= cb_upnp_err;
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err << " disc_res=" << notified_disc_res);

	// sanity check - if upnp_err.succeed() then disc_res MUST be non-null
	if( upnp_err.succeed() )	DBG_ASSERT( !notified_disc_res.is_null() );
	// sanity check - if upnp_err.failed() then disc_res MUST be null
	if( upnp_err.failed() )		DBG_ASSERT(  notified_disc_res.is_null() );
	// sanity check - the disc_timeout MUST NOT be running during upnp_disc_t execution
	DBG_ASSERT( !disc_timeout.is_running() );

	// determine if the upnp_disc_res_t changed or not  
	bool	res_changed;
	if( notified_disc_res == current_disc_res() )	res_changed	= false;
	else						res_changed	= true;


#if 1	// TODO to remove - only to debug assert failing in some case of UPNP_CHANGED
	upnp_disc_res_t	old_disc_res	= m_current_disc_res;
#endif	
	// copy the new upnp_disc_res_t
	m_current_disc_res	= notified_disc_res;

	// delete the upnp_disc_t
	nipmem_zdelete upnp_disc;

	// if the upnp_disc_res_t changed, notify the caller
	if( res_changed ){
		bool	tokeep	= notify_callback(upnp_watch_event_t::build_upnpdisc_changed());
		if( !tokeep )	return false;
	}


	// relaunch the disc_timeout for the next upnp_disc_t
	disc_timeout.start(disc_delaygen.pre_inc(), this, NULL);
	
#if 0	// TODO not sure about this one
	// if upnp_unavail() now, reset the m_extipaddr
	// - TODO should i notify this event ?
	if( upnp_unavail() )	m_extipaddr	= ip_addr_t();
#endif

	// if the res_changed and now upnp_isavail(), start the extipaddr_timeout and portcleaner_timeout
	if( res_changed && upnp_isavail() ){
		// sanity check: the extipaddr_timeout MUST NOT be running
		// - TODO this assert fails from time to time... fix this
		//   - i dont even see how it can be right to have this as this 'if' 
		//     may be entered at anytime
		//   - instead, would be way better to delete call_extipaddr if needed
#if 1	// TODO to remove - only to debug assert failing in some case of UPNP_CHANGED
	// - this is due to getporttest sometime being 0 on a router in which is it 1
	//   - so for unknown reason sometime this is diagnosed as
	//   - is it due to a timeout ? and using the default value as 0 ?
	//   - apparently getportendian_test report an error on timeout... so this
	//     cant be it...
	//   - how it is possible to get a 0 while the upnp router is a 1 ?
	//   - what about a race ?
	//   - it uses a constant "upnp revendian testing" tag to identify the addport
	//     SO if 2 getportend_test are done simultaneously, this MAY result in a conflict
	//     - likely rare, but trivial to fix. 
	//     - TODO just put a nonce in the tag
	// - res_changed ISNOT upnp is avail or not, the result may change even
	//   between isavail and isavail
	//   - as shown in changing from getporttest false to getposttest true
	//   - SO DO NOT assume that it is 
		if( !(!extipaddr_timeout.is_running() && !call_extipaddr) ){
			KLOG_ERR("new disc_rec=" << m_current_disc_res);
			KLOG_ERR("old disc_rec=" << old_disc_res);
		}
		DBG_ASSERT( !extipaddr_timeout.is_running() );
		DBG_ASSERT( !call_extipaddr );
#else
		DBG_ASSERT( !extipaddr_timeout.is_running() && !call_extipaddr );
#endif
		// init the extipaddr_timeout
		extipaddr_delaygen	= delaygen_t(profile.extipaddr_delaygen_arg());
		extipaddr_timeout.start(extipaddr_delaygen.current(), this, NULL);
		// init the portcleaner_timeout - IIF profile has portcleaner_enabled
		if( get_profile().portcleaner_enabled() ){
			// - sanity check: upnp_portcleaner_t MUST NOT be running
			DBG_ASSERT( !upnp_portcleaner );
			upnp_portcleaner= nipmem_new upnp_portcleaner_t();
			upnp_err	= upnp_portcleaner->start(this);
			DBG_ASSERT( upnp_err.succeed() );
		}
	}

	// if the res_changed and now upnp_unavail(), stop the extipaddr_timeout and portcleaner_timeout
	if( res_changed && upnp_unavail() ){
		// stop the extipaddr_timeout
		// - sanity check: the extipaddr_timeout MUST be running
		DBG_ASSERT( extipaddr_timeout.is_running() || call_extipaddr );
		extipaddr_timeout.stop();
		nipmem_zdelete	call_extipaddr;
		// stop the portcleaner_timeout - IIF profile has portcleaner_enabled
		if( get_profile().portcleaner_enabled() ){
			// - sanity check: upnp_portcleaner_t  MUST be running
			DBG_ASSERT( upnp_portcleaner );
			// delete upnp_portcleaner
			nipmem_zdelete	upnp_portcleaner;
		}
	}

	// return 'dontkeep' as the upnp_disc_t has been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_call_extipaddr_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_watch_t::extipaddr_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// sanity check - upnp_disc MUST be null while disc_timeout is running
	DBG_ASSERT( call_extipaddr == NULL );

	// notify the expiration of the timeout to the delaygen
	extipaddr_delaygen.notify_expiration();
	// if the delaygen is is_timedout, dont relaunch the timeout
	if( extipaddr_delaygen.is_timedout() ){
		extipaddr_timeout.stop();
		return true;
	}
	
	// launch a upnp_call_extipaddr_t
	upnp_err_t	upnp_err;
	call_extipaddr	= nipmem_new upnp_call_extipaddr_t();
	upnp_err	= call_extipaddr->set_profile(profile.call()).start(current_disc_res(),this,NULL);
	DBG_ASSERT( upnp_err.succeed() );	// TODO poor error management

	// stop the extipaddr_timeout while upnp_call_extipaddr_t is pending
	// - will be relaunch after its completion
	extipaddr_timeout.stop();
	
	// return a 'tokeep'
	return true;
}

/** \brief callback notified by \ref upnp_call_extipaddr_t when completed
 */
bool	upnp_watch_t::neoip_upnp_call_extipaddr_cb(void *cb_userptr, upnp_call_extipaddr_t &cb_call_extipaddr
			, const upnp_err_t &upnp_err, const ip_addr_t &notified_extipaddr)	throw()
{
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err << " extipaddr=" << notified_extipaddr);

	// sanity check - if upnp_err.succeed() then notified_extipaddr MUST be non-null
	if( upnp_err.succeed() )	DBG_ASSERT( !notified_extipaddr.is_null() );
	// sanity check - if upnp_err.failed() then notified_extipaddr MUST be null
	if( upnp_err.failed() )		DBG_ASSERT(  notified_extipaddr.is_null() );
	// sanity check - the extipaddr_timeout MUST NOT be running during upnp_call_extipaddr_t execution
	DBG_ASSERT( !extipaddr_timeout.is_running() );

	// determine if the upnp_disc_res_t changed or not  
	bool	res_changed;
	if( notified_extipaddr == extipaddr() )	res_changed	= false;
	else					res_changed	= true;
	
	// copy the new ip_addr_t
	m_extipaddr	= notified_extipaddr;

	// delete the upnp_call_extipaddr_t
	nipmem_zdelete	call_extipaddr;

	// if the extipaddr changed, notify the caller
	if( res_changed ){
		bool	tokeep	= notify_callback( upnp_watch_event_t::build_extipaddr_changed() );
		if( !tokeep )	return false;
	}

	// relaunch the extipaddr_timeout for the next upnp_extipaddr_t
	extipaddr_timeout.start(extipaddr_delaygen.pre_inc(), this, NULL);
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			call_delport
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch a upnp_call_delport_t
 * 
 * - this is a special case used by upnp_bindport_t because the upnp_call_delport_t
 *   is launched in the upnp_bindport_t dtor.
 *   - so the upnp_delport_t is linked in the upnp_watch_t
 *   - the callback is returned here too
 * - the disc_res_4del is used to launcht the upnp_call_delport_t
 *   - this is important due to a kludge in upnp_getportendian_test_t
 *   - at this point, upnp_watch_t has no upnp_disc_res_t so the upnp_getportendian_test_t
 *     provides one
 */
upnp_err_t	upnp_watch_t::launch_delport(uint16_t port_pview, const upnp_sockfam_t &sock_fam
				, const upnp_disc_res_t &disc_res_4del)	throw()
{
	upnp_call_delport_t *	call_delport;
	upnp_err_t		upnp_err;
	// log to debug
	KLOG_DBG("enter port_pview="<< port_pview << " sock_fam=" << sock_fam);
	// sanity check - upnp_disc_res_t MUST NOT be null
	DBG_ASSERT( !disc_res_4del.is_null() );
	// create a upnp_call_delport_t
	call_delport	= nipmem_new upnp_call_delport_t();
	upnp_err	= call_delport->set_profile(get_profile().call())
				.start(disc_res_4del, port_pview, sock_fam, this, NULL);
	if( upnp_err.failed() ){
		// delete the object
		nipmem_zdelete	call_delport;
		// return the error
		return upnp_err;
	}
	// link it to the call_delport_db
	call_delport_db.push_back(call_delport);
	// return no error
	return upnp_err_t::OK;
}

/** \brief callback notified by \ref upnp_call_getport_t when completed
 */
bool 	upnp_watch_t::neoip_upnp_call_delport_cb(void *cb_userptr, upnp_call_delport_t &cb_call_delport
						, const upnp_err_t &upnp_err)	throw()
{
	upnp_call_delport_t *	call_delport	= &cb_call_delport;
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err << " call_delport_db.size()="<< call_delport_db.size());
	
	// remove the upnp_call_delport_t from call_delport_db
	call_delport_db.remove( call_delport );
	// delete the upnp_call_delport_t
	nipmem_zdelete	call_delport;

	// if  call_delport_db is now empty and lib_session_exit is_notified(), delete it 
	if( lib_session_exit->is_notified() && call_delport_db.empty() )
		nipmem_zdelete	lib_session_exit;

	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			lib_session_exit_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref lib_session_exit_t when to notify an event
 */
bool	upnp_watch_t::neoip_lib_session_exit_cb(void *cb_userptr, lib_session_exit_t &session_exit) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// sanity check - bindport_db MUST be empty here
	// - aka they MUST have been deleted before reaching this lib_session_exit_t
	//   likely with other lib_session_exit_t with a lowest exit_order
	DBG_ASSERT( bindport_db.empty() );

	// if call_delport_db is NOT empty, wait until they are all completed
	if( !call_delport_db.empty() )	return true;
	
	// else delete the exit procedure
	nipmem_zdelete	lib_session_exit;
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_watch_t::notify_callback(const upnp_watch_event_t &watch_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the discer
	bool tokeep = callback->neoip_upnp_watch_cb(userptr, *this, watch_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


