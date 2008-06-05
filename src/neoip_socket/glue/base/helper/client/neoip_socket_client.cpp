/*! \file
    \brief Definition of the socket_client_t
*/


/* system include */
/* local include */
#include "neoip_socket_client.hpp"
#include "neoip_socket.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_client_t::socket_client_t() throw()
{
	socket_itor	= nipmem_new socket_itor_t();
	socket_full	= NULL;
	callback	= NULL;
}

/** \brief Desstructor
 */
socket_client_t::~socket_client_t() throw()
{
	nipmem_zdelete	socket_itor;
	nipmem_zdelete	socket_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set the profile
 */
socket_client_t &socket_client_t::set_profile(const socket_profile_t &socket_profile)	throw()
{
	// sanity check - socket_client_t MUST be a itor
	DBG_ASSERT( is_itor() );
	// set the profile
	socket_itor->set_profile(socket_profile);
	// return the object itself
	return *this;
}

/** \brief Setup the socket_client_t with all the mandatory parameter
 */
socket_err_t socket_client_t::setup(const socket_domain_t &socket_domain, const socket_type_t &socket_type
				, const socket_addr_t &local_addr, const socket_addr_t &remote_addr
				, socket_client_cb_t *callback, void *userptr)	throw()
{
	// sanity check
	DBG_ASSERT( is_itor() );
	// copy the callback
	this->callback	= callback;
	this->userptr	= userptr;
	// return no error
	return socket_itor->setup(socket_domain, socket_type, local_addr, remote_addr, this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bunch of helper
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Start helper which does setup() before
 */
socket_err_t socket_client_t::start(const socket_domain_t &socket_domain, const socket_type_t &socket_type
				, const socket_addr_t &local_addr, const socket_addr_t &remote_addr
				, socket_client_cb_t *callback, void *userptr)	throw()
{
	socket_err_t	socket_err;
	// setup the socket_client_t
	socket_err	= setup(socket_domain, socket_type, local_addr, remote_addr, callback, userptr);
	if( socket_err.failed() )	return socket_err;
	// start it
	return start();
}


/** \brief Setup the socket_client_t with a socket_itor_arg_t
 */
socket_err_t	socket_client_t::setup(const socket_itor_arg_t &arg
				, socket_client_cb_t *callback, void *userptr)	throw()
{
	socket_err_t	socket_err;
	socket_addr_t	local_addr;	
	// sanity check - the socket_itor_arg_t MUST be valid
	DBG_ASSERT( arg.is_valid() );
	// if the local address is present in the arg, copy it
	if( arg.local_addr_present() )	local_addr	= arg.local_addr();	
	// setup the socket_client_t
	socket_err	= setup(arg.domain(), arg.type(), local_addr, arg.remote_addr(),callback,userptr);
	if( socket_err.failed() )	return socket_err;
	// add the profile if present
	if( arg.profile_present() )	set_profile(arg.profile());
	// return noerror
	return socket_err_t::OK;
}

/** \brief Start helper which does setup() before
 */
socket_err_t	socket_client_t::start(const socket_itor_arg_t &arg
				, socket_client_cb_t *callback, void *userptr)	throw()
{
	// setup the socket_itor_t
	socket_err_t	socket_err	= setup(arg, callback, userptr);
	if( socket_err.failed() )	return socket_err;
	// start it
	return start();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      to_string() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_client_t::to_string()					const throw()
{
	if( socket_itor )	return socket_itor->to_string();
	else if(socket_full)	return socket_full->to_string();
	return "null";
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       steal_full()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Steal the socket_full_t from a established socket_client_t.
 * 
 * - After this call, the caller is MUST delete the socket_client_t
 * - clearly a 'caller MUST know what he is doing' function 
 * 
 * @return the socket_full_t pointer
 */
socket_full_t *	socket_client_t::steal_full()	throw()
{
	// sanity check - socket_full_t MUST be non-null
	DBG_ASSERT( socket_full );
	// mark a copy the socket_full
	socket_full_t *	socket_full_stolen = socket_full;
	// set the callback to NULL 
	// - protective measure to ensure it wont call this socket_client_t without noticing
	socket_full_stolen->set_callback(NULL, NULL);
	// mark socket_full as unused
	socket_full	= NULL;
	// return the copy
	return socket_full_stolen;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     initiator callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_itor_t when a connection is established
 */
bool	socket_client_t::neoip_socket_itor_event_cb(void *userptr, socket_itor_t &cb_socket_itor
					, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( socket_event.is_itor_ok() );
	
	// delete socket_itor and mark it unused
	nipmem_zdelete	socket_itor;
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:
			recv_cnx_established(socket_event.get_cnx_established());
			// the tokeep/dontkeep returned by the callback is ignored as the 
			// socket_itor has already been deleted so false is returned
			return false;
	case socket_event_t::CNX_REFUSED:
			// simply forward the event
			notify_callback(socket_event);
			// the tokeep/dontkeep returned by the callback is ignored as the 
			// socket_itor has already been deleted so false is returned
			return false;
	default:	DBG_ASSERT(0);
	}
	// return 'dontkeep'
	return false;
}

/** \brief handle the reception of a socket_event_t::CNX_ESTABLISHED from itor
 */
void	socket_client_t::recv_cnx_established(socket_full_t *socket_full)	throw()
{
	socket_err_t	socket_err;
	socket_event_t	socket_event;
	// copy the socket_full_t pointer
	this->socket_full = socket_full;
	// start the socket_full
	socket_err = socket_full->start(this, NULL);
	if( socket_err.failed() )	goto error;

	// logging to debug
	KLOG_DBG("create a full socket " << *socket_full);
	
	// notify a CNX_ESTABLISHED with a NULL pointer
	// - the NULL pointer is required as the socket_full is already contained in the socket_client_t
	socket_event = socket_event_t::build_cnx_established(NULL);
	// notify the event
	// - may delete the socket_client_t itself so dont use it after this notification
	notify_callback(socket_event);
	// return now
	return;
	
error:;	KLOG_ERR("Unable to init socket_full due to " << socket_err );
	// delete socket_full and mark it unused
	nipmem_delete	socket_full;
	socket_full = NULL;
	// notify a CNX_REFUSED
	socket_event = socket_event_t::build_cnx_refused("Unable to init socket_full due to "+ socket_err.to_string());
	// notify the event
	// - may delete the socket_client_t itself so dont use it after this notification
	notify_callback(socket_event);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when to notify an event
 */
bool	socket_client_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
					, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );
	// sanity check - here the socket_itor MUST be NULL
	DBG_ASSERT( socket_itor == NULL );

 	// backup the object_slotid of the socket_full_t - may be needed in case of stolen socket_full_t
 	slot_id_t	socket_full_slotid	= socket_full->get_object_slotid();
 
	// simply forward the event whatever the event type
	bool tokeep	= notify_callback(socket_event);
	// if tokeep == 'dontkeep' compute socket_full_t tokeep as it may have been stolen during the cb
	if( !tokeep )	return object_slotid_tokeep(socket_full_slotid);
	// return 'tokeep'
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        function to notify caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief notify the callback with the socket_event
 */
bool socket_client_t::notify_callback(const socket_event_t &socket_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_client_event_cb(userptr, *this, socket_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}
NEOIP_NAMESPACE_END

