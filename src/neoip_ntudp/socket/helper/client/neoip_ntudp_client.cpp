/*! \file
    \brief Definition of the ntudp_client_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_client.hpp"
#include "neoip_ntudp_event.hpp"
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
ntudp_client_t::ntudp_client_t() throw()
{
	// init some field
	ntudp_itor	= nipmem_new ntudp_itor_retry_t();
	ntudp_full	= NULL;
	callback	= NULL;
}

/** \brief Desstructor
 */
ntudp_client_t::~ntudp_client_t() throw()
{
	// delete ntudp_itor_t if needed
	nipmem_zdelete	ntudp_itor;
	// delete ntudp_full_t if needed
	nipmem_zdelete	ntudp_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Setup functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
void	ntudp_client_t::set_callback(ntudp_client_cb_t *callback, void * userptr)	throw()
{
	// copy the callback field
	this->callback	= callback;
	this->userptr	= userptr;
}


/** \brief Start the operation
 */
ntudp_err_t	ntudp_client_t::start()							throw()
{
	// set the ntudp_itor_t callback
	ntudp_itor->set_callback(this, NULL);
	// start the operation on the ntudp_itor_t
	return ntudp_itor->start();
}

/** \brief Setup all mandatory parameters and start the operation
 */
ntudp_err_t	ntudp_client_t::start(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &remote_addr
					, ntudp_client_cb_t *callback, void * userptr)		throw()
{
	ntudp_err_t	ntudp_err;
	// set the remote_addr
	ntudp_err	= set_remote_addr(ntudp_peer, remote_addr);
	if( ntudp_err.failed() )	return ntudp_err;
	// set the callback
	set_callback(callback, userptr);
	// start the operation now
	return start();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       steal_ntudp_full()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Steal the ntudp_full_t from a established ntudp_client_t.
 * 
 * - After this call, the caller is MUST delete the ntudp_client_t
 * - clearly a 'caller MUST know what he is doing' function 
 * 
 * @return the ntudp_full_t pointer
 */
ntudp_full_t *	ntudp_client_t::steal_full()	throw()
{
	// sanity check - ntudp_full_t MUST be non-null
	// mark a copy the ntudp_full
	ntudp_full_t *	ntudp_full_copy = ntudp_full;
	// set the callback to NULL 
	// - protective measure to ensure it wont call this ntudp_client_t without noticing
	ntudp_full_copy->set_callback(NULL, NULL);
	// mark ntudp_full as unused
	ntudp_full	= NULL;
	// return the copy
	return ntudp_full_copy;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     to_string function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	ntudp_client_t::to_string()					const throw()
{
	if( ntudp_itor )	return ntudp_itor->to_string();
	else if(ntudp_full)	return ntudp_full->to_string();
	return "NONE";
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     initiator callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_itor_t when a connection is established
 */
bool	ntudp_client_t::neoip_ntudp_itor_retry_event_cb(void *userptr, ntudp_itor_retry_t &cb_itor_retry
					, const ntudp_event_t &ntudp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << ntudp_event);
	// sanity check - the event MUST be is_itor_retry_ok()
	DBG_ASSERT( ntudp_event.is_itor_retry_ok() );

	// delete ntudp_itor and mark it unused
	nipmem_delete	ntudp_itor;
	ntudp_itor	= NULL;

	// handle each possible events from its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_ESTABLISHED:{
			ntudp_full_t *	ntudp_full = ntudp_event.get_cnx_established();
			recv_cnx_established(ntudp_full);
			return false;}
	case ntudp_event_t::UNKNOWN_HOST:
	case ntudp_event_t::NO_ROUTE_TO_HOST:
	case ntudp_event_t::CNX_ESTA_TIMEDOUT:
			// just forward the event
			notify_callback(ntudp_event);
			return false;
	default:	DBG_ASSERT(0);
	}

	// return 'dontkeep' - as it has been deleted
	return false;
}

/** \brief handle the reception of a ntudp_event_t::CNX_ESTABLISHED from itor
 */
bool	ntudp_client_t::recv_cnx_established(ntudp_full_t *ntudp_full)	throw()
{
	ntudp_err_t	ntudp_err;
	ntudp_event_t	ntudp_event;
	// copy the ntudp_full_t pointer
	this->ntudp_full = ntudp_full;
	// start the ntudp_full
	ntudp_err = ntudp_full->start(this, NULL);
	if( ntudp_err.failed() )	goto error;

	// logging to debug
	KLOG_DBG("create a full udp " << *ntudp_full);
	
	// notify a CNX_ESTABLISHED with a NULL pointer
	// - the NULL pointer is required as the ntudp_full is already contained in the ntudp_client_t
	ntudp_event = ntudp_event_t::build_cnx_established(NULL);
	// notify the event
	return notify_callback(ntudp_event);
	
error:;	KLOG_ERR("Unable to init ntudp_full due to " << ntudp_err );
	// delete ntudp_full and mark it unused
	nipmem_zdelete	ntudp_full;
	// TODO: solve this issue
	// - ntudp_event has no real cnx_refused as it has no connection establishement over the wire
	// - so how to notify this error ?
	// - note that this error is currently quite theorical - doing a ASSERT( 0 ) until it 
	//   a solution is found
	DBG_ASSERT( 0 );
	// notify a CNX_REFUSED
//	ntudp_event = ntudp_event_t::build_cnx_refused("Unable to init ntudp_full due to "+ ntudp_err.to_string());
	// notify the event
//	notify_callback(ntudp_event);
	// return dontkeep - as the notified has just been deleted
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     ntudp_full callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_full_t when to notify an event
 */
bool	ntudp_client_t::neoip_ntudp_full_event_cb(void *userptr, ntudp_full_t &cb_ntudp_full
					, const ntudp_event_t &ntudp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << ntudp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( ntudp_event.is_full_ok() );
	// sanity check - here the ntudp_itor MUST be NULL
	DBG_ASSERT( ntudp_itor == NULL );

 	// backup the object_slotid of the ntudp_full_t - may be needed in case of stolen ntudp_full_t
 	slot_id_t	ntudp_full_slotid	= ntudp_full->get_object_slotid();
 
	// simply forward the event whatever the event type
	bool tokeep = notify_callback(ntudp_event);
	// if tokeep == 'dontkeep' compute ntudp_full_t tokeep in case it has been stolen during the cb
	if( !tokeep )	return object_slotid_tokeep(ntudp_full_slotid);
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    main function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	ntudp_client_t::notify_callback(const ntudp_event_t &ntudp_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_client_event_cb( userptr, *this, ntudp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;	
}

NEOIP_NAMESPACE_END

