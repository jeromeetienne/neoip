/*! \file
    \brief Definition of the tcp_client_t
*/


/* system include */
/* local include */
#include "neoip_tcp_client.hpp"
#include "neoip_tcp.hpp"
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
tcp_client_t::tcp_client_t() throw()
{
	// init some field
	tcp_itor	= nipmem_new tcp_itor_t();
	tcp_full	= NULL;
	callback	= NULL;
}

/** \brief Desstructor
 */
tcp_client_t::~tcp_client_t() throw()
{
	// delete tcp_itor_t if needed
	nipmem_zdelete	tcp_itor;
	// delete tcp_full_t if needed
	nipmem_zdelete	tcp_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Setup functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

tcp_client_t &	tcp_client_t::set_local_addr(const ipport_addr_t &local_addr)		throw()
{
	// sanity check - the tcp_client_t MUST be in itor 
	DBG_ASSERT( is_itor() );
	// set the local_addr of the tcp_itor_t
	tcp_itor->set_local_addr(local_addr);
	// return the object itself
	return *this;
}

/** \brief Setup all mandatory parameters and start the operation
 */
inet_err_t	tcp_client_t::start(const ipport_addr_t &remote_addr, tcp_client_cb_t *callback
						, void * userptr)		throw()
{
	inet_err_t	inet_err;
	// copy the callback field
	this->callback	= callback;
	this->userptr	= userptr;
	// set the itor callback
	inet_err	= tcp_itor->start(remote_addr, this, NULL);
	if( inet_err.failed() )		return inet_err;	
	// return no error
	return inet_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       steal_full()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Steal the tcp_full_t from a established tcp_client_t.
 * 
 * - After this call, the caller is MUST delete the tcp_client_t
 * - clearly a 'caller MUST know what he is doing' function 
 * 
 * @return the tcp_full_t pointer
 */
tcp_full_t *	tcp_client_t::steal_full()	throw()
{
	// sanity check - tcp_full_t MUST be non-null
	// mark a copy the tcp_full
	tcp_full_t *	tcp_full_copy = tcp_full;
	// set the callback to NULL 
	// - protective measure to ensure it wont call this tcp_client_t without noticing
	tcp_full_copy->set_callback(NULL, NULL);
	// mark tcp_full as unused
	tcp_full	= NULL;
	// return the copy
	return tcp_full_copy;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     initiator callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_itor_t when a connection is established
 */
bool	tcp_client_t::neoip_tcp_itor_event_cb(void *userptr, tcp_itor_t &cb_tcp_itor
					, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( tcp_event.is_itor_ok() );

	// delete tcp_itor and mark it unused
	nipmem_zdelete	tcp_itor;

	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:
			recv_cnx_established(tcp_event.get_cnx_established());
			return false;
	case tcp_event_t::CNX_REFUSED:
			// just forward the event
			notify_callback( tcp_event );
			return false;
	default:	DBG_ASSERT(0);
	}

	// return 'dontkeep' - as it has been deleted
	return false;
}

/** \brief handle the reception of a tcp_event_t::CNX_ESTABLISHED from itor
 */
bool	tcp_client_t::recv_cnx_established(tcp_full_t *tcp_full)	throw()
{
	inet_err_t	inet_err;
	// sanity check - the tcp_client_t MUST be in itor
	DBG_ASSERT( this->tcp_full == NULL );
	// copy the tcp_full_t pointer
	this->tcp_full = tcp_full;
	// start the tcp_full
	inet_err	= tcp_full->start(this, NULL);
	if( inet_err.failed() ){
		std::string	reason	= "Unable to init tcp_full due to "+ inet_err.to_string();
		return notify_callback( tcp_event_t::build_cnx_refused(reason) );		
	}
	
	// notify a CNX_ESTABLISHED with a NULL pointer
	// - the NULL pointer is required as the tcp_full is already contained in the tcp_client_t
	return notify_callback( tcp_event_t::build_cnx_established(NULL) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     tcp_full callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_full_t when to notify an event
 */
bool	tcp_client_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
					, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( tcp_event.is_full_ok() );
	// sanity check - here the tcp_itor MUST be NULL
	DBG_ASSERT( tcp_itor == NULL );

 	// backup the object_slotid of the tcp_full_t - may be needed in case of stolen tcp_full_t
 	slot_id_t	tcp_full_slotid	= tcp_full->get_object_slotid();
 
	// simply forward the event whatever the event type
	bool tokeep = notify_callback(tcp_event);
	// if tokeep == 'dontkeep' compute the tcp_full_t tokeep as is may have stolen during the cb
	if( !tokeep )	return object_slotid_tokeep(tcp_full_slotid);
	// return 'tokeep'
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	tcp_client_t::to_string()					const throw()
{
	if( tcp_itor )		return tcp_itor->to_string();
	else if(tcp_full)	return tcp_full->to_string();
	return "null";
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        function to notify caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	tcp_client_t::notify_callback(const tcp_event_t &tcp_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_tcp_client_event_cb(userptr, *this, tcp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;	
}

NEOIP_NAMESPACE_END

