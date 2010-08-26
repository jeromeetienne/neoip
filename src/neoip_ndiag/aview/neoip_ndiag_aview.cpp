/*! \file
    \brief Definition of the ndiag_aview_t

- TODO the variable naming is very bad
  - the variable name are way too long

\par Note about ndiag_cacheport_t
- the desc_str() is used as key in the ndiag_cacheport_t
- thus a ndiag_aview_t with the same desc_str() as the previous run will attemps
  to get the same port public view than previously
  - this help for the bootstrapping in general.

\par Notes
- the ipport_pview_curr() is generated immediatly during the start()
  - so it may be used by the caller just after it
- this may generate ipport_pview_curr() 
  - being null: if there are no pview at all
  - being aport_qualified: if only the port of the pview is known
  - being fully_qualified: if the port and ipaddr of the pview are known
- ipport_pview_curr() may change over time
  - e.g. it is_fully_qualified() and suddently the ipaddr_pview change
    then it is renotified with the new ipaddr
  - e.g. it is fully_qualified(), thanks to upnp, and suddently upnp is nomore
    available, so it is notified as null

*/

/* system include */
/* local include */
#include "neoip_ndiag_aview.hpp"
#include "neoip_ndiag_err.hpp"
#include "neoip_ndiag_watch.hpp"
#include "neoip_ndiag_cacheport.hpp"
#include "neoip_upnp_bindport.hpp"
#include "neoip_upnp_sockfam.hpp"
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
ndiag_aview_t::ndiag_aview_t()		throw()
{
	// zero some field
	upnp_bindport	= NULL;
}

/** \brief Desstructor
 */
