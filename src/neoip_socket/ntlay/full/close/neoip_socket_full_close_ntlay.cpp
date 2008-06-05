/*! \file
    \brief Definition of the socket_full_close_ntlay_t

\par TODO issue with the linger
- how to pass the linger parameter from the socket_profile to the socket_full_close_ntlay_t ?
- how to know if the socket is itor in the closure ?
  - maybe to look at the event received by socket_full_ntlay_t
  - if nlay already received closed, it is responder
  - what about closing/closed ?
  - what about the CLOSE_NOW, is it different than CLOSE_REQUEST ?
- how to handle the state internally to socket_full_close_ntlay_t

\par TODO
- if the socket has been closed due to a remote peer unreachable or to an otsp
  close,
  - useless to do the closure protocol
  - same for the linger
  
- the idle_timedout doesnt change anything both are made anyway
    
*/


/* system include */
/* local include */
#include "neoip_socket_full_close_ntlay.hpp"
#include "neoip_socket_full_ntlay.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_layer_ntlay.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_log.hpp"

#include "neoip_socket_linger_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_full_close_ntlay_t::socket_full_close_ntlay_t(socket_full_ntlay_t *socket_full_ntlay) throw()
{
	socket_err_t	socket_err;
	// lot to debug
	KLOG_DBG("CREATE CLOSE for " << socket_full_ntlay->to_string());
	// copy the parameter
	this->socket_full_ntlay	= socket_full_ntlay;
	// register this to the ntudp_peer_t deletion	
	// - NOTE: this is used in case of the ntudp_peer_t being deleted during the closure of this
	//         socket_full_ntlay_t. in this case, the closure is done ungracefully
	ntudp_peer_t *	ntudp_peer	= socket_full_ntlay->ntudp_full->get_ntudp_peer();
	ntudp_peer->event_hook_append(ntudp_peer_t::HOOK_PRE_PEER_DELETION, this, NULL);
	
	// link this object to the socket_layer_ntlay_t
	socket_layer_ntlay_get()->full_close_dolink(this);
	// redirect the event callback to this object
	// - thus the socket user wont receive socket_event_t from an obsolete socket
	socket_full_ntlay->set_callback(this, NULL);

	// if the fatal event is REMOTE_PEER_UNREACHABLE or NETWORK_ERROR, the remote peer is 
	// already unreachable so no need to do a linger or a closure.
	socket_event_t	&fatal_event	 = socket_full_ntlay->reported_fatal_event;
	if( fatal_event.is_remote_peer_unreachable() || fatal_event.is_network_error() ){
		// log to debug
		KLOG_DBG("NOCLOSURE/LINGER destroy it immediatly");
		// zerotimer with a autodelete via zerotimer
		// - to avoid delete an object in its constructor
		autodelete_zerotimer.append(this, NULL);
		return;
	}

	if( should_linger() )	start_linger();
	else			start_closure();
}

/** \brief Desstructor
 */
socket_full_close_ntlay_t::~socket_full_close_ntlay_t() throw()
{
	// log to debug
	KLOG_ERR("DESTROY CLOSE for " << socket_full_ntlay->to_string());
	// unlink this object from the socket_layer_ntlay_t
	socket_layer_ntlay_get()->full_close_unlink(this);
	// unregister this to the ntudp_peer_t deletion	
	ntudp_peer_t *	ntudp_peer	= socket_full_ntlay->ntudp_full->get_ntudp_peer();
	ntudp_peer->event_hook_remove(ntudp_peer_t::HOOK_PRE_PEER_DELETION, this, NULL);	
	// destroy the socket_full_vapi_t thru the object factory
	socket_full_ntlay->destroy();
}

/** \brief start the linger on this socket
 */
