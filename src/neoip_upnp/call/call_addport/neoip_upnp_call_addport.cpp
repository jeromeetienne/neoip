/*! \file
    \brief Definition of the upnp_call_addport_t

*/

/* system include */
/* local include */
#include "neoip_upnp_call_addport.hpp"
#include "neoip_upnp_call.hpp"
#include "neoip_upnp_disc_res.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_date.hpp"
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
upnp_call_addport_t::upnp_call_addport_t()		throw()
{
	// init the upnp_call_t
	upnp_call	= nipmem_new upnp_call_t();
}

/** \brief Desstructor
 */
upnp_call_addport_t::~upnp_call_addport_t()		throw()
{
	// delete the upnp_call_t if needed
	nipmem_zdelete	upnp_call;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
upnp_call_addport_t &upnp_call_addport_t::set_profile(const upnp_call_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == upnp_err_t::OK );
	// forward the parameter to upnp_call_t
	upnp_call->set_profile( profile );
	// return the object iself
	return *this;
}

/** \brief start function - the generic one used by all the helper one
 */
upnp_err_t upnp_call_addport_t::start(const upnp_disc_res_t &disc_res
				, uint16_t port_pview, uint16_t port_lview
				, const upnp_sockfam_t &upnp_sockfam, const delay_t &lease_delay
				, const std::string &description_str
				, upnp_call_addport_cb_t *callback, void * userptr)	throw()
{
	upnp_err_t		upnp_err;
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;

	// set the method_name and strvar_db
	std::string	method_name	= "AddPortMapping";
	strvar_db_t	strvar_db;
	strvar_db.append("NewRemoteHost"	, ""					);
	strvar_db.append("NewExternalPort"	, OSTREAMSTR(port_pview)		);
	strvar_db.append("NewProtocol"		, upnp_sockfam.to_string()		);
	strvar_db.append("NewInternalPort"	, OSTREAMSTR(port_lview)		);
	strvar_db.append("NewInternalClient"	, disc_res.local_ipaddr().to_string()	);
	strvar_db.append("NewEnabled"		, "1"					);
	strvar_db.append("NewPortMappingDescription", description_str			);
	if( lease_delay.is_infinite() ){
		strvar_db.append("NewLeaseDuration", "0"				);
	}else{	
		strvar_db.append("NewLeaseDuration", OSTREAMSTR(lease_delay.to_sec_32bit()));
	}	
	// launch the upnp_call_t
	upnp_err	= upnp_call->start(disc_res.control_uri(), disc_res.service_name(), method_name
								, strvar_db, this, NULL);
	if( upnp_err.failed() )	return upnp_err;

	// return no error
	return upnp_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_call_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref upnp_call_t on completion
 */
bool	upnp_call_addport_t::neoip_upnp_call_cb(void *cb_userptr, upnp_call_t &cb_call
				, const upnp_err_t &cb_upnp_err, const strvar_db_t &strvar_db)	throw()
{
	// log to debug
	KLOG_DBG("enter upnp_err=" << cb_upnp_err << " strvar_db=" << strvar_db);
	
	// if the upnp_call_t failed, forward the upnp_err_t
	if( cb_upnp_err.failed() )	return notify_callback(cb_upnp_err);
	
	// NOTE: if this point is reached, the upnp_call_addport_t has been successfull, notify the caller
	return notify_callback(upnp_err_t::OK);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_call_addport_t::notify_callback(const upnp_err_t &upnp_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_upnp_call_addport_cb(userptr, *this, upnp_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


