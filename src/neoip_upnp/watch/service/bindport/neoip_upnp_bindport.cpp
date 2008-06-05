/*! \file
    \brief Definition of the upnp_bindport_t

\par Brief Description
\ref upnp_bindport_t bind a port in the upnp router. The algorithm followed to 
bind a port is:
- if the pview_hint is null, put a random port in it ip_addr_t::ANY_IP4:random port
- test if pview_hint is already bound in the upnp router
- if not, bind it and notify the ipport_addr_t pview to the caller
- if it is already bound, pick a random port and test if it is already bound
  - if it is, loop until either the max number of retries is reached
    or until an unbound one is found
- on upnp_bindport_t deletion, launch a upnp_call_delport_t to delete it
  - this is a special case where the upnp_call_delport_t it supposed to 
    autodelete itself.
  - this is required as the upnp_bindport_t deletion is insync and return immediatly
    so nomore object to receive the callback notification 

\par About the dependancy on upnp_call_getport_t
- a upnp_call_getport_t is done before doing the actual upnp_call_addport_t.
  - it aims to test is the port is already allocated or not
  - this is required to test it via getport, because a addport on a already
    allocated port will silently overwrite the previous allocation.
  - without it, different implementations may overwrite each others allocation
    without being aware of it. 

\par About handling ipport_addr_t and not just port
- it is because upnp protocol is supposed to handle ipport_addr_t for internal/lview
  and external/pview.
  - so this function handles stuff as for the upnp protocol without assuming policy
  - but in practice the ipport_addr_t::ipaddr() is almost always ip_addr_t::ANY_IP4

\par About lease duration and renewal
- the 'offical' lease delay in the router is not used
  - it is because all upnp routers i tried do not support it and will simply
    report an error when asked to have a lease.
  - so it is not used by this code in order to stay as compatible as possible
    with most upnp router
- But it uses the upnp_portcleaner_t mechanism to provide dangling port expiration
  with a piggyback mechanism in the port description string.

*/

/* system include */
/* local include */
#include "neoip_upnp_bindport.hpp"
#include "neoip_upnp_call_getport.hpp"
#include "neoip_upnp_call_addport.hpp"
#include "neoip_upnp_call_delport.hpp"
#include "neoip_upnp_watch.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_rand.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

#include "neoip_upnp_portcleaner.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_bindport_t::upnp_bindport_t()		throw()
{
	// zero some fields
	upnp_watch	= NULL;
	call_getport	= NULL;
	call_addport	= NULL;
	// set some default value
	getport_nbretry	= 0;
	m_is_bound	= false;
}

/** \brief Desstructor
 */
