/*! \file
    \brief Declaration of the neoip_socket_ntudp_full
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_full_ntlay.hpp"
#include "neoip_socket_full_close_ntlay.hpp"
#include "neoip_socket_layer_ntlay.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_nlay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// include for the nlay_full_upapi_t
#include "neoip_nlay_scnx_full_api.hpp"

NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_full_vapi_t, socket_full_ntlay_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_full_ntlay_t::socket_full_ntlay_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero from field
	callback	= NULL;
	ntudp_full	= NULL;
	nlay_full	= NULL;

	// link this object to the socket_layer_ntlay_t
	socket_layer_ntlay_get()->socket_full_dolink(this);
}

/** \brief Destructor
 */
socket_full_ntlay_t::~socket_full_ntlay_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// link this object to the socket_layer_ntlay_t
	socket_layer_ntlay_get()->socket_full_unlink(this);

	// destroy internal structure
	nipmem_zdelete	nlay_full;
	nipmem_zdelete	ntudp_full;
}

/** \brief notify that the socket_glue has been destroyed
 * 
 * - called when the caller of the socket layer just destroyed the socket_full_t
 * - this function attachs the socket_full_ntlay_t to a socket_full_close_ntlay_t
 */
void socket_full_ntlay_t::notify_glue_destruction()		throw()
{
	// TODO here issue if the socket_full_ntlay_t has not yet been started
	// pass the socket_full_vapi_t in socket_full_close_t (up to it to destroy it when appropriate)
	nipmem_new socket_full_close_ntlay_t(this);
}

/** \brief start the closure 
 * 
 * - called only by the socket_full_close_ntlay_t
 */
void socket_full_ntlay_t::start_closure()	throw() 
{
	// forward the start_closure to nlay_full_t
	nlay_full->start_closure();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   set parameter
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
void	socket_full_ntlay_t::set_callback(socket_full_vapi_cb_t *callback, void *userptr)	throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
}

/** \brief set the parameter
 * 
 * - used only by socket_resp_ntlay_t and socket_itor_ntlay_t
 *   - TODO to make it private and add itor/resp in friend
 *   - why this stuff if not in the ctor itself ? usuaflly it is where i put it
 */
