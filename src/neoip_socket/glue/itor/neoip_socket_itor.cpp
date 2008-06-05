/*! \file
    \brief Definition of the socket_itor_t
*/


/* system include */
/* local include */
#include "neoip_socket_itor.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of the factory plant for socket_itor_vapi_t 
FACTORY_PLANT_DEFINITION (socket_itor_factory, socket_domain_t, socket_itor_vapi_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_itor_t::socket_itor_t() throw()
{
	m_itor_vapi	= NULL;
}

/** \brief Desstructor
 */
socket_itor_t::~socket_itor_t() throw()
{
	if( m_itor_vapi )	itor_vapi()->destroy();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set the profile
 */
socket_itor_t &	socket_itor_t::set_profile(const socket_profile_t &socket_profile)	throw()
{
	// sanity check - socket_itor_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// set the profile
	itor_vapi()->set_profile(socket_profile);
	// return the object itself
	return *this;
}

/** \brief Setup the socket_itor_t with all the mandatory parameter
 */
socket_err_t socket_itor_t::setup(const socket_domain_t &socket_domain, const socket_type_t &socket_type
				, const socket_addr_t &local_addr, const socket_addr_t &remote_addr
				, socket_itor_cb_t *callback, void *userptr)	throw()
{
	// sanity check
	DBG_ASSERT( is_null() );
	// sanity check - the socket_domain_t MUST support the socket_type
	DBG_ASSERT( socket_domain.support(socket_type) );
	
	// copy the callback parameter
	this->callback	= callback;
	this->userptr	= userptr;	
	// create the itor thru the factory
	m_itor_vapi	= socket_itor_factory->create(socket_domain);
	// return no error
	return itor_vapi()->setup(socket_type, local_addr, remote_addr, this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bunch of helper
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Start helper which does setup() before
 */
socket_err_t socket_itor_t::start(const socket_domain_t &socket_domain, const socket_type_t &socket_type
				, const socket_addr_t &local_addr, const socket_addr_t &remote_addr
				, socket_itor_cb_t *callback, void *userptr)	throw()
{
	socket_err_t	socket_err;
	// setup the socket_itor_t
	socket_err	= setup(socket_domain, socket_type, local_addr, remote_addr, callback, userptr);
	if( socket_err.failed() )	return socket_err;
	// start it
	return start();
}


/** \brief Setup the socket_itor_t with a socket_itor_arg_t
 */
socket_err_t	socket_itor_t::setup(const socket_itor_arg_t &arg
				, socket_itor_cb_t *callback, void *userptr)	throw()
{
	socket_err_t	socket_err;
	socket_addr_t	local_addr;
	// sanity check - the socket_itor_arg_t MUST be valid
	DBG_ASSERT( arg.is_valid() );
	// if the local address is present in the arg, copy it
	if( arg.local_addr_present() )	local_addr	= arg.local_addr();
	// setup the socket_itor_t
	socket_err	= setup(arg.domain(), arg.type(), local_addr, arg.remote_addr(),callback,userptr);
	if( socket_err.failed() )	return socket_err;
	// add the profile if present
	if( arg.profile_present() )	set_profile(arg.profile());
	// return noerror
	return socket_err_t::OK;
}
/** \brief Start helper which does setup() before
 */
socket_err_t	socket_itor_t::start(const socket_itor_arg_t &arg
				, socket_itor_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// setup the socket_itor_t
	socket_err_t	socket_err	= setup(arg, callback, userptr);
	if( socket_err.failed() )	return socket_err;
	// start it
	return start();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_itor_vapi_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_itor_vapi_t to provide event
 * 
 * - note: just forward the socket_event_t from the socket_itor_vapi_t
 */
bool	socket_itor_t::neoip_socket_itor_vapi_cb(void *userptr, socket_itor_vapi_t &cb_itor_vapi
						, const socket_event_t &socket_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( socket_event.is_itor_ok() );	
	// notify the socket_event_t to the caller
	return notify_callback(socket_event);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	socket_itor_t::notify_callback(const socket_event_t &socket_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_itor_event_cb(userptr, *this, socket_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END

