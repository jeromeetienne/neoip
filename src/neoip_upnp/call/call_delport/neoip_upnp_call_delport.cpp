/*! \file
    \brief Definition of the upnp_call_delport_t

\par About null callback and upnp_watch_t 
\ref upnp_call_delport_t has a special status compared to other upnp_call_t 
because it is typically used during the destructoin of an object.
As destructor are synchronous and as upnp_call_delport_t is asynchronous, 
upnp_call_delport_t  MUST:
1. have a null callback to show this specific case and autodelete itself
   on completion with null callback.
2. be linked a 'global' object
   - and thus be deleted when the whole program shutdown
   - i would prefere a pure global stuff - aka not something which must be 
     passed as parameter. thus no API pollution
     - this is a clear layer violation
     - the upnp_watch_t is used. it is global. but the linking is done by the caller.
- additionnaly the global layer MUST linger at the end to try to delete all the port
  libsess_exit_t or something
  
*/

/* system include */
/* local include */
#include "neoip_upnp_call_delport.hpp"
#include "neoip_upnp_call.hpp"
#include "neoip_upnp_disc_res.hpp"
#include "neoip_upnp_sockfam.hpp"
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
upnp_call_delport_t::upnp_call_delport_t()		throw()
{
	// init the upnp_call_t
	upnp_call	= nipmem_new upnp_call_t();
}

/** \brief Desstructor
 */
upnp_call_delport_t::~upnp_call_delport_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
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
upnp_call_delport_t &upnp_call_delport_t::set_profile(const upnp_call_profile_t &profile)	throw()
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
upnp_err_t upnp_call_delport_t::start(const upnp_disc_res_t &disc_res
				, uint16_t port_pview, const upnp_sockfam_t &upnp_sockfam
				, upnp_call_delport_cb_t *callback, void * userptr)	throw()
{
	upnp_err_t	upnp_err;
	// log to debug
	KLOG_DBG("enter port_pview=" << port_pview << " upnp_sockfam=" << upnp_sockfam);
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;

	// set the method_name and strvar_db
	std::string	method_name	= "DeletePortMapping";
	strvar_db_t	strvar_db;
	strvar_db.append("NewRemoteHost"	, ""				);
	strvar_db.append("NewExternalPort"	, OSTREAMSTR(port_pview)	);
	strvar_db.append("NewProtocol"		, upnp_sockfam.to_string()	);
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
bool	upnp_call_delport_t::neoip_upnp_call_cb(void *cb_userptr, upnp_call_t &cb_call
				, const upnp_err_t &upnp_err, const strvar_db_t &strvar_db)	throw()
{
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err << " strvar_db=" << strvar_db
							<< " callback=" << callback);
	
	// if callback == NULL, autodelete - see the file header for details
	if( callback == NULL ){
		// if there is an error, log it - as it wont be reported in a callback
		if( upnp_err.failed() )	KLOG_ERR("cant delete port due to " << upnp_err);
		// autodelete and return dontkeep
		nipmem_delete this;
		return false;
	}
	
	// if the upnp_call_t failed, forward the upnp_err_t
	if( upnp_err.failed() )	return notify_callback(upnp_err);
	
	// NOTE: if this point is reached, the upnp_call_delport_t has been successfull, notify the caller
	return notify_callback(upnp_err_t::OK);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_call_delport_t::notify_callback(const upnp_err_t &upnp_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_upnp_call_delport_cb(userptr, *this, upnp_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


