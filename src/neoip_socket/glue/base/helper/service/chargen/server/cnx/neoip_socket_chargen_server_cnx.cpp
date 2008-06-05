/*! \file
    \brief Definition of the socket_chargen_server_cnx_t class

*/

/* system include */
/* local include */
#include "neoip_socket_chargen_server_cnx.hpp"
#include "neoip_socket_chargen_server.hpp"
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
socket_chargen_server_cnx_t::socket_chargen_server_cnx_t(socket_chargen_server_t *chargen_server)	throw()
{
	// copy the parameter
	this->chargen_server	= chargen_server;
	// zero some field
	socket_full		= NULL;
	
	// link it to the socket_chargen_server_t
	chargen_server->cnx_dolink(this);
}

/** \brief Destructor
 */
socket_chargen_server_cnx_t::~socket_chargen_server_cnx_t()	throw()
{
	// unlink it from the socket_chargen_server_t
	chargen_server->cnx_unlink(this);
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
socket_err_t	socket_chargen_server_cnx_t::start(socket_full_t *socket_full)	throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");

	// copy the parameter
	this->socket_full	= socket_full;
	// start the socket_full
	socket_err		= socket_full->start(this, NULL);	
	if( socket_err.failed() )	return socket_err;

	// dont read anything on this tcp_full
	counter		= 0;
	socket_full->sendbuf_set_max_len( 50*1024 );
	fill_sendbuf();	

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
bool	socket_chargen_server_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
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
	case socket_event_t::RECVED_DATA:	nipmem_delete	this;
						return false;
	case socket_event_t::MAYSEND_ON:	fill_sendbuf();
						break;
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         fill_sendbuf
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief full the socket buffer
 */
void socket_chargen_server_cnx_t::fill_sendbuf()			throw()
{
	socket_err_t	socket_err;
	size_t		len = 70;
	while( socket_full->sendbuf_get_free_len() >= len ){
		std::string	str;
		size_t		i;
		// build the string to send
		for( str="", i = std::string("\n").size(); i < len; i++ ){
			char	c = 'a' + (i + counter) % 26;
			str += c;
		}
		str += "\n";
		DBG_ASSERT( str.size() == len );
		// send the data to the connection
		socket_err = socket_full->send( str.c_str(), len );
		DBG_ASSERT( socket_err.succeed() );	// TODO poor error management
		// log to debug
		KLOG_DBG("write " << len << "-byte. now sendbuf_free_len=" << socket_full->sendbuf_get_free_len() );
		// increase the counter
		counter++;
	}
	// setup the maysend limit
	socket_full->maysend_set_threshold( socket_full->sendbuf_get_max_len()/2 );
	// log to debug
	KLOG_DBG("stopped to write with sendbuf_free_len=" << socket_full->sendbuf_get_free_len() );
	KLOG_DBG("next write bunch when maysend_threshold=" << socket_full->maysend_get_threshold() );
}

NEOIP_NAMESPACE_END

