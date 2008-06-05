/*! \file
    \brief Definition of the upnp_scanallport_t

*/

/* system include */
/* local include */
#include "neoip_upnp_scanallport.hpp"
#include "neoip_upnp_call_scanport.hpp"
#include "neoip_upnp_err.hpp"
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
upnp_scanallport_t::upnp_scanallport_t()		throw()
{
	// zero some fields
	call_scanport	= NULL;
	scanidx		= 0;
}

/** \brief Desstructor
 */
upnp_scanallport_t::~upnp_scanallport_t()		throw()
{
	// delete the upnp_call_scanport_t if needed
	nipmem_zdelete	call_scanport;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief start function - the generic one used by all the helper one
 */
upnp_err_t upnp_scanallport_t::start(const upnp_disc_res_t &m_disc_res
				, upnp_scanallport_cb_t *callback, void * userptr)	throw()
{
	upnp_err_t	upnp_err;
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->m_disc_res	= m_disc_res;
	this->callback		= callback;
	this->userptr		= userptr;

	// launch the first upnp_call_scanport
	upnp_err	= launch_next_scanport();
	if( upnp_err.failed() )	return upnp_err;	
	
	// return no error
	return upnp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_call_scanport_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the next upnp_call_scanport_t
 */
upnp_err_t	upnp_scanallport_t::launch_next_scanport()	throw()
{
	upnp_err_t	upnp_err;
	// sanity check - call_scanport MUST be NULL
	DBG_ASSERT( call_scanport == NULL );
	// start a upnp_call_scanport_t
	call_scanport	= nipmem_new upnp_call_scanport_t();
	upnp_err	= call_scanport->start(disc_res(), scanidx, this, NULL);
	if( upnp_err.failed() )	return upnp_err;
	// return noerror
	return upnp_err_t::OK;
}

/** \brief callback notified by \ref upnp_call_scanport_t when completed
 */
bool 	upnp_scanallport_t::neoip_upnp_call_scanport_cb(void *cb_userptr, upnp_call_scanport_t &cb_call_scanport
						, const upnp_err_t &cb_upnp_err)	throw()
{
	upnp_err_t	upnp_err	= cb_upnp_err;
	upnp_portdesc_t	upnp_portdesc	= call_scanport->portdesc();
	// log to debug
	KLOG_DBG("enter upnp_err=" << upnp_err << " scanidx=" << scanidx << " portdesc=" << upnp_portdesc);

	// if there is an error, this is considered the end of the list
	if( upnp_err.failed() )	return notify_callback(upnp_err_t::OK );
	
	// update the portdesc_arr
	m_portdesc_arr.append(upnp_portdesc);
	
	// delete the upnp_call_scanport_t
	nipmem_zdelete	call_scanport;
	
	// update the scanidx for the next upnp_call_scanport_t
	scanidx++;
	// launch the next upnp_call_scanport_t
	upnp_err	= launch_next_scanport();
	if( upnp_err.failed() )	return notify_callback(upnp_err);
	
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
bool upnp_scanallport_t::notify_callback(const upnp_err_t &upnp_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_upnp_scanallport_cb(userptr, *this, upnp_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


