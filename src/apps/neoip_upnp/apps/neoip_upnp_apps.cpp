/*! \file
    \brief Definition of the \ref upnp_apps_t

\par Brief Description
upnp_apps_t is just a small cmdline tool in order to see how the libneoip_upnp
library reacts and display the results.
- its primary goal is for testing/debugging

*/

/* system include */
/* local include */
#include "neoip_upnp_apps.hpp"
#include "neoip_ndiag_watch.hpp"
#include "neoip_upnp_watch.hpp"
#include "neoip_upnp_call_statusinfo.hpp"
#include "neoip_upnp_scanallport.hpp"
#include "neoip_clineopt_arr.hpp"

#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_apps_t::upnp_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	call_statusinfo	= NULL;
	upnp_scanallport= NULL;
}

/** \brief Destructor
 */
upnp_apps_t::~upnp_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete upnp_call_statusinfo_t if needed
	nipmem_zdelete	call_statusinfo;
	// delete upnp_scanallport_t if needed
	nipmem_zdelete	upnp_scanallport;
	
	// unhook the ndiag_watch_t events if needed
	// - TODO will fails if ::start() has not been called
	ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
	ndiag_watch->event_hook_remove(ndiag_watch_t::UPNPDISC_CHANGED, this, NULL);
	ndiag_watch->event_hook_remove(ndiag_watch_t::IPADDR_PVIEW_CHANGED, this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
upnp_err_t	upnp_apps_t::start()						throw()
{
	ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
	// log to debug
	KLOG_WARN("enter");

	// dohook the ndiag_watch_t event
	ndiag_watch->event_hook_append(ndiag_watch_t::UPNPDISC_CHANGED		, this, NULL);
	ndiag_watch->event_hook_append(ndiag_watch_t::IPADDR_PVIEW_CHANGED	, this, NULL);
	
	// do the initial display
	disp_initial();
	
	// return no error
	return upnp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Display the current status when being in bt_ezswarm_state_t::SHARE
 */
void	upnp_apps_t::disp_initial()			throw()
{
	// start the display
	KLOG_STDOUT("Starting watching for upnp router...\n");
}

/** \brief Display the upnp_statusinfo_t result in case of success
 * 
 * - WARNING: it MUST be called ONLY when upnp_apps_t::upnp_statusinfo is not NULL
 */
void	upnp_apps_t::disp_statusinfo_success()			throw()
{
	// sanity check - upnp_statusinfo MUST NOT be NULL
	DBG_ASSERT( call_statusinfo );
	// start the display
	KLOG_STDOUT("Current upnp Status info:\n");
	KLOG_STDOUT("\tcnx_status\t: "	<< call_statusinfo->cnx_status()	<< "\n");
	KLOG_STDOUT("\tlastcnx_err\t: "	<< call_statusinfo->lastcnx_err() 	<< "\n");
	KLOG_STDOUT("\tuptime_delay\t: "<< call_statusinfo->uptime_delay()	<< "\n");
}

/** \brief Display the upnp_statusinfo_t result in case of failure
 */
void	upnp_apps_t::disp_statusinfo_failure(const upnp_err_t &upnp_err)	throw()
{
	// start the display
	KLOG_STDOUT("Current upnp Status info:\n");
	KLOG_STDOUT("\tUnable to obtain it due to " << upnp_err << "\n" );
}

/** \brief Display the upnp_scanallport_t result in case of success
 */
void	upnp_apps_t::disp_scanallport_success(const upnp_portdesc_arr_t &portdesc_arr)	throw()
{
	// start the display
	KLOG_STDOUT("Currently upnp dynamically allocated ports: " << portdesc_arr.size()
							<< " ports\n");
	for(size_t i = 0; i < portdesc_arr.size(); i++ ){
		const upnp_portdesc_t &	portdesc	= portdesc_arr[i];
		KLOG_STDOUT( i << ": " << portdesc.to_human_str() << "\n");
	}
}

/** \brief Display the upnp_scanallport_t result in case of failure
 */
void	upnp_apps_t::disp_scanallport_failure(const upnp_err_t &upnp_err)	throw()
{
	// start the display
	KLOG_STDOUT("Current upnp dynamically allocated ports:\n");
	KLOG_STDOUT("\tUnable to obtain them due to " << upnp_err << "\n" );
}


/** \brief Display stuff when upnp become available
 */
void	upnp_apps_t::disp_upnp_isavail()	throw()
{
	ndiag_watch_t *		ndiag_watch	= ndiag_watch_get();
	upnp_watch_t *		upnp_watch	= ndiag_watch->upnp_watch();
	const upnp_disc_res_t &	disc_res	= upnp_watch->current_disc_res();
	// start the display
	KLOG_STDOUT("UPNP router has been found:\n");
	KLOG_STDOUT(disc_res.to_human_str() << "\n");
}

/** \brief Display stuff when upnp become unavailable
 */
void	upnp_apps_t::disp_upnp_unavail()	throw()
{
	// start the display
	KLOG_STDOUT("UPNP router is now unavailable.\n");
}

/** \brief Display stuff when ipaddr_pview_changed
 */
void	upnp_apps_t::disp_ipaddr_pview_changed()	throw()
{
	ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
	ip_addr_t	ipaddr_pview	= ndiag_watch->ipaddr_pview();
	// start the display
	if( ipaddr_pview.is_null() )	KLOG_STDOUT("External IP address is now unknown.\n");
	else				KLOG_STDOUT("External IP address is currently " << ipaddr_pview << "\n");	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ndiag_watch_t event_hook_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 */
bool	upnp_apps_t::neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
							, int hook_level)	throw()
{
	ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
	// log to debug
	KLOG_ERR("enter");
	
	// handle the ndiag_watch event_hook depending on the hook_level
	switch( hook_level ){
	case ndiag_watch_t::UPNPDISC_CHANGED:
			KLOG_ERR("upnp_isavail_changed. current=" << ndiag_watch->upnp_isavail());
			// sanity check - if upnp_isavail, upnp_watch_t MUST NOT be NULL
			if( ndiag_watch->upnp_isavail() )	DBG_ASSERT( ndiag_watch->upnp_watch() );

			// display the result depending of upnp_isavail/upnp_unavail
			if( ndiag_watch->upnp_isavail() )	disp_upnp_isavail();
			else					disp_upnp_unavail();
			

			// if upnp_isavail, launch a upnp_call_statusinfo_t
			if( ndiag_watch->upnp_isavail() )	statusinfo_ctor();

			// if upnp_unavail, delete upnp_call_statusinfo_t if needed
			if( ndiag_watch->upnp_unavail() )	statusinfo_dtor();
			// if upnp_unavail, delete upnp_scanallport_t if needed
			if( ndiag_watch->upnp_unavail() )	scanallport_dtor();
			
			break;
	case ndiag_watch_t::IPADDR_PVIEW_CHANGED:
			disp_ipaddr_pview_changed();
			break;
	default:	DBG_ASSERT( 0 );
	}
		
	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_call_statusinfo_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief contruct the upnp_statusinfo_t 
 * 
 * - NOTE: may be called event with upnp_statusinfo_t not init
 */
void	upnp_apps_t::statusinfo_ctor()		throw()
{
	// sanity check - upnp MUST be available
	DBG_ASSERT( ndiag_watch_get()->upnp_isavail() );
	// init some variable
	ndiag_watch_t *		ndiag_watch	= ndiag_watch_get();
	upnp_watch_t *		upnp_watch	= ndiag_watch->upnp_watch();
	const upnp_disc_res_t &	disc_res	= upnp_watch->current_disc_res();
	upnp_err_t		upnp_err;
	// sanity check - call_statusinfo MUST be NULL
	DBG_ASSERT( call_statusinfo == NULL );	
	// launch a upnp_call_statusinfo_t
	call_statusinfo	= nipmem_new upnp_call_statusinfo_t();
	upnp_err	= call_statusinfo->start(disc_res, this, NULL);
	if( upnp_err.failed() ){
		KLOG_STDOUT("Cant launch upnp_call_statusinfo_t due to " << upnp_err);
		nipmem_zdelete call_statusinfo;
	}				
}

/** \brief destruct the upnp_statusinfo_t 
 * 
 * - NOTE: may be called event with upnp_statusinfo_t not init
 */
void	upnp_apps_t::statusinfo_dtor()		throw()
{
	// delete upnp_statusinfo_t if needed
	nipmem_zdelete	call_statusinfo;
}
/** \brief callback notified by \ref upnp_call_statusinfo_t when completed
 */
bool 	upnp_apps_t::neoip_upnp_call_statusinfo_cb(void *cb_userptr, upnp_call_statusinfo_t &cb_call_statusinfo
						, const upnp_err_t &upnp_err)	throw()
{
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err);
	
	// display the result in case of faillure or success
	if( upnp_err.succeed() )	disp_statusinfo_success();
	else				disp_statusinfo_failure(upnp_err);
	
	// delete the upnp_call_statusinfo_t
	nipmem_zdelete	call_statusinfo;
	
	// if upnp_isavail, launch a upnp_scanallport_t
	// - NOTE: it is tested again as this may have changed during
	//   the upnp_call_statusinfo_t completion
	if( ndiag_watch_get()->upnp_isavail() )	scanallport_ctor();

	// return dontkeep - as the object has just been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_scanallport_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief contruct the upnp_scanallport_t 
 * 
 * - NOTE: may be called event with upnp_scanallport_t not init
 */
void	upnp_apps_t::scanallport_ctor()		throw()
{
	// sanity check - upnp MUST be available
	DBG_ASSERT( ndiag_watch_get()->upnp_isavail() );
	// init some variable
	ndiag_watch_t *		ndiag_watch	= ndiag_watch_get();
	upnp_watch_t *		upnp_watch	= ndiag_watch->upnp_watch();
	const upnp_disc_res_t &	disc_res	= upnp_watch->current_disc_res();
	upnp_err_t		upnp_err;
	// sanity check - upnp_scanallport MUST be NULL
	DBG_ASSERT( upnp_scanallport == NULL );	
	// launch a upnp_call_statusinfo_t
	upnp_scanallport= nipmem_new upnp_scanallport_t();
	upnp_err	= upnp_scanallport->start(disc_res, this, NULL);
	if( upnp_err.failed() ){
		KLOG_STDOUT("Cant launch upnp_scanallport_t due to " << upnp_err);
		nipmem_zdelete	upnp_scanallport;
	}				
}

/** \brief destruct the upnp_scanallport_t 
 * 
 * - NOTE: may be called event with upnp_scanallport_t not init
 */
void	upnp_apps_t::scanallport_dtor()		throw()
{
	// delete upnp_scanallport_t if needed
	nipmem_zdelete	upnp_scanallport;
}

/** \brief callback notified by \ref upnp_scanallport_t when completed
 */
bool 	upnp_apps_t::neoip_upnp_scanallport_cb(void *cb_userptr, upnp_scanallport_t &cb_scanallport
						, const upnp_err_t &upnp_err)	throw()
{
	// log to debug
	KLOG_ERR("enter upnp_err=" << upnp_err);
	
	// display the result in case of faillure or success
	if( upnp_err.succeed() )	disp_scanallport_success(upnp_scanallport->portdesc_arr());
	else				disp_scanallport_failure(upnp_err);
	
	// delete the upnp_scanallport_t
	nipmem_zdelete	upnp_scanallport;
	
	// return dontkeep - as the object has just been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	upnp_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END;




