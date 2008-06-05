/*! \file
    \brief Definition of the upnp_call_extipaddr_t

*/

/* system include */
/* local include */
#include "neoip_upnp_call_extipaddr.hpp"
#include "neoip_upnp_call.hpp"
#include "neoip_upnp_disc_res.hpp"
#include "neoip_upnp_err.hpp"
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
upnp_call_extipaddr_t::upnp_call_extipaddr_t()		throw()
{
	// init the upnp_call_t
	upnp_call	= nipmem_new upnp_call_t();
}

/** \brief Desstructor
 */
upnp_call_extipaddr_t::~upnp_call_extipaddr_t()		throw()
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
upnp_call_extipaddr_t &upnp_call_extipaddr_t::set_profile(const upnp_call_profile_t &profile)	throw()
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
upnp_err_t upnp_call_extipaddr_t::start(const upnp_disc_res_t &disc_res
				, upnp_call_extipaddr_cb_t *callback, void * userptr)		throw()
{
	upnp_err_t	upnp_err;
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;

	// set the method_name and strvar_db
	std::string	method_name	= "GetExternalIPAddress";
	strvar_db_t	strvar_db;
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
bool	upnp_call_extipaddr_t::neoip_upnp_call_cb(void *cb_userptr, upnp_call_t &cb_call
				, const upnp_err_t &cb_upnp_err, const strvar_db_t &strvar_db)	throw()
{
	// log to debug
	KLOG_DBG("enter upnp_err=" << cb_upnp_err << " strvar_db=" << strvar_db);
	
	// if the upnp_call_t failed, forward the upnp_err_t
	if( cb_upnp_err.failed() )	return notify_callback(cb_upnp_err, ip_addr_t());
	
	// if the upnp_call_t replied strvar_db doesnt contain "NewExternalIPAddr", notify an error
	if( !strvar_db.contain_key("NewExternalIPAddress") ){
		upnp_err_t upnp_err(upnp_err_t::ERROR, "unable to find NewExternalIPAddress in upnp_call_t response");
		return notify_callback(upnp_err, ip_addr_t());
	}

	// try to convert the content into a ip_addr_t
	ip_addr_t extipaddr	= strvar_db.get_first_value("NewExternalIPAddress");
	// if the content can't be parse as an ip_addr_t, notify an error
	if( !extipaddr.is_fully_qualified() ){
		upnp_err_t upnp_err(upnp_err_t::ERROR, "unable to parse NewExternalIPAddress as ip_addr_t");
		return notify_callback(upnp_err, ip_addr_t());
	}
	
	// NOTE: if this point is reached, the upnp_call_extipaddr_t has been successfull, notify the caller
	return notify_callback(upnp_err_t::OK, extipaddr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_call_extipaddr_t::notify_callback(const upnp_err_t &upnp_err, const ip_addr_t &ipaddr)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_upnp_call_extipaddr_cb(userptr, *this, upnp_err, ipaddr);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


