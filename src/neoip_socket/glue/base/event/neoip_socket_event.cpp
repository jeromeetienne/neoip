/*! \file
    \brief Implementation of the socket_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_socket_event.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
socket_event_t::socket_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
socket_event_t::~socket_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const socket_event_t &event) throw()
{
	switch( event.type_val ){
	case socket_event_t::NONE:		os << "NONE";		break;
	case socket_event_t::CNX_REFUSED:
		os << "CNX_REFUSED (reason: " << event.get_cnx_refused_reason() << ")";
		break;
	case socket_event_t::CNX_CLOSED:	
		os << "CNX_CLOSED (reason: " << event.get_cnx_closed_reason() << ")";
		break;
	case socket_event_t::IDLE_TIMEDOUT:	
		os << "IDLE_TIMEDOUT (reason: " << event.get_idle_timedout_reason() << ")";
		break;
	case socket_event_t::REMOTE_PEER_UNREACHABLE:	
		os << "REMOTE_PEER_UNREACHABLE (reason: " << event.get_remote_peer_unreachable_reason() << ")";
		break;
	case socket_event_t::NETWORK_ERROR:	
		os << "NETWORK_ERROR (reason: " << event.get_network_error_reason() << ")";
		break;
	case socket_event_t::CNX_ESTABLISHED:
// TODO disabled as socket_full_t isnt yet coded
//		os << "CNX_ESTABLISHED (cnx: " << *(event.get_cnx_established()) << ")";
		os << "CNX_ESTABLISHED";
		break;	
	case socket_event_t::RECVED_DATA:
		os << "RECVED_DATA (" << (event.get_recved_data())->get_len() << "-byte)";
		break;	
	case socket_event_t::MAYSEND_ON:
		os << "MAYSEND_ON";
		break;	
	case socket_event_t::NEW_MTU:
		os << "NEW_MTU (mtu=" << event.get_new_mtu() << "-byte)";
		break;
	default: 	DBG_ASSERT(0);
	}
	return os;
}

/** \brief Convert the object to a std::string
 */
std::string	socket_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << *this;
	// return the just built string
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_REFUSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_REFUSED, false otherwise
 */
bool	socket_event_t::is_cnx_refused() const throw()
{
	return type_val == CNX_REFUSED;
}

/** \brief build a socket_event_t to CNX_REFUSED (with a possible reason)
 */
socket_event_t socket_event_t::build_cnx_refused(const std::string &reason )	throw()
{
	socket_event_t	socket_event;
	// set the type_val
	socket_event.type_val	= CNX_REFUSED;
	socket_event.reason	= reason;
	// return the built object
	return socket_event;
}

/** \brief return the CNX_REFUSED reason
 */
