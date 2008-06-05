/*! \file
    \brief Definition of the upnp_getportendian_test_t

- TODO should be renamed getport_endianbug or something
  - for sure getportendian_test is NO GOOD
- TODO BUG BUG in getport identification
  - the identification of the port in the upnp router is done by the tag
  - BUT this tag is a constant! so 2 getport at the same time/port may
    conflict.
  - this is unlikely but trivial to fix
  - just add a nonce. ok how large ?
    
\par Brief Description
\ref upnp_getportendian_test_t tests the upnp router for presence of the 
getport reverse endian bug.

\par description of the getport reverse endian bug
- BUG: some upnp routers inverse the endianness of the reported ip_addr_t and port
  - those data are sent over the network as ASCII in http so the endiannesss
    should not matter. So this is a bug inside their implementations. 
  - they forgot a htons/htonl somewhere. 
- ip_addr_t example: they report 2.1.168.192 instead of 192.168.1.2
- port example: they report 20480 (aka 0x5000) instead of 80 (aka 0x0050)
- NOTE: this case is quite personnal as my personnal ISP box got this issue.
- currently there is a flag in the profile to determine if the upnp router
  got this issue or not

\par autodetection algorithm
- pick a random port for tcp
- do 2 getport: 1 with normal endian, 1 with inverse endian
- if any return success, goto back to step 1
- addport the chosen port
- do 2 getport: 1 with normal endian, 1 with inverse endian
- if the inverse endian succeed, the bug is present
- if the normal endian succeed, the bug is not present
- if none succeed, the bug is assumed not present

*/

/* system include */
/* local include */
#include "neoip_upnp_getportendian_test.hpp"
#include "neoip_upnp_call_getport.hpp"
#include "neoip_upnp_call_addport.hpp"
#include "neoip_upnp_call_delport.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_rand.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

#include "neoip_ndiag_watch.hpp"	// just here for the delport kludge
#include "neoip_upnp_watch.hpp"		// dirty stuff and this is a layer violation

#include "neoip_upnp_portcleaner.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_getportendian_test_t::upnp_getportendian_test_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	itor_getport_norendian	= NULL;
	itor_getport_revendian	= NULL;
	call_addport		= NULL;
	full_getport_norendian	= NULL;
	full_getport_revendian	= NULL;
	// set some default value
	getport_nbretry	= 0;
	is_bound	= false;
}

/** \brief Desstructor
 */
