/*! \file
    \brief Definition of the upnp_call_getport_t

\par About upnp_disc_res_t and local revendian field
- upnp_call_getport_t will use the reverse endianness bug workaround IIF revendian() is set
- revendian() defaults to false
- the caller may set revendian(true) before calling start()
- in start(), if the local revendian() is false but the upnp_disc_res_t::getport_endianbug()
  is set, start() will automatically set revendian() to true
- thus if the caller want to use the revendian() of the upnp_disc_res_t, it doesnt
  have to explicitly set revendian()
- thus if the caller want to force the usage of revendian() even if
  upnp_disc_res_t::getport_endianbug(), it may use call_getport->revendian(true).start(...)
- nevertheless if upnp_disc_res_t::getport_endianbug() is true, the caller can
  not force it to false.
  - which is not a problem because the only user of this feature is upnp_getportendian_test_t
    and in this very specific case, upnp_disc_res_t::getport_endianbug() is false.

*/

/* system include */
/* local include */
#include "neoip_upnp_call_getport.hpp"
#include "neoip_upnp_call.hpp"
#include "neoip_upnp_disc_res.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_endianswap.hpp"
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
upnp_call_getport_t::upnp_call_getport_t()		throw()
{
	// init the upnp_call_t
	upnp_call	= nipmem_new upnp_call_t();
	// set some default value
	m_revendian	= false;
}

/** \brief Desstructor
 */
upnp_call_getport_t::~upnp_call_getport_t()		throw()
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
upnp_call_getport_t &upnp_call_getport_t::set_profile(const upnp_call_profile_t &profile)	throw()
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
upnp_err_t upnp_call_getport_t::start(const upnp_disc_res_t &disc_res, uint16_t port_pview_orig
				, const upnp_sockfam_t &upnp_sockfam
				, upnp_call_getport_cb_t *callback, void * userptr)	throw()
{
	upnp_err_t		upnp_err;
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;
	
	// if disc_res.getport_endianbug() is set, set revendian to true
	if( disc_res.getport_endianbug() )	revendian(true);

	// compute the port_pview to use
	uint16_t port_pview	= port_pview_orig;
	if( revendian() )	port_pview = endianswap_t::swap16(port_pview_orig);

	// set the method_name and strvar_db
	std::string	method_name	= "GetSpecificPortMappingEntry";
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
bool	upnp_call_getport_t::neoip_upnp_call_cb(void *cb_userptr, upnp_call_t &cb_call
				, const upnp_err_t &cb_upnp_err, const strvar_db_t &strvar_db)	throw()
{
	// log to debug
	KLOG_DBG("enter upnp_err=" << cb_upnp_err << " strvar_db=" << strvar_db);
	
	// if the upnp_call_t failed, forward the upnp_err_t
	if( cb_upnp_err.failed() )	return notify_callback(cb_upnp_err);

	// parse the upnp_portdesc_t from the strvar_db
	m_portdesc	= upnp_portdesc_t::from_strvar_db(strvar_db, revendian());
	if( portdesc().is_null() ){
		upnp_err_t upnp_err(upnp_err_t::ERROR, "unable to parse a upnp_portdesc_t in upnp_call_t response");
		return notify_callback(upnp_err);
	}

	// notify the called of the success
	return notify_callback(upnp_err_t::OK);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_call_getport_t::notify_callback(const upnp_err_t &upnp_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_upnp_call_getport_cb(userptr, *this, upnp_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


