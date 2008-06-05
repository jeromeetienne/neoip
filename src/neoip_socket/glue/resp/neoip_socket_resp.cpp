/*! \file
    \brief Definition of the socket_resp_t

*/


/* system include */
/* local include */
#include "neoip_socket_resp.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of the factory plant for socket_resp_vapi_t 
FACTORY_PLANT_DEFINITION (socket_resp_factory, socket_domain_t, socket_resp_vapi_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_resp_t::socket_resp_t() throw()
{
	// zero some fields
	m_resp_vapi	= NULL;
}

/** \brief Desstructor
 */
socket_resp_t::~socket_resp_t() throw()
{
	if( m_resp_vapi )	resp_vapi()->destroy();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set the profile
 */
socket_resp_t &	socket_resp_t::set_profile(const socket_profile_t &socket_profile)	throw()
{
	// sanity check - socket_resp_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// set the profile
	resp_vapi()->set_profile(socket_profile);
	// return the object itself
	return *this;
}

/** \brief Setup the socket_resp_t with all the mandatory parameter
 */
socket_err_t	socket_resp_t::setup(const socket_domain_t &socket_domain, const socket_type_t &socket_type
					, const socket_addr_t &listen_addr
					, socket_resp_cb_t *callback, void *userptr)	throw()
{
	// sanity check
	DBG_ASSERT( is_null() );
	// sanity check - the socket_domain_t MUST support the socket_type
	DBG_ASSERT( socket_domain.support(socket_type) );

	// copy the callback parameter
	this->callback	= callback;
	this->userptr	= userptr;

	// create the responder thru the factory
	m_resp_vapi	= socket_resp_factory->create(socket_domain);
	DBG_ASSERT( m_resp_vapi );
	
	// then setup the resp_vapi
	return resp_vapi()->setup(socket_type, listen_addr, this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bunch of helper
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Start helper which does setup() before
 */
socket_err_t	socket_resp_t::start(const socket_domain_t &socket_domain
				, const socket_type_t &socket_type
				, const socket_addr_t &listen_addr
				, socket_resp_cb_t *callback, void *userptr)	throw()
{
	socket_err_t	socket_err;
	// setup the socket_resp_t
	socket_err	= setup(socket_domain, socket_type, listen_addr, callback, userptr);
	if( socket_err.failed() )	return socket_err;
	// start it
	return start();
}


/** \brief Setup the socket_resp_t with a socket_resp_arg_t
 */
socket_err_t	socket_resp_t::setup(const socket_resp_arg_t &arg
				, socket_resp_cb_t *callback, void *userptr)	throw()
{
	socket_err_t	socket_err;
	// sanity check - the socket_resp_arg_t MUST be valid
	DBG_ASSERT( arg.is_valid() );
	// setup the socket_resp_t
	socket_err	= setup(arg.domain(), arg.type(), arg.listen_addr(), callback, userptr);
	if( socket_err.failed() )	return socket_err;
	// add the profile if present
	if( arg.profile_present() )	set_profile(arg.profile());	
	// return noerror
	return socket_err_t::OK;
}
/** \brief Start helper which does setup() before
 */
socket_err_t	socket_resp_t::start(const socket_resp_arg_t &arg
				, socket_resp_cb_t *callback, void *userptr)	throw()
{
	// setup the socket_resp_t
	socket_err_t	socket_err	= setup(arg, callback, userptr);
	if( socket_err.failed() )	return socket_err;
	// start it
	return start();
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_resp_vapi_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_resp_vapi_t to provide event
 * 
 * - note: just forward the socket_event_t from the socket_resp_vapi_t
 */
bool	socket_resp_t::neoip_socket_resp_vapi_cb(void *userptr, socket_resp_vapi_t &cb_resp_vapi
						, const socket_event_t &socket_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( socket_event.is_resp_ok() );	
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
bool 	socket_resp_t::notify_callback(const socket_event_t &socket_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_resp_event_cb(userptr, *this, socket_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}




NEOIP_NAMESPACE_END

