/*! \file
    \brief Definition of the socket_echo_server_cnx_t class

*/

/* system include */
/* local include */
#include "neoip_socket_echo_server_cnx.hpp"
#include "neoip_socket_echo_server.hpp"
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
socket_echo_server_cnx_t::socket_echo_server_cnx_t(socket_echo_server_t *echo_server)	throw()
{
	// copy the parameter
	this->echo_server	= echo_server;
	// zero some field
	socket_full		= NULL;
	
	// link it to the socket_echo_server_t
	echo_server->cnx_dolink(this);
}

/** \brief Destructor
 */
socket_echo_server_cnx_t::~socket_echo_server_cnx_t()	throw()
{
	// unlink it from the socket_echo_server_t
	echo_server->cnx_unlink(this);
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_echo_server_cnx_t::start(socket_full_t *socket_full)	throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");

	// copy the parameter
	this->socket_full	= socket_full;
	// start the socket_full
	socket_err		= socket_full->start(this, NULL);	
	if( socket_err.failed() )	return socket_err;

	// return no error
	return socket_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when a connection is established
 */
bool	socket_echo_server_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() ){
		nipmem_delete	this;
		return false;
	}	
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::NEW_MTU:
			// this event MUST NOT happen as the mtu discovery is not on
			// fall thru
	case socket_event_t::MAYSEND_ON:
			// those events are for reliable socket_type_t only - so NOT on DGRAM
			// fall thru
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	socket_echo_server_cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);
	// just send back the received data
	socket_full->send( pkt );
	// return 'tokeep'
	return true;
}



NEOIP_NAMESPACE_END

