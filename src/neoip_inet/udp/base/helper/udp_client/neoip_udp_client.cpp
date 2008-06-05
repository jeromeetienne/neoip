/*! \file
    \brief Definition of the udp_client_t
*/


/* system include */
/* local include */
#include "neoip_udp_client.hpp"
#include "neoip_udp.hpp"
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
udp_client_t::udp_client_t() throw()
{
	// init some field
	udp_itor	= nipmem_new udp_itor_t();
	udp_full	= NULL;
	callback	= NULL;
}

/** \brief Desstructor
 */
udp_client_t::~udp_client_t() throw()
{
	// delete udp_itor_t if needed
	nipmem_zdelete	udp_itor;
	// delete udp_full_t if needed
	nipmem_zdelete	udp_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Setup functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
inet_err_t	udp_client_t::set_callback(udp_client_cb_t *callback, void * userptr)	throw()
{
	// copy the callback field
	this->callback	= callback;
	this->userptr	= userptr;
	// set the itor callback
	inet_err_t	inet_err	= udp_itor->set_callback(this, NULL);
	if( inet_err.failed() )		return inet_err;
	// return no error
	return inet_err_t::OK;
}

/** \brief Setup all mandatory parameters and start the operation
 */
inet_err_t	udp_client_t::start(const ipport_addr_t &remote_addr, udp_client_cb_t *callback
						, void * userptr)		throw()
{
	inet_err_t	inet_err;
	// set the remote_addr
	inet_err	= set_remote_addr(remote_addr);
	if( inet_err.failed() )	return inet_err;
	// set the callback
	inet_err	= set_callback(callback, userptr);
	if( inet_err.failed() )	return inet_err;
	// start the operation now
	return start();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       steal_full()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Steal the udp_full_t from a established udp_client_t.
 * 
 * - After this call, the caller is MUST delete the udp_client_t
 * - clearly a 'caller MUST know what he is doing' function 
 * 
 * @return the udp_full_t pointer
 */
udp_full_t *	udp_client_t::steal_full()	throw()
{
	// sanity check - udp_full_t MUST be non-null
	// mark a copy the udp_full
	udp_full_t *	udp_full_copy = udp_full;
	// set the callback to NULL 
	// - protective measure to ensure it wont call this udp_client_t without noticing
	udp_full_copy->set_callback(NULL, NULL);
	// mark udp_full as unused
	udp_full	= NULL;
	// return the copy
	return udp_full_copy;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     to_string function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	udp_client_t::to_string()					const throw()
{
	if( udp_itor )		return udp_itor->to_string();
	else if(udp_full)	return udp_full->to_string();
	return "NONE";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     initiator callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_itor_t when a connection is established
 */
bool	udp_client_t::neoip_inet_udp_itor_event_cb(void *userptr, udp_itor_t &cb_udp_itor
					, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( udp_event.is_itor_ok() );

	// delete udp_itor and mark it unused
	nipmem_zdelete	udp_itor;

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:
			recv_cnx_established(udp_event.get_cnx_established());
			return false;
	default:	DBG_ASSERT(0);
	}

	// return 'dontkeep' - as it has been deleted
	return false;
}

/** \brief handle the reception of a udp_event_t::CNX_ESTABLISHED from itor
 */
bool	udp_client_t::recv_cnx_established(udp_full_t *udp_full)	throw()
{
	inet_err_t	inet_err;
	udp_event_t	udp_event;
	// copy the udp_full_t pointer
	this->udp_full = udp_full;
	// set the callback
	inet_err = udp_full->set_callback(this, NULL);
	if( inet_err.failed() )	goto error;
	// start the udp_full
	inet_err = udp_full->start();
	if( inet_err.failed() )	goto error;

	// logging to debug
	KLOG_DBG("create a full udp " << *udp_full);
	
	// notify a CNX_ESTABLISHED with a NULL pointer
	// - the NULL pointer is required as the udp_full is already contained in the udp_client_t
	udp_event = udp_event_t::build_cnx_established(NULL);
	// notify the event
	return notify_callback(udp_event);
	
error:;	KLOG_ERR("Unable to init udp_full due to " << inet_err );
	// delete udp_full and mark it unused
	nipmem_zdelete	udp_full;
	// TODO: solve this issue
	// - udp_event has no real cnx_refused as it has no connection establishement over the wire
	// - so how to notify this error ?
	// - note that this error is currently quite theorical - doing a ASSERT( 0 ) until it 
	//   a solution is found
	DBG_ASSERT( 0 );
	// notify a CNX_REFUSED
	udp_event = udp_event_t::build_cnx_refused("Unable to init udp_full due to "+ inet_err.to_string());
	// notify the event
	notify_callback(udp_event);
	// return dontkeep - as the udp_full_t has just been deleted
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     udp_full callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	udp_client_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
					, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );
	// sanity check - here the udp_itor MUST be NULL
	DBG_ASSERT( udp_itor == NULL );

 	// backup the object_slotid of the udp_full_t - may be needed in case of stolen udp_full_t
 	slot_id_t	udp_full_slotid	= udp_full->get_object_slotid();
 
	// simply forward the event whatever the event type
	bool tokeep = notify_callback(udp_event);
	// if tokeep == 'dontkeep' compute the udp_full_t tokeep as is may have stolen during the cb
	if( !tokeep )	return object_slotid_tokeep(udp_full_slotid);
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        function to notify caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	udp_client_t::notify_callback(const udp_event_t &udp_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_udp_client_event_cb( userptr, *this, udp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;	
}

NEOIP_NAMESPACE_END

