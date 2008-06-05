/*! \file
    \brief Implementation of the ndiag_watch_t

\par Brief Description
\ref ndiag_watch_t centralizes information about the network position.
those informations may come from different sources. If those information
changes, it notifies all the callback registered to it via event_hook_t
- additionnaly it store a ndiag_cacheport_t to increase the persistence
  of the public view of address. aka use the same port public view when 
  possible.

\par Note about each event hook
- ip_addr_t pview
  - the IPADDR_PVIEW_CHANGED event is notified when the value of 
    ndiag_watch_t::ipaddr_pview() change 
  - the ip_addr_t pview may be provided by:
    - kad thru the kad_ping_t echoing the public address
    - ntudp thru the ntudp_ndiag_res_t
    - upnp thru the upnp_call_extipaddr_t
- upnp_isavail
  - the UPNPDISC_CHANGED event is notified when the value of 
    ndiag_watch_t::upnp_isavail() change

*/

/* system include */
/* local include */
#include "neoip_ndiag_watch.hpp"
#include "neoip_ndiag_err.hpp"
#include "neoip_ndiag_cacheport.hpp"
#include "neoip_ndiag_cacheport_helper.hpp"
#include "neoip_event_hook.hpp"
#include "neoip_upnp_watch.hpp"
#include "neoip_upnp_watch_event.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief contructor of the ndiag_watch_t
 */
ndiag_watch_t::ndiag_watch_t() throw()
{
	// zero some variables
	event_hook	= NULL;
	m_upnp_watch	= NULL;
	m_cacheport	= NULL;
	// set the default values
	m_upnp_isavail	= false;	
}

/** \brief destructor of the ndiag_watch_t
 */
ndiag_watch_t::~ndiag_watch_t() throw()
{
	// save the ndiag_cacheport_t file if possible
	if( m_cacheport ){
		file_path_t file_path	= ndiag_cacheport_helper_t::default_path();
		ndiag_err_t  ndiag_err	= ndiag_cacheport_helper_t::to_file(*ndiag_cacheport(), file_path);
		if( ndiag_err.failed() )	KLOG_ERR("cant save " << file_path << " due to " << ndiag_err);
	}
	// delete the event_hook if needed
	nipmem_zdelete	m_cacheport;
	// delete the event_hook if needed
	nipmem_zdelete	event_hook;
	// delete the upnp_watch_t if needed
	nipmem_zdelete	m_upnp_watch;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ndiag_watch_t &	ndiag_watch_t::set_profile(const ndiag_watch_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check().succeed() );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief start the session
 * 
 * @return false if no error occured, true otherwise
 */
ndiag_err_t ndiag_watch_t::start()	throw()
{	
	// init the event hook
	event_hook	= nipmem_new event_hook_t(HOOK_MAX);
	// create the ndiag_cacheport_t
	m_cacheport	= nipmem_new ndiag_cacheport_t();
	// try to load the ndiag_cacheport_t file
	*m_cacheport	= ndiag_cacheport_helper_t::from_file(ndiag_cacheport_helper_t::default_path());
	// create a upnp_watch_listener_t
	if( profile.enable_upnp_watch() ){
		upnp_err_t	upnp_err;
		m_upnp_watch	= nipmem_new upnp_watch_t();
		upnp_err	= upnp_watch()->start(this, NULL);
		if( upnp_err.failed() )	return ndiag_err_from_upnp(upnp_err);
	}
	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/ndiag_watch_" + OSTREAMSTR(this));

	// return no error
	return ndiag_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_watch_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref upnp_watch_t to notify an event
 */
bool 	ndiag_watch_t::neoip_upnp_watch_cb(void *cb_userptr, upnp_watch_t &cb_upnp_watch
						, const upnp_watch_event_t &watch_event)	throw()
{
	// log to debug
	KLOG_ERR("enter upnp_watch_event=" << watch_event);

	// if upnp_watch()->extipaddr() is not public, treat it as a UPNPDISC_CHANGE upnp_unavail()
	// - NOTE: this happen in case of nested NAT box with the inner one supporting upnp
	// - POSSIBLE IMPROVEMENT: chain the upnp call (if all nat box support upnp)
	//   - have a static configuration of the ip_addr_t of all the upnp box
	//     - or even having a guessing mechanim based on the heuristic that the 
	//       upnp router have the first ip address of the lan subnet most of the time
	//     - e.g. if local ip address is 192.168.7.19  the upnp router is 192.168.7.1
	//       or if local ipaddress is 192.168.1.3, the upnp router is 192.168.1.1
	//       or if local ipaddress is 10.0.0.2, the upnp router is 10.0.0.1
	//   - query them via unicast udp (instead of the default in unicast)
	if( !upnp_watch()->extipaddr().is_null() && !upnp_watch()->extipaddr().is_public() ){
		// schedule a event_zerotimer if needed
		if( upnp_isavail() != false )
			event_zerotimer.append(this, (void *)UPNPDISC_CHANGED);
		// update the local value
		m_upnp_isavail	= false;
		// return tokeep
		return true;
	}

	// handle each possible events from its type
	switch( watch_event.get_value() ){
	case upnp_watch_event_t::UPNPDISC_CHANGED:{
		// if upnp_watch()->upnp_isavail() is different from local one, schedule a notification 
		// - notify UPNPDISC_CHANGED thru a zerotimer_t to avoid nested callback
		if( upnp_isavail() != upnp_watch()->upnp_isavail() )
			event_zerotimer.append(this, (void *)UPNPDISC_CHANGED);
		// update the local value
		m_upnp_isavail	= upnp_watch()->upnp_isavail();
		break;}
	case upnp_watch_event_t::EXTIPADDR_CHANGED:
		// handle it by the usual notification function
		notify_ipaddr_pview( upnp_watch()->extipaddr() );
		break;
	default:	DBG_ASSERT( 0 );
	}
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function used 
 */
void	ndiag_watch_t::notify_ipaddr_pview(const ip_addr_t &new_ipaddr_pview)	throw()
{
	// sanity check - the new_ipaddr_pview MUST be public or null
	DBG_ASSERT( new_ipaddr_pview.is_public() || new_ipaddr_pview.is_null() );
	// if the newly notified value, is the same as before, do nothing and return now
	if( new_ipaddr_pview == ipaddr_pview() )	return;
	// copy the new value into the current one
	m_ipaddr_pview	= new_ipaddr_pview;
	// schedule a notification of IPADDR_PVIEW_CHANGED thru a zerotimer_t
	// - the zerotimer_t allow to avoid nested callback
	event_zerotimer.append(this, (void *)IPADDR_PVIEW_CHANGED);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			hook event
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief hook an event in \ref ndiag_watch_t
 */
void ndiag_watch_t::event_hook_append(int level_no, event_hook_cb_t *callback, void *userptr) throw()
{
	event_hook->append(level_no, callback, userptr);
}

/** \brief unhook an event in \ref ndiag_watch_t
 */
void ndiag_watch_t::event_hook_remove(int level_no, event_hook_cb_t *callback, void *userptr) throw()
{
	event_hook->remove(level_no, callback, userptr);
}

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	ndiag_watch_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	int level_no	= (int)userptr;
	// log to debug
	KLOG_DBG("enter");
	// notify the event
	event_hook->notify(level_no);
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END


