/*! \file
    \brief Definition of the socket_echo_server_t class

*/

/* system include */
/* local include */
#include "neoip_socket_echo_server.hpp"
#include "neoip_socket_echo_server_cnx.hpp"
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
socket_echo_server_t::socket_echo_server_t()	throw()
{
	// zero some field
	socket_resp	= NULL;
}

/** \brief Destructor
 */
socket_echo_server_t::~socket_echo_server_t()	throw()
{
	// delete all the established connection
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
	// delete the socket_resp_t if needed
	nipmem_zdelete	socket_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_echo_server_t::start(const socket_resp_arg_t &resp_arg)	throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");

	// init and start socket_resp_t
	socket_resp	= nipmem_new socket_resp_t();
	socket_err	= socket_resp->start(resp_arg, this, NULL);
	if( socket_err.failed() )	return socket_err;

	// return no error
	return socket_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_resp_t when a connection is established
 */
bool	socket_echo_server_t::neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( socket_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:{
			// spawn a socket_echo_server_cnx_t with the notified socket_full_t
			socket_full_t *	socket_full 	= socket_event.get_cnx_established();
			socket_echo_server_cnx_t *cnx	= nipmem_new socket_echo_server_cnx_t(this);
			socket_err_t	socket_err	= cnx->start(socket_full);
			if( socket_err.failed() )	nipmem_delete cnx;
			break;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END