void	socket_full_close_ntlay_t::start_closure()		throw()
{
	// log to debug
	KLOG_ERR("START CLOSURE");
	// sanity check - the should_linger() MUST be false
	DBG_ASSERT( should_linger() == false );
	DBG_ASSERT( linger_timeout.is_running() == false );
	
	// set the state
	state	= CLOSING;
	
	// pass the socket_full_ntlay into closure
	socket_full_ntlay->start_closure();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_pserver_pool_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 * 
 * @return a 'tokeep' for the event_hook_t object
 */
bool	socket_full_close_ntlay_t::neoip_event_hook_notify_cb(void *userptr
					, const event_hook_t *cb_event_hook, int hook_level)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// autodelete
	nipmem_delete this;
	// return tokeep
	return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       autodelete_zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	socket_full_close_ntlay_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	nipmem_delete this;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		linger management function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if the linger should be done on this socket, false otherwise
 */
bool	socket_full_close_ntlay_t::should_linger()	const throw()
{
	socket_event_t	&fatal_event	 = socket_full_ntlay->reported_fatal_event;
	// log to debug
	KLOG_DBG("closure_resp=" << socket_full_ntlay->reported_fatal_event);
	KLOG_DBG("linger onoff=" << socket_full_ntlay->linger_profile.onoff());
	KLOG_DBG("linger timeout=" << socket_full_ntlay->linger_profile.timeout());
	
	// dont linger on non reliable socket type
	if( socket_full_ntlay->type().is_reliable() == false )	return false;
	
	// dont linger if socket_full is responder into the closure
	if( fatal_event.is_cnx_closed() )				return false;

	// sanity check
	// only 'spontaneous close' (aka fatal_event is none) and idle_timedout may linger
	DBG_ASSERT( fatal_event.get_value() == socket_event_t::NONE || fatal_event.is_idle_timedout());
	// when it is impossible to reach the other peer, those event should be routed before here
	DBG_ASSERT( !fatal_event.is_remote_peer_unreachable() );
	DBG_ASSERT( !fatal_event.is_network_error() );
	
	// dont linger if linger is not set on this socket
	if( socket_full_ntlay->linger_profile.onoff() == false )		return false;

	// dont linger if sendbuf is already empty
	if( socket_full_ntlay->xmitbuf_usedlen() == 0 )		return false;
	
	return true;
}

/** \brief start the linger on this socket
 */
void	socket_full_close_ntlay_t::start_linger()		throw()
{
	socket_err_t	socket_err;
	// sanity check - the should_linger() MUST be true
	DBG_ASSERT( should_linger() );
	// log to debug
	KLOG_ERR("START LINGER");
	// set the state
	state	= LINGERING;
	// set the sendbuf_max_len to the used len
	// - thus sendbuf_max_len is set anyway (as it may not be set on a socket)
	socket_full_ntlay->xmitbuf_maxlen(socket_full_ntlay->xmitbuf_usedlen());
	// set the maysend_threshold to the xmitbuf_maxlen
	// - thus MAYSEND_ON will be notified when the sendbuf will be empty
	socket_full_ntlay->maysend_tshold(socket_full_ntlay->xmitbuf_maxlen());
	// start the linger timeout
	linger_timeout.start(socket_full_ntlay->linger_profile.timeout(), this, NULL);
}

/** \brief stop the linger on this socket
 */
void	socket_full_close_ntlay_t::stop_linger()		throw()
{
	// Check the state
	DBG_ASSERT( state == LINGERING );
	// stop the linger timeout
	linger_timeout.stop();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       	linger_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	socket_full_close_ntlay_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// sanity check - this function MUST be called only when the linger_timeout expire
	DBG_ASSERT( &cb_timeout == &linger_timeout );
	// stop the linger process
	stop_linger();
	// start the closure process
	start_closure();
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    	socket_full_ntlay_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief to receive the event from socket_full_t
 */
bool socket_full_close_ntlay_t::neoip_socket_full_vapi_cb(void *cb_userptr, socket_full_vapi_t &cb_full_vapi
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("socket_event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// if the socket event is fatal, destroy the socket_full_close_ntlay_t
	if( socket_event.is_fatal() ){
		nipmem_delete this;
		return false;
	}
	
	// handle the event according to its type
	switch( socket_event.get_value() ){
	case socket_event_t::MAYSEND_ON:
			// This event should be notified here IIF state is LINGERING
			DBG_ASSERT( state == LINGERING );
			// if a linger is in progress, this event must be only when sendbuf is empty
			DBG_ASSERT( socket_full_ntlay->xmitbuf_usedlen() == 0 );
			// stop the linger process
			stop_linger();
			// start the closure process
			start_closure();	
			break;
	default:	break;
	}
	return true;
}

NEOIP_NAMESPACE_END