const std::string &socket_event_t::get_cnx_refused_reason() const throw()
{
	// sanity check - the event MUST be CNX_REFUSED
	DBG_ASSERT( is_cnx_refused() );
	// return the reason
	return reason;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_CLOSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_CLOSED, false otherwise
 */
bool	socket_event_t::is_cnx_closed() const throw()
{
	return type_val == CNX_CLOSED;
}

/** \brief build a socket_event_t to CNX_CLOSED (with a possible reason)
 */
socket_event_t socket_event_t::build_cnx_closed(const std::string &reason )	throw()
{
	socket_event_t	socket_event;
	// set the type_val
	socket_event.type_val	= CNX_CLOSED;
	socket_event.reason	= reason;
	// return the built object
	return socket_event;
}

/** \brief return the CNX_CLOSED reason
 */
const std::string &socket_event_t::get_cnx_closed_reason() const throw()
{
	// sanity check - the event MUST be CNX_CLOSED
	DBG_ASSERT( is_cnx_closed() );
	// return the reason
	return reason;	
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    IDLE_TIMEDOUT
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is IDLE_TIMEDOUT, false otherwise
 */
bool	socket_event_t::is_idle_timedout() const throw()
{
	return type_val == IDLE_TIMEDOUT;
}

/** \brief build a socket_event_t to IDLE_TIMEDOUT (with a possible reason)
 */
socket_event_t socket_event_t::build_idle_timedout(const std::string &reason )	throw()
{
	socket_event_t	socket_event;
	// set the type_val
	socket_event.type_val	= IDLE_TIMEDOUT;
	socket_event.reason	= reason;
	// return the built object
	return socket_event;
}

/** \brief return the IDLE_TIMEDOUT reason
 */
const std::string &socket_event_t::get_idle_timedout_reason() const throw()
{
	// sanity check - the event MUST be IDLE_TIMEDOUT
	DBG_ASSERT( is_idle_timedout() );
	// return the reason
	return reason;	
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    REMOTE_PEER_UNREACHABLE
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is REMOTE_PEER_UNREACHABLE, false otherwise
 */
bool	socket_event_t::is_remote_peer_unreachable() const throw()
{
	return type_val == REMOTE_PEER_UNREACHABLE;
}

/** \brief build a socket_event_t to REMOTE_PEER_UNREACHABLE (with a possible reason)
 */
socket_event_t socket_event_t::build_remote_peer_unreachable(const std::string &reason )	throw()
{
	socket_event_t	socket_event;
	// set the type_val
	socket_event.type_val	= REMOTE_PEER_UNREACHABLE;
	socket_event.reason	= reason;
	// return the built object
	return socket_event;
}

/** \brief return the REMOTE_PEER_UNREACHABLE reason
 */
const std::string &socket_event_t::get_remote_peer_unreachable_reason() const throw()
{
	// sanity check - the event MUST be REMOTE_PEER_UNREACHABLE
	DBG_ASSERT( is_remote_peer_unreachable() );
	// return the reason
	return reason;	
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    NETWORK_ERROR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is NETWORK_ERROR, false otherwise
 */
bool	socket_event_t::is_network_error() const throw()
{
	return type_val == NETWORK_ERROR;
}

/** \brief build a socket_event_t to NETWORK_ERROR (with a possible reason)
 */
socket_event_t socket_event_t::build_network_error(const std::string &reason )	throw()
{
	socket_event_t	socket_event;
	// set the type_val
	socket_event.type_val	= NETWORK_ERROR;
	socket_event.reason	= reason;
	// return the built object
	return socket_event;
}

/** \brief return the NETWORK_ERROR reason
 */
const std::string &socket_event_t::get_network_error_reason() const throw()
{
	// sanity check - the event MUST be NETWORK_ERROR
	DBG_ASSERT( is_network_error() );
	// return the reason
	return reason;	
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_ESTABLISHED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_ESTABLISHED, false otherwise
 */
bool	socket_event_t::is_cnx_established() const throw()
{
	return type_val == CNX_ESTABLISHED;
}

/** \brief Build a socket_event_t to CNX_ESTABLISHED
 */
socket_event_t socket_event_t::build_cnx_established( socket_full_t *socket_full ) 
						throw()
{
	socket_event_t	socket_event;
	// set the type_val
	socket_event.type_val		= CNX_ESTABLISHED;
	socket_event.socket_full	= socket_full;
	// return the built object
	return socket_event;
}

/** \brief return the socket_full_t when CNX_ESTABLISHED
 */
socket_full_t *socket_event_t::get_cnx_established() const throw()
{
	// sanity check - the event MUST be CNX_ESTABLISHED
	DBG_ASSERT( is_cnx_established() );
	// return the socket_full
	return socket_full;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    RECVED_DATA
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is RECVED_DATA, false otherwise
 */
bool	socket_event_t::is_recved_data() const throw()
{
	return type_val == RECVED_DATA;
}

/** \brief Build a socket_event_t to RECVED_DATA
 */
socket_event_t socket_event_t::build_recved_data( pkt_t *pkt ) 			throw()
{
	socket_event_t	socket_event;
	// set the type_val
	socket_event.type_val	= RECVED_DATA;
	socket_event.pkt_ptr	= pkt;
	// return the built object
	return socket_event;
}

/** \brief return the neoip_socket_full when RECVED_DATA
 */
pkt_t *socket_event_t::get_recved_data() 					const throw()
{
	// sanity check - the event MUST be RECVED_DATA
	DBG_ASSERT( is_recved_data() );
	// return the datum_ptr
	return pkt_ptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    MAYSEND_ON
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is MAYSEND_ON, false otherwise
 */
bool	socket_event_t::is_maysend_on() const throw()
{
	return type_val == MAYSEND_ON;
}

/** \brief Build a socket_event_t to MAYSEND_ON
 */
socket_event_t socket_event_t::build_maysend_on() throw()
{
	socket_event_t	socket_event;
	// set the type_val
	socket_event.type_val		= MAYSEND_ON;
	// return the built object
	return socket_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    NEW_MTU
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is NEW_MTU, false otherwise
 */
bool	socket_event_t::is_new_mtu()						const throw()
{
	return type_val == NEW_MTU;
}

/** \brief Build a socket_event_t to NEW_MTU
 */
socket_event_t socket_event_t::build_new_mtu(size_t mtu_size) 			throw()
{
	socket_event_t	inet_socket_event;
	// set the type_val
	inet_socket_event.type_val		= NEW_MTU;
	inet_socket_event.mtu_size		= mtu_size;
	// return the built object
	return inet_socket_event;
}

/** \brief return the udp_full_t when NEW_MTU
 */
size_t	socket_event_t::get_new_mtu()						const throw()
{
	// sanity check - the event MUST be NEW_MTU
	DBG_ASSERT( is_new_mtu() );
	// return the pkt_ptr
	return mtu_size;
}
NEOIP_NAMESPACE_END