ndiag_aview_t::~ndiag_aview_t()		throw()
{
	// unhook the ndiag_watch_t events if needed
	if( callback ){
		ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
		ndiag_watch->event_hook_remove(ndiag_watch_t::UPNPDISC_CHANGED, this, NULL);
		ndiag_watch->event_hook_remove(ndiag_watch_t::IPADDR_PVIEW_CHANGED, this, NULL);
	}
	// delete the upnp_bindport_t
	nipmem_zdelete	upnp_bindport;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief start function - the generic one used by all the helper one
 */
ndiag_err_t ndiag_aview_t::start(const ipport_aview_t &m_ipport_aview_init, const upnp_sockfam_t &m_sockfam
				, const std::string &m_desc_str
				, ndiag_aview_cb_t *callback, void * userptr)	throw()
{
	ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
	ndiag_err_t	ndiag_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the m_ipport_aview_init MUST be is_run_ok
	DBG_ASSERT( m_ipport_aview_init.is_run_ok() );
	// copy the parameter
	this->m_ipport_aview_init	= m_ipport_aview_init;
	this->m_sockfam			= m_sockfam;
	this->m_desc_str		= m_desc_str;
	this->callback			= callback;
	this->userptr			= userptr;

	// dohook the ndiag_watch_t event
	ndiag_watch->event_hook_append(ndiag_watch_t::UPNPDISC_CHANGED, this, NULL);
	ndiag_watch->event_hook_append(ndiag_watch_t::IPADDR_PVIEW_CHANGED, this, NULL);

	// generate the ipport_pview_curr if possible
	m_ipport_pview_curr	= generate_pview_curr();
	
	// if a upnp_bindport_t is needed and upnp_watch_t say upnp_isavail, launch a upnp_bindport_t now
	if( ndiag_watch->upnp_isavail() && is_bindport_needed() ){
		ndiag_err	= launch_upnp_bindport();
		if( ndiag_err.failed() )	return ndiag_err;
	}

	// return no error
	return ndiag_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief update ipport_pview_curr() and possibly notify the caller
 * 
 * - NOTE: this may delete the ndiag_aview_t
 */
bool	ndiag_aview_t::update_pview_curr()	throw()
{
	// save the ipport_pview_curr()
	ipport_addr_t old_pview	= ipport_pview_curr();
	// generate the ipport_pview_curr if possible
	m_ipport_pview_curr	= generate_pview_curr();
	// if the ipport_pview_curr() changed, notify the caller
	if( ipport_pview_curr() != old_pview ){
		bool tokeep	= notify_callback(ipport_pview_curr());
		if( !tokeep )	return false;
	}
	// return tokeep
	return true;
}
/** \brief Generate ipport_pview_curr if possible
 */
ipport_addr_t	ndiag_aview_t::generate_pview_curr()	throw()
{
	ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
	// if ipport_aview_init().pview() already is_fully_qualified, just copy it
	if( ipport_aview_init().pview().is_fully_qualified() )
		return ipport_aview_init().pview();
	
	// if ipport_aview_init().pview() is_aport_qualified, build the ipport_pview_curr() from it
	if( ipport_aview_init().pview().is_aport_qualified() ){
		ip_addr_t	ipaddr_pview	= "0.0.0.0";
		// if ndiag_watch->ipaddr_pview() is known, use it too
		if( !ndiag_watch->ipaddr_pview().is_null() )	ipaddr_pview = ndiag_watch->ipaddr_pview();
		// build the ipport_addr_t to return
		return ipport_addr_t(ipaddr_pview, ipport_aview_init().pview().port());
	}
	
	// if ipport_aview_init().pview() is null, and upnp_bindport->ipport_pview()
	// is_bound, then build ipport_pview_curr() from it
	if( ipport_aview_init().pview().is_null() && upnp_bindport && upnp_bindport->is_bound() ){
		ip_addr_t	ipaddr_pview;
		// if ndiag_watch->ipaddr_pview() is known, use it too
		if( !ndiag_watch->ipaddr_pview().is_null() )	ipaddr_pview = ndiag_watch->ipaddr_pview();
		// build the ipport_addr_t to return
		return ipport_addr_t(ipaddr_pview, upnp_bindport->ipport_pview().port());
	}
	
	// in all other cases, ipport_pview_curr is null
	return ipport_addr_t();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ndiag_watch_t event_hook_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 */
bool	ndiag_aview_t::neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
							, int hook_level)	throw()
{
	ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
	// log to debug
	KLOG_DBG("enter");
	
	// handle the ndiag_watch event_hook depending on the hook_level
	switch( hook_level ){
	case ndiag_watch_t::UPNPDISC_CHANGED:
			KLOG_ERR("upnp_isavail_changed. current=" << ndiag_watch->upnp_isavail());
			// if upnp is nomore available, and a upnp_bindport is pending, delete it
			if( ndiag_watch->upnp_unavail() && upnp_bindport )
				nipmem_zdelete upnp_bindport;
			// if upnp is newly available, and a upnp_bindport is needed, launch one
			if( ndiag_watch->upnp_isavail() && !upnp_bindport && is_bindport_needed() ){
				ndiag_err_t	ndiag_err;
				ndiag_err	= launch_upnp_bindport();
				if( ndiag_err.failed() )	KLOG_ERR("cant start upnp_bindport_t due to " << ndiag_err);
			}	
			break;
	case ndiag_watch_t::IPADDR_PVIEW_CHANGED:
			// log to debug
			KLOG_ERR("ipaddr_pview_changed. current=" << ndiag_watch->ipaddr_pview());
			// fall thru to get the update_pview_curr()
			break;
	default:	DBG_ASSERT( 0 );
	}
	
	// update the ipport_pview_curr()
	bool	tokeep	= update_pview_curr();
	if( !tokeep )	return false;
	
	// return tokeep
	return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_bindport_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if a upnp_bindport_t is needed, false otherwise
 */
bool	ndiag_aview_t::is_bindport_needed()	const throw()
{
	// if ipport_aview_init().pview() IS NOT null, it is not needed
	// - NOTE: if the port is already available, only the ipaddr_pview is needed
	if( !ipport_aview_init().pview().is_null() )	return false;
	// else it it needed
	return true;
}

/** \brief Launch a upnp_bindport_t
 */
ndiag_err_t	ndiag_aview_t::launch_upnp_bindport()	throw()
{
	upnp_watch_t *		upnp_watch	= ndiag_watch_get()->upnp_watch();
	upnp_err_t		upnp_err;
	// log to debug
	KLOG_ERR("enter");
	// sanity check - the upnp_bindport MUST be NULL
	DBG_ASSERT( upnp_bindport == NULL );
	// sanity check - is_bindport_needed() MUST be true
	DBG_ASSERT( is_bindport_needed() );
	// sanity check - upnp MUST be availlable
	DBG_ASSERT( ndiag_watch_get()->upnp_isavail() );
	
	// build the ipport_pview
	ipport_addr_t	ipport_pview;
	
#if 0	// TODO to fix
	// - it has been disabled as neoip-webpeer get a LOT of conflict in this
	//   - e.g. all bt_ezsession got the same desc_str so all obtain the same
	//     external port... gaps
	
	
	// try to get a value from the ndiag_cacheport_t
	// - TODO it would be good to put the apps name into the description
	//   - thus 2 apps running on the same cache wont conflict
	//   - desc_str = "neoip-router UDP"
	//   - desc_str = "neoip-get TCP" or "neoip-get UDP"
	//   - this is done in the caller, not here
	// - TODO this is unclear if a local port should be cached as well as the 
	//   local port is used quite a lot too
	//   - caching the local port would require some modification on the caller usage
	// - this caching system is not yet clear
	ndiag_cacheport_t *	ndiag_cacheport	= ndiag_watch_get()->ndiag_cacheport();
	if( ndiag_cacheport->contain(desc_str(), sockfam()) ){
		uint16_t	cached_port;
		cached_port	= ndiag_cacheport->item(desc_str(), sockfam()).port();
		ipport_pview	= ipport_addr_t("0.0.0.0", cached_port);
		// log to debug
		KLOG_DBG("ipport_pview found in cacheport=" << ipport_pview << " for desc_str=" << desc_str());
	}
#endif
	
	// start the upnp_bindport_t
	upnp_bindport	= nipmem_new upnp_bindport_t();
	upnp_err	= upnp_bindport->start(upnp_watch, ipport_aview_init().lview(), sockfam()
						, ipport_pview, desc_str(), this, NULL); 
	if( upnp_err.failed() )	return ndiag_err_from_upnp(upnp_err);
	// return no error
	return ndiag_err_t::OK;
}

/** \brief callback notified by \ref upnp_bindport_t on completion
 */
bool	ndiag_aview_t::neoip_upnp_bindport_cb(void *cb_userptr, upnp_bindport_t &cb_upnp_bindport
			, const upnp_err_t &upnp_err, const ipport_addr_t &ipport_pview)	throw()
{
	ndiag_cacheport_t *	ndiag_cacheport	= ndiag_watch_get()->ndiag_cacheport();
	// log to debug
	KLOG_ERR("enter upnp_err=" << upnp_err << " ipport_pview=" << ipport_pview);
	
	// sanity check - the ipport_pview MUST NOT be null as upnp_err.succeed()
	if( upnp_err.succeed() )	DBG_ASSERT( !ipport_pview.is_null() );

	// if upnp_bindport_t failed, delete it
	if( upnp_err.failed() )		nipmem_zdelete upnp_bindport;
	// update the new port into the ndiag_cacheport_t - if 
	if( upnp_err.succeed() )	ndiag_cacheport->update(desc_str(), sockfam(), ipport_pview.port());


	// update the ipport_pview_curr()
	bool	tokeep	= update_pview_curr();
	if( !tokeep )	return false;

	// return tokeep/dontkeep depending on the upnp_bindport existence
	if( upnp_bindport )	return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool ndiag_aview_t::notify_callback(const ipport_addr_t &ipport_pview)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ndiag_aview_cb(userptr, *this, ipport_pview);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