socket_err_t	socket_full_ntlay_t::set_ctor_param(ntudp_full_t *ntudp_full, nlay_full_t *nlay_full
					, const socket_profile_t &socket_profile)	throw()
{
	// copy the socket_profile_t
	m_socket_profile	= socket_profile;
	DBG_ASSERT( profile().check().succeed() );
	// copy the linger profile 
	// - needed to get linger information when initiating the closure on this socket
	this->linger_profile	= socket_profile.linger();
	
	this->ntudp_full	= ntudp_full;
	// TODO ISSUE with ntudp_full_t being already started when arriving here
	// - should i put the set_callback of nlay_full here ? likely
	// - and allow the local callback not to be set when receiving udp event
	this->nlay_full		= nlay_full;
	
	// return no error
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         Setup Fcuntion
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/** \brief Start the operation
 */
socket_err_t	socket_full_ntlay_t::start()					throw()
{
	nlay_err_t	nlay_err;
	ntudp_err_t	ntudp_err;
	// sanity check - ntudp_full and nlay_full MUST be set
	DBG_ASSERT( ntudp_full );
	DBG_ASSERT( nlay_full );
	DBG_ASSERT( callback );

	// TODO ISSUE with ntudp_full_t being already started when arriving here
	ntudp_full->set_callback(this, NULL);

	// start nlay_full
	nlay_err	= nlay_full->start(this, NULL);
	if( nlay_err.failed() )	return socket_err_from_nlay(nlay_err);

	// read the local_addr() once the socket is started
	m_local_addr	= socket_addr_t(domain().to_string() + "://" + ntudp_full->local_addr().to_string());
	DBG_ASSERT( !local_addr().is_null() );
	// read the remote_addr() once the socket is started
	m_remote_addr	= socket_addr_t(domain().to_string() + "://" + ntudp_full->remote_addr().to_string());
	DBG_ASSERT( !remote_addr().is_null() );
	// read the socket_type_t once the socket is started
	m_socket_type	= socket_type_from_nlay(nlay_full->get_inner_type());
	DBG_ASSERT( !type().is_null() );

	// enable mtu_pathdisc on outter connection IIF socket_type_t::is_stream() is true
	if( type().is_stream() )	ntudp_full->mtu_pathdisc(true);
	// notify nlay_full of its current outter mtu - thus it can propagate it thru nlay_full
	nlay_full->mtu_outter( ntudp_full->mtu_inner() );

	// return no error
	return socket_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   access to domain specific api
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to access socket_full_ntlay_t of this socket_full_ntlay_t
 */
const socket_full_ntlay_t &socket_full_ntlay_t::from_socket(const socket_full_t &socket_full)	throw()
{
	// sanity check - the socket_domain_t MUST be stcp
	DBG_ASSERT( socket_full.get_domain() == socket_domain_t::NTLAY );
	// return the socket_full_stcp_api_t
	return dynamic_cast <const socket_full_ntlay_t &>(*socket_full.full_vapi());
}

/** \brief to access socket_full_ntlay_t of this socket_full_ntlay_t
 */
socket_full_ntlay_t &	socket_full_ntlay_t::from_socket(socket_full_t &socket_full)	throw()
{
	// sanity check - the socket_domain_t MUST be stcp
	DBG_ASSERT( socket_full.get_domain() == socket_domain_t::NTLAY );
	// return the socket_full_stcp_api_t
	return dynamic_cast <socket_full_ntlay_t &>(*socket_full.full_vapi());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   nlay_scnx_full_api_t function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to access nlay_full_upapi_t of this socket_full_ntlay_t
 */
const nlay_full_upapi_t &	socket_full_ntlay_t::nlay()	const throw()
{
	// sanity check nlay_full MUST be non null
	DBG_ASSERT( nlay_full );
	// return the nlay_scnx_full
	return *nlay_full;
}

/** \brief to access nlay_full_upapi_t of this socket_full_ntlay_t
 */
nlay_full_upapi_t &	socket_full_ntlay_t::nlay()		throw()
{
	// sanity check nlay_full MUST be non null
	DBG_ASSERT( nlay_full );
	// return the nlay_scnx_full
	return *nlay_full;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ???
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief notify a fatal event
 * 
 * - TODO what the fuck is this function ?
 *   - it seems in relation with socket_full_udp_close
 */
bool	socket_full_ntlay_t::notify_fatal_event(socket_event_t socket_event)	throw()
{
	// sanity check - here the socket_event MUST be fatal
	DBG_ASSERT( socket_event.is_fatal() );
	// sanity check - only one fatal event MUST be reported
	if( typeid(*callback) != typeid(socket_full_close_ntlay_t))
		DBG_ASSERT( reported_fatal_event.get_value() == socket_event_t::NONE );

	// keep the reported fatal event for socket_full_close_ntlay_t 
	reported_fatal_event	= socket_event;

	// notify the event
	notify_callback(socket_event);
	// NOTE: here the callback 'tokeep' is ignored as the event is fatal
	// so the connection will still be used internally to complete the closure.
	// so the connection is kept in anycase. even if the caller will likely
	// destroy it
	return true;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         ntudp_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_full_t when to notify an event
 */
bool	socket_full_ntlay_t::neoip_ntudp_full_event_cb(void *userptr, ntudp_full_t &cb_ntudp_full
					, const ntudp_event_t &ntudp_event)		throw()
{
	nlay_err_t	nlay_err;
	socket_event_t	socket_event;
	// log to debug
	KLOG_DBG("enter event=" << ntudp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( ntudp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_CLOSED:{
			// build the event
			std::string	reason	= ntudp_event.get_cnx_closed_reason();
			return notify_fatal_event(socket_event_t::build_network_error(reason));}
	case ntudp_event_t::RECVED_DATA:{
			pkt_t *	pkt = ntudp_event.get_recved_data();
			KLOG_DBG("recved data=" << *pkt);
			// go thru nlay_full
			nlay_err = nlay_full->pkt_from_lower( *pkt );
			if( !nlay_err.succeed() ){
				KLOG_ERR("processing incoming udp packet thru nlay produced "<< nlay_err);
				break;
			}
			// if the packet has been swallowed, exit
			if( pkt->is_null() )	break;
			// do an event notification for the packet
			socket_event = socket_event_t::build_recved_data(pkt);
			return notify_callback(socket_event);}
	case ntudp_event_t::MTU_CHANGE:{
			size_t	new_outter_mtu	= ntudp_event.get_mtu_change();
			// notify nlay_full of new_outter_mtu - thus it can propagate it thru nlay_full
			nlay_full->mtu_outter( new_outter_mtu );
			// if the socket_type_t::is_datagram() is false, dont notify the caller
			if( !type().is_datagram() )	break;
			// notify the caller of the new mtu - updated to include nlay_full->mtu_overhead()
			size_t	new_inner_mtu	= new_outter_mtu - nlay_full->mtu_overhead();
			return notify_callback( socket_event_t::build_new_mtu(new_inner_mtu) );
			break;}
	default:	DBG_ASSERT( 0 );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         nlay_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref nlay_full_t when to notify an event
 */
bool	socket_full_ntlay_t::neoip_nlay_full_event_cb(void *userptr, nlay_full_t &cb_nlay_full
					, const nlay_event_t &nlay_event)	throw()
{
	nlay_err_t	nlay_err;
	socket_event_t	socket_event;
	KLOG_DBG("enter event=" << nlay_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( nlay_event.is_full_ok() );
	// sanity check - the ntudp_full and nlay_itor MUST NOT be null
	DBG_ASSERT( nlay_full );
	DBG_ASSERT( ntudp_full );
	
	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::IDLE_TIMEDOUT:{
			std::string	reason	= nlay_event.get_idle_timedout_reason();
			return notify_fatal_event(socket_event_t::build_idle_timedout(reason));}
	case nlay_event_t::REMOTE_PEER_UNREACHABLE:{
			std::string	reason	= nlay_event.get_remote_peer_unreachable_reason();
			return notify_fatal_event(socket_event_t::build_remote_peer_unreachable(reason));}
	case nlay_event_t::CNX_CLOSING:{
			std::string	reason	= nlay_event.get_cnx_closing_reason();
			return notify_fatal_event(socket_event_t::build_cnx_closed(reason));}
	case nlay_event_t::CNX_DESTROYED:{
			// sanity check - this event MUST be reported only to socket_full_close_ntlay_t
			DBG_ASSERT( typeid(*callback) == typeid(socket_full_close_ntlay_t));
			// notify a CNX_CLOSED 
			std::string	reason	= nlay_event.get_cnx_destroyed_reason();
			return notify_callback( socket_event_t::build_cnx_closed(reason) );}
	case nlay_event_t::PKT_TO_LOWER:
			// send the packet thru the ntudp_full
			ntudp_full->send( *nlay_event.get_pkt_to_lower() );
			break;
	case nlay_event_t::PKT_TO_UPPER:{
			// simply forward the event
			pkt_t *		pkt	= nlay_event.get_pkt_to_upper();
			return notify_callback( socket_event_t::build_recved_data(pkt) );}
	case nlay_event_t::MAYSEND_ON:
			// simply forward the event
			return notify_callback( socket_event_t::build_maysend_on() );
	default:	DBG_ASSERT( 0 );
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          recv_max_len function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void	socket_full_ntlay_t::rcvdata_maxlen(size_t new_value)	throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( type().is_reliable() );

// TODO here 
// set the recv_max_len in udp too ?
// - yes to prevent otsp socket from reading
// iif the recv_max_len is 0 else do 64k ?
	// forward the function to nlay_full_t
	nlay_err_t	nlay_err = nlay_full->recv_max_len_set(new_value);
	DBG_ASSERT( nlay_err.succeed() );
}

size_t	socket_full_ntlay_t::rcvdata_maxlen()			const throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_full->recv_max_len_get();
}

/** \brief write data on the \ref socket_full_ntlay_t
 */
size_t	socket_full_ntlay_t::send(const void *data_ptr, size_t data_len)	throw()
{
	nlay_err_t	nlay_err;
	size_t		sent_len;
	// build the packet
	pkt_t		pkt(data_ptr, data_len);
	// go thru nlay_full_t
	nlay_err	= nlay_full->pkt_from_upper( pkt );
	if( !nlay_err.succeed() ){
		KLOG_ERR("NLAAAAAAAAAAAAYYYYYYYYYYYYYYYY RETURNNNNNNNN in PKT_FROM_UPPER=" << nlay_err);
		return 0;
	}
	// send it thru ntudp_full_t
	sent_len	= ntudp_full->send(pkt.get_data(), pkt.get_len());
#if 0	// TMP:
	if( sent_len != pkt.get_len() ){
		DBG_ASSERT(0);	// TODO dunno what error to return
	}
#endif
	// return no error
	return sent_len;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			socket_stream_vapi_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	socket_full_ntlay_t::maysend_tshold(size_t new_value)	throw()
{
	// forward the function to nlay_full_t
	nlay_err_t	nlay_err;
	nlay_err	= nlay_full->maysend_set_threshold(new_value);
	DBG_ASSERT( nlay_err.succeed() );
}

size_t	socket_full_ntlay_t::maysend_tshold()			const throw()
{
	if( nlay_full->maysend_is_set() )	return socket_full_t::UNLIMITED;
	return nlay_full->maysend_get_threshold();
}

void	socket_full_ntlay_t::xmitbuf_maxlen(size_t new_value)	throw()
{
	nlay_err_t	nlay_err;
	nlay_err	= nlay_full->sendbuf_set_max_len(new_value);
	DBG_ASSERT( nlay_err.succeed() );
}

size_t	socket_full_ntlay_t::xmitbuf_maxlen()			const throw()
{
	if( nlay_full->sendbuf_is_limited() )	return socket_full_t::UNLIMITED;
	return nlay_full->sendbuf_get_max_len();
}

size_t	socket_full_ntlay_t::xmitbuf_usedlen()			const throw()
{
	return nlay_full->sendbuf_get_used_len();
}

size_t	socket_full_ntlay_t::xmitbuf_freelen()			const throw()
{
	if( nlay_full->sendbuf_is_limited() )	return socket_full_t::UNLIMITED;
	return nlay_full->sendbuf_get_free_len();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			socket_mtu_vapi_t implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	socket_full_ntlay_t::mtu_pathdisc(bool onoff)	throw()
{
	ntudp_full->mtu_pathdisc(onoff);	
}

bool	socket_full_ntlay_t::mtu_pathdisc()		const throw()
{
	return ntudp_full->mtu_pathdisc();
}

size_t	socket_full_ntlay_t::mtu_overhead()		const throw()
{
	return ntudp_full->mtu_overhead() + nlay_full->mtu_overhead();
}

size_t	socket_full_ntlay_t::mtu_outter()		const throw()
{
	return ntudp_full->mtu_outter();
}

size_t	socket_full_ntlay_t::mtu_inner()		const throw()
{
	return ntudp_full->mtu_outter() - mtu_overhead();	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_full_ntlay_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "connected";
	oss << " in "	<< type();
	oss << " from "	<< local_addr();
	oss << " to "	<< remote_addr();
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	socket_full_ntlay_t::notify_callback(const socket_event_t &socket_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_full_vapi_cb(userptr, *this, socket_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END