upnp_bindport_t::~upnp_bindport_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the upnp_call_getport_t if needed
	nipmem_zdelete	call_getport;
	// delete the upnp_call_addport_t if needed
	nipmem_zdelete	call_addport;
	
	// unlink it from upnp_watch_t if needed
	if( upnp_watch )	upnp_watch->bindport_unlink(this);
	// launch a call_delport IIF the port has been already bound AND upnp_isavail
	// - this is done thru upnp_watch_t as this is done in the dtor
	// - unless to launch a upnp_call_delport_t if no upnp... so in *theory*
	//   it doesnt leave dangling allocation.
	//   - in *practice* tho, it may very well be a misdetection and would leave
	//     crubs, in this case, one rely on the upnp_portcleaner_t to remove them
	if( is_bound() && upnp_watch->upnp_isavail()){
		upnp_err_t	upnp_err;
		upnp_err	= upnp_watch->launch_delport(ipport_pview().port()
					, upnp_sockfam(), upnp_watch->current_disc_res());
		if( upnp_err.failed() )	KLOG_ERR("cant launch upnp_call_delport_t due to " << upnp_err);
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
upnp_bindport_t &upnp_bindport_t::set_profile(const upnp_bindport_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == upnp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}
/** \brief start function - the generic one used by all the helper one
 */
upnp_err_t upnp_bindport_t::start(upnp_watch_t *upnp_watch, const ipport_addr_t &m_ipport_lview
				, const upnp_sockfam_t &m_upnp_sockfam, const ipport_addr_t &m_ipport_pview
				, const std::string &description_str
				, upnp_bindport_cb_t *callback, void * userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->upnp_watch	= upnp_watch;
	this->m_ipport_lview	= m_ipport_lview;
	this->m_ipport_pview	= m_ipport_pview;
	this->m_upnp_sockfam	= m_upnp_sockfam;
	this->description_str	= description_str;
	this->callback		= callback;
	this->userptr		= userptr;
	// link it to upnp_watch_t
	upnp_watch->bindport_dolink(this);

	// if upnp_watch_t has no discovered upnp router, return an error
	if( upnp_watch->upnp_unavail())	return upnp_err_t(upnp_err_t::ERROR, "upnp not available");

	// if the ipport_pview is null, pick a random one
	if( ipport_pview().is_null() )	this->m_ipport_pview = pick_random_pview();

	// sanity check - the ipport_lview().ipaddr() MUST be is_any()
	DBG_ASSERT( ipport_lview().ipaddr().is_any() );
	// sanity check - the ipport_pview().ipaddr() MUST be is_any()
	DBG_ASSERT( ipport_pview().ipaddr().is_any() );

	// init the itor_expire_timeout
	itor_expire_timeout.start(profile.itor_expire_delay(), this, NULL);
	
	// launch the getport
	upnp_err_t	upnp_err;
	upnp_err	= launch_getport();
	if( upnp_err.failed() )	return upnp_err;

	// return no error
	return upnp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a random ipport_addr_t pview
 * 
 * - NOTE: the ipport_addr_t::ipaddr() is always "0.0.0.0"
 */
ipport_addr_t	upnp_bindport_t::pick_random_pview()	const throw()
{
	// pick a random port
	uint16_t port	= uint16_t(neoip_rand(1,65535));
	// return the picked random ipport_addr_t
	return ipport_addr_t("0.0.0.0", port);
}

/** \brief Return a upnp_call_getport_t
 */
upnp_err_t	upnp_bindport_t::launch_getport()	throw()
{
	// log to debug
	KLOG_DBG("enter getport_nbretry=" << getport_nbretry << " ipport_pview=" << ipport_pview());
	// sanity check - upnp_call_getport_t MUST NOT be running
	DBG_ASSERT( call_getport == NULL );
	// sanity check - upnp_call_addport_t MUST NOT be running
	DBG_ASSERT( call_addport == NULL );
	
	// update the getport_nbretry
	getport_nbretry++;
	// if the getport_nbretry reached its maximum from the profile, report an error
	if( getport_nbretry >= profile.getport_nbretry_max() )
		return upnp_err_t(upnp_err_t::ERROR, "max number of call_getport has been reached");

	// launch the upnp_call_getport_t utself
	upnp_err_t	upnp_err;
	call_getport	= nipmem_new upnp_call_getport_t();
	call_getport->set_profile(upnp_watch->get_profile().call());
	upnp_err	= call_getport->start(upnp_watch->current_disc_res(), ipport_pview().port()
								, upnp_sockfam(), this, NULL);
	if( upnp_err.failed() )	return upnp_err;

	// return noerror
	return upnp_err_t::OK;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_call_getport_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref upnp_call_getport_t when completed
 */
bool 	upnp_bindport_t::neoip_upnp_call_getport_cb(void *cb_userptr, upnp_call_getport_t &cb_call_getport
						, const upnp_err_t &cb_upnp_err)	throw()
{
	upnp_err_t	upnp_err	= cb_upnp_err;;
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err);
	// delete the upnp_call_getport_t
	nipmem_zdelete	call_getport;
	
	// if call_getport succeed, ipport_pview is already bound so pick a new one, and relaunch getport
	if( upnp_err.succeed() ){
		m_ipport_pview	= pick_random_pview();
		upnp_err	= launch_getport();
		if( upnp_err.failed() )	return notify_callback(upnp_err, ipport_addr_t());	
		return false;	
	}

	// stop the itor_expire_timeout - which is only for the itor phase
	itor_expire_timeout.stop();
	
	// zero the addport_counter
	addport_counter		= rand();	
	// init the upnp_call_addport_t timeout stuff
	addport_delaygen	= delaygen_t(profile.addport_delaygen_arg());
	addport_timeout.start(addport_delaygen.current(), this, NULL);
	
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_call_addport_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref upnp_call_addport_t when completed
 */
bool 	upnp_bindport_t::neoip_upnp_call_addport_cb(void *cb_userptr, upnp_call_addport_t &cb_call_addport
						, const upnp_err_t &upnp_err)	throw()
{
	// log to debug
	KLOG_ERR("enter upnp_err=" << upnp_err);
	// delete the upnp_call_addport_t
	nipmem_zdelete	call_addport;

	// if the upnp_call_addport_t failed, forward the error to the caller
	if( upnp_err.failed() )	return notify_callback(upnp_err, ipport_addr_t());

	// relaunch the addport_timeout for the next upnp_disc_t
	addport_timeout.start(addport_delaygen.pre_inc(), this, NULL);
	
	// if it is already declared is_bound, return now
	// - it may happen during the refreshing of the addport
	if( is_bound() )	return false;
	
	// set the is_bound to true
	m_is_bound	= true;
	
	// notify the success to the caller
	// - then sit there waiting the deletion
	bool	tokeep	= notify_callback(upnp_err_t::OK, ipport_pview());
	if( !tokeep )	return tokeep;	

	// return dontkeep
	return false;
}


/** \brief callback called when the neoip_timeout expire
 */
bool upnp_bindport_t::addport_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// sanity check - call_addport MUST be null while addport_timeout is running
	DBG_ASSERT( call_addport == NULL );

	// notify the expiration of the timeout to the delaygen
	addport_delaygen.notify_expiration();
	// if the delaygen is is_timedout, dont relaunch the timeout
	if( addport_delaygen.is_timedout() ){
		addport_timeout.stop();
		return true;
	}

	// build the tag_str - using the upnp_portcleaner_t tag builder
	std::string	tag_str	= upnp_portcleaner_t::build_tag_desc(description_str
						, profile.portcleaner_lease_delay()
						, OSTREAMSTR(char(addport_counter%26+'a')));


	// launch the upnp_call_addport_t
	upnp_err_t	upnp_err;
	call_addport	= nipmem_new upnp_call_addport_t();
	upnp_err	= call_addport->set_profile(upnp_watch->get_profile().call())
					.start(upnp_watch->current_disc_res()
						, ipport_pview().port(), ipport_lview().port()
						, upnp_sockfam(), delay_t::INFINITE
						, tag_str, this, NULL);
	if( upnp_err.failed() )	return notify_callback(upnp_err, ipport_addr_t());

	// update the addport_counter
	addport_counter++;	


	// stop the addport_timeout while upnp_addport_t is pending - will be relaunch after its completion
	addport_timeout.stop();
	
	// return a 'tokeep'
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_bindport_t::neoip_timeout_expire_cb(void *cb_userptr, timeout_t &cb_timeout)	throw()
{
	// just forward to the specific callbacks
	if( &itor_expire_timeout == &cb_timeout)	return itor_expire_timeout_cb(cb_userptr, cb_timeout);
	if( &addport_timeout == &cb_timeout)		return addport_timeout_cb(cb_userptr, cb_timeout);
	// NOTE: this poinrt MUST NEVER be reached
	DBG_ASSERT( 0 );
	return false;
}


/** \brief callback called when the neoip_timeout expire
 */
bool upnp_bindport_t::itor_expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// notify the caller of the expiration
	std::string reason = "upnp_bindport_t timedout after " + OSTREAMSTR(itor_expire_timeout.get_period());
	return notify_callback(upnp_err_t(upnp_err_t::ERROR, reason), ipport_addr_t());
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_bindport_t::notify_callback(const upnp_err_t &upnp_err, const ipport_addr_t &ipport_pview)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_upnp_bindport_cb(userptr, *this, upnp_err, ipport_pview);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


