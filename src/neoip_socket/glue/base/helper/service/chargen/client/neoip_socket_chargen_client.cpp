/*! \file
    \brief Definition of the unit test for the \ref socket_client_t

*/

/* system include */
/* local include */
#include "neoip_socket_chargen_client.hpp"
#include "neoip_socket_client.hpp"
#include "neoip_socket.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_chargen_client_t::socket_chargen_client_t()	throw()
{
	// zero some field
	socket_client		= NULL;
	recv_curlen		= 0;
}

/** \brief Destructor
 */
socket_chargen_client_t::~socket_chargen_client_t()	throw()
{
	// destruct the socket client
	nipmem_zdelete socket_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_chargen_client_t::start(const socket_itor_arg_t &itor_arg, size_t recv_maxlen
				, socket_chargen_client_cb_t *callback, void *userptr)	throw()
{
	socket_err_t	socket_err;
	// copy the parameter
	this->recv_maxlen	= recv_maxlen;
	this->callback		= callback;
	this->userptr		= userptr;

	// init and start socket_client_t
	socket_client	= nipmem_new socket_client_t();
	socket_err	= socket_client->start(itor_arg, this, NULL);
	if( socket_err.failed() )	return socket_err;
	
	// return no error
	return socket_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_client_t to provide event
 */
bool	socket_chargen_client_t::neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( socket_event.is_client_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() )	return notify_callback(socket_err_t::ERROR);

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:
			// log to debug
			KLOG_ERR("Connection established");
			// return tokeep
			return true;
	case socket_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::NEW_MTU:
			// this event MUST NOT happen as the MTU discovery is not on
			// fall thru
	case socket_event_t::MAYSEND_ON:
	default:	DBG_ASSERT(0);
	}
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   packet reception
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the socket_client_t
 */
bool	socket_chargen_client_t::handle_recved_data(pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_ERR("enter pkt size=" << pkt.size());
	// update the recv_curlen
	recv_curlen	+= pkt.size();
	// check if it is completed
	if( recv_maxlen && recv_curlen >= recv_maxlen )	return notify_callback(socket_err_t::OK);

#if 0
	// to display the output
	KLOG_STDOUT(std::string((char *)pkt.get_ptr(), pkt.get_len()));
#endif
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
bool socket_chargen_client_t::notify_callback(const socket_err_t &socket_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_chargen_client_cb(userptr, *this, socket_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}




NEOIP_NAMESPACE_END