upnp_getportendian_test_t::~upnp_getportendian_test_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the upnp_call_addport_t if needed
	nipmem_zdelete	call_addport;

	// delete the various upnp_call_getport_t if needed
	nipmem_zdelete	itor_getport_norendian;
	nipmem_zdelete	itor_getport_revendian;
	nipmem_zdelete	full_getport_norendian;
	nipmem_zdelete	full_getport_revendian;
	
	// launch a upnp_call_delport_t IIF the port has been already bound
	// - as it is done inside the destructor, it has a usual trouble
	//   of launch a async operation(delport) into a sync function(dtor)
	// - 2 possibilities: both implemented
	//   1. do a layer violation and do the delete thru upnp_watch
	//      - it has the advantage of attaching the pointer somewhere and to
	//        honor the lib_session_exit_t
	//      - dirty as it does a serious layer violation
	//   2. create a dangling pointer on a upnp_call_delport_t which will 
	//      autodelete itself on completion
	//      - it will autodelete itself on completion but may report unfreed memory
	//        if done just before the end of the program.
	//      - dirty due to the dangling pointer
	// - so if upnp_watch_t is available thry ndiag_watch, use this one. 
	//   else use the other
	//   - in fact the other is just for completness as in practice
	//     upnp_watch_t is always available. so it is dirty but works without
	//     issue 
	if( is_bound ){
		ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
		upnp_watch_t *	upnp_watch	= ndiag_watch ? ndiag_watch->upnp_watch():NULL;
		upnp_err_t	upnp_err;
		// if upnp_watch_t is available, do the delete thru it
		if( upnp_watch ){
			upnp_err	= upnp_watch->launch_delport(port_pview
						, upnp_sockfam_t::TCP, upnp_disc_res);
			if( upnp_err.failed() )	KLOG_ERR("cant launch upnp_call_delport_t due to " << upnp_err);
		}else{	// if upnp_watch_t IS NOT avail, launch it thru dangling pointer
			upnp_call_delport_t *	call_delport;
			call_delport	= nipmem_new upnp_call_delport_t();
			upnp_err	= call_delport->set_profile(profile.call())
						.start(upnp_disc_res, port_pview
							, upnp_sockfam_t::TCP, NULL, NULL);
			if( upnp_err.failed() )	KLOG_ERR("cant launch upnp_call_delport_t due to " << upnp_err);
		}
	}


}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
upnp_getportendian_test_t &upnp_getportendian_test_t::set_profile(const upnp_getportendian_test_profile_t &profile)	throw()
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
upnp_err_t upnp_getportendian_test_t::start(const upnp_disc_res_t &upnp_disc_res
				, upnp_getportendian_test_cb_t *callback, void * userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->upnp_disc_res	= upnp_disc_res;
	this->callback		= callback;
	this->userptr		= userptr;

	// init the itor_expire_timeout
	expire_timeout.start(profile.expire_delay(), this, NULL);
	
	// pick a random port lview
	port_lview	= uint16_t(neoip_rand(1,65535));
	// pick a random port pview
	port_pview	= uint16_t(neoip_rand(1,65535));
	// launch the getport
	upnp_err_t	upnp_err;
	upnp_err	= itor_getport_launch();
	if( upnp_err.failed() )	return upnp_err;

	// return no error
	return upnp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_call_getport_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref upnp_call_getport_t when completed
 */
bool 	upnp_getportendian_test_t::neoip_upnp_call_getport_cb(void *cb_userptr, upnp_call_getport_t &cb_call_getport
						, const upnp_err_t &upnp_err)	throw()
{
	// just forward to the specific callbacks
	if(itor_getport_norendian == &cb_call_getport)	return itor_getport_cb(cb_userptr, cb_call_getport, upnp_err);
	if(itor_getport_revendian == &cb_call_getport)	return itor_getport_cb(cb_userptr, cb_call_getport, upnp_err);
	if(full_getport_norendian == &cb_call_getport)	return full_getport_cb(cb_userptr, cb_call_getport, upnp_err);
	if(full_getport_revendian == &cb_call_getport)	return full_getport_cb(cb_userptr, cb_call_getport, upnp_err);
	// NOTE: this poinrt MUST NEVER be reached
	DBG_ASSERT( 0 );
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			itor_getport stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the 2 itor_getport norendian/revendian
 */
upnp_err_t	upnp_getportendian_test_t::itor_getport_launch()	throw()
{
	upnp_err_t	upnp_err;
	// log to debug
	KLOG_DBG("enter getport_nbretry=" << getport_nbretry << " port_pview=" << port_pview);
	// sanity check - upnp_call_getport_t MUST NOT be running
	DBG_ASSERT( !itor_getport_revendian && !itor_getport_norendian );
	
	// update the getport_nbretry
	getport_nbretry++;
	// if the getport_nbretry reached its maximum from the profile, report an error
	if( getport_nbretry >= profile.getport_nbretry_max() )
		return upnp_err_t(upnp_err_t::ERROR, "max number of call_getport has been reached");

	// launch the itor_getport_revendian
	itor_getport_revendian	= nipmem_new upnp_call_getport_t();
	itor_getport_revendian->set_profile(profile.call()).revendian(true);
	upnp_err		= itor_getport_revendian->start(upnp_disc_res, port_pview
							, upnp_sockfam_t::TCP, this, NULL);
	if( upnp_err.failed() )	return upnp_err;

	// launch the itor_getport_norendian
	itor_getport_norendian	= nipmem_new upnp_call_getport_t();
	itor_getport_norendian->set_profile(profile.call()).revendian(false);
	upnp_err		= itor_getport_norendian->start(upnp_disc_res, port_pview
							, upnp_sockfam_t::TCP, this, NULL);
	if( upnp_err.failed() )	return upnp_err;

	// return noerror
	return upnp_err_t::OK;
}


/** \brief callback notified by \ref upnp_call_getport_t when completed
 */
bool 	upnp_getportendian_test_t::itor_getport_cb(void *cb_userptr, upnp_call_getport_t &cb_call_getport
						, const upnp_err_t &cb_upnp_err)	throw()
{
	upnp_err_t	upnp_err	= cb_upnp_err;;
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err);
	// delete the upnp_call_getport_t
	if(itor_getport_norendian == &cb_call_getport)	nipmem_zdelete	itor_getport_norendian;
	if(itor_getport_revendian == &cb_call_getport)	nipmem_zdelete	itor_getport_revendian;
	
	// if call_getport succeed, ipport_pview is already bound so pick a new one, and relaunch getport
	if( upnp_err.succeed() ){
		// delete the 2 itor_getport 
		nipmem_zdelete	itor_getport_norendian;
		nipmem_zdelete	itor_getport_revendian;
		// pick a new port_pview
		port_pview	= uint16_t(neoip_rand(1,65535));
		// relaunch it
		upnp_err	= itor_getport_launch();
		if( upnp_err.failed() )	return notify_callback(upnp_err);	
		return false;	
	}

	// if both itor_getport are completed, launch the addport phase
	if( !itor_getport_norendian && !itor_getport_revendian ){
		upnp_err	= call_addport_launch();
		if( upnp_err.failed() )	return notify_callback(upnp_err);
	}
	
	// return dontkeep
	return false;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_call_addport_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the 2 full_getport norendian/revendian
 */
upnp_err_t	upnp_getportendian_test_t::call_addport_launch()		throw()
{
	upnp_err_t	upnp_err;
	// log to debug
	KLOG_DBG("enter port_pview=" << port_pview);
	// sanity check - upnp_call_getport_t MUST NOT be running
	DBG_ASSERT( !itor_getport_norendian && !itor_getport_revendian );
	DBG_ASSERT( !full_getport_norendian && !full_getport_revendian );
	DBG_ASSERT( !call_addport );

	// build the tag_str - using the upnp_portcleaner_t tag builder
	// - NOTE: no need to bother to renew this tag or handle any renew because
	//   it is very short-lived and will be deleted at the end of 
	//   this upnp_getportendian_test_t
	std::string	portcleaner_tag;
	portcleaner_tag	= upnp_portcleaner_t::build_tag_desc("upnp revendian testing"
						, delay_t::from_sec(60), "dummy");

	// launch the full_getport_revendian
	call_addport	= nipmem_new upnp_call_addport_t();
	upnp_err	= call_addport->set_profile(profile.call()).start(upnp_disc_res
						, port_pview, port_lview, upnp_sockfam_t::TCP
						, delay_t::INFINITE, portcleaner_tag
						, this, NULL);
	if( upnp_err.failed() )	return upnp_err;

	// return noerror
	return upnp_err_t::OK;
}

/** \brief callback notified by \ref upnp_call_addport_t when completed
 */
bool 	upnp_getportendian_test_t::neoip_upnp_call_addport_cb(void *cb_userptr, upnp_call_addport_t &cb_call_addport
						, const upnp_err_t &cb_upnp_err)	throw()
{
	upnp_err_t	upnp_err	= cb_upnp_err;
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err);
	// delete the upnp_call_addport_t
	nipmem_zdelete	call_addport;

	// if the upnp_call_addport_t failed, forward the error to the caller
	if( upnp_err.failed() )	return notify_callback(upnp_err);
	
	// set the is_bound to true
	is_bound	= true;
	
	// launch the full_getport
	upnp_err	= full_getport_launch();
	if( upnp_err.failed() )	return notify_callback(upnp_err);

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			full_getport
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the 2 full_getport norendian/revendian
 */
upnp_err_t	upnp_getportendian_test_t::full_getport_launch()	throw()
{
	upnp_err_t	upnp_err;
	// log to debug
	KLOG_DBG("enter port_pview=" << port_pview);
	// sanity check - upnp_call_getport_t MUST NOT be running
	DBG_ASSERT( !itor_getport_norendian && !itor_getport_revendian );
	DBG_ASSERT( !full_getport_norendian && !full_getport_revendian );
	DBG_ASSERT( !call_addport );

	// launch the full_getport_revendian
	full_getport_revendian	= nipmem_new upnp_call_getport_t();
	full_getport_revendian->set_profile(profile.call()).revendian(true);
	upnp_err		= full_getport_revendian->start(upnp_disc_res, port_pview
							, upnp_sockfam_t::TCP, this, NULL);
	if( upnp_err.failed() )	return upnp_err;

	// launch the full_getport_norendian
	full_getport_norendian	= nipmem_new upnp_call_getport_t();
	full_getport_norendian->set_profile(profile.call()).revendian(false);
	upnp_err		= full_getport_norendian->start(upnp_disc_res, port_pview
							, upnp_sockfam_t::TCP, this, NULL);
	if( upnp_err.failed() )	return upnp_err;

	// return noerror
	return upnp_err_t::OK;
}

/** \brief callback notified by \ref upnp_call_getport_t when completed
 */
bool 	upnp_getportendian_test_t::full_getport_cb(void *cb_userptr, upnp_call_getport_t &cb_call_getport
						, const upnp_err_t &cb_upnp_err)	throw()
{
	upnp_call_getport_t *	call_getport	= &cb_call_getport;
	const upnp_portdesc_t &	portdesc	= call_getport->portdesc();
	upnp_err_t		upnp_err	= cb_upnp_err;;
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err);
	
	// if call_getport succeed, then it show the norendian/revendian
	// - NOTE: the description_str is tested to ensure it has been allocated by this apps
	//   and not another apps which won a race and bind it between the itor_getport and 
	//   the full_getport
	if( upnp_err.succeed() && portdesc.desc_str() == "upnp revendian testing"
				&& portdesc.ipport_lview().port() == port_lview ){
		// set the m_is_revendian
		if(full_getport_norendian == call_getport)	m_is_revendian	= false;
		if(full_getport_revendian == call_getport)	m_is_revendian	= true;
		// log to debug
		KLOG_DBG("is_revendian()=" << is_revendian() );
		// notify the caller, that the result is known
		return notify_callback(upnp_err_t::OK);
	}

	// delete the upnp_call_getport_t
	if(full_getport_norendian == &cb_call_getport)	nipmem_zdelete	full_getport_norendian;
	if(full_getport_revendian == &cb_call_getport)	nipmem_zdelete	full_getport_revendian;

	// if both full_getport are completed with non succeeding, assume it is normal endian
	if( !full_getport_norendian && !full_getport_revendian ){
		std::string	reason	= "Impossible to complete the full_getport.. upnp_call_getport_t not supported ?";
		return notify_callback(upnp_err_t(upnp_err_t::ERROR, reason));
	}
	
	// return dontkeep
	return false;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_getportendian_test_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// notify the caller of the expiration
	std::string reason = "upnp_getportendian_test_t timedout after " + OSTREAMSTR(expire_timeout.get_period());
	return notify_callback(upnp_err_t(upnp_err_t::ERROR, reason));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_getportendian_test_t::notify_callback(const upnp_err_t &upnp_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_upnp_getportendian_test_cb(userptr, *this, upnp_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


