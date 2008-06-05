/*! \file
    \brief Implementation of the nlay_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
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
nlay_event_t::nlay_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
nlay_event_t::~nlay_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const nlay_event_t &event) throw()
{
	switch( event.type_val ){
	case nlay_event_t::NONE:
		os << "NONE";	
		break;
	case nlay_event_t::CNX_REFUSED:
		os << "CNX_REFUSED (reason: " << event.get_cnx_refused_reason() << ")";
		break;
	case nlay_event_t::CNX_CLOSING:
		os << "CNX_CLOSING (reason: " << event.get_cnx_closing_reason() << ")";
		break;
	case nlay_event_t::CNX_DESTROYED:
		os << "CNX_DESTROYED (reason: " << event.get_cnx_destroyed_reason() << ")";
		break;
	case nlay_event_t::IDLE_TIMEDOUT:
		os << "IDLE_TIMEDOUT (reason: " << event.get_idle_timedout_reason() << ")";
		break;
	case nlay_event_t::REMOTE_PEER_UNREACHABLE:
		os << "REMOTE_PEER_UNREACHABLE (reason: " << event.get_remote_peer_unreachable_reason() << ")";
		break;
	case nlay_event_t::CNX_ESTABLISHED:
		//os << "CNX_ESTABLISHED (nlay_full_t=" << *event.get_cnx_established();
		os << "CNX_ESTABLISHED";
		break;	
	case nlay_event_t::PKT_TO_LOWER:
		os << "PKT_TO_LOWER (" << (event.get_pkt_to_lower())->get_len() << "-byte)";
		break;
	case nlay_event_t::PKT_TO_UPPER:
		os << "PKT_TO_UPPER (" << (event.get_pkt_to_upper())->get_len() << "-byte)";
		break;
	case nlay_event_t::MAYSEND_ON:
		os << "MAYSEND_ON";
		break;	
	case nlay_event_t::MAYSEND_OFF:
		os << "MAYSEND_OFF";
		break;			
	case nlay_event_t::NEW_MTU:
		os << "NEW_MTU (mtu=" << event.get_new_mtu() << "-byte)";
		break;
	default: 	DBG_ASSERT(0);
	}
	return os;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_REFUSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_REFUSED, false otherwise
 */
bool	nlay_event_t::is_cnx_refused() const throw()
{
	return type_val == CNX_REFUSED;
}

/** \brief build a nlay_event_t to CNX_REFUSED (with a possible reason)
 */
nlay_event_t nlay_event_t::build_cnx_refused(const std::string &reason )	throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val	= CNX_REFUSED;
	nlay_event.reason	= reason;
	// return the built object
	return nlay_event;
}

/** \brief return the CNX_REFUSED reason
 */
const std::string &nlay_event_t::get_cnx_refused_reason() const throw()
{
	// sanity check - the event MUST be CNX_REFUSED
	DBG_ASSERT( is_cnx_refused() );
	// return the reason
	return reason;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_CLOSING
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_CLOSING, false otherwise
 */
bool	nlay_event_t::is_cnx_closing() const throw()
{
	return type_val == CNX_CLOSING;
}

/** \brief build a nlay_event_t to CNX_CLOSING (with a possible reason)
 */
nlay_event_t nlay_event_t::build_cnx_closing(const std::string &reason )	throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val	= CNX_CLOSING;
	nlay_event.reason	= reason;
	// return the built object
	return nlay_event;
}

/** \brief return the CNX_CLOSING reason
 */
const std::string &nlay_event_t::get_cnx_closing_reason() const throw()
{
	// sanity check - the event MUST be CNX_CLOSING
	DBG_ASSERT( is_cnx_closing() );
	// return the reason
	return reason;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_DESTROYED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_DESTROYED, false otherwise
 */
bool	nlay_event_t::is_cnx_destroyed() const throw()
{
	return type_val == CNX_DESTROYED;
}

/** \brief build a nlay_event_t to CNX_DESTROYED (with a possible reason)
 */
nlay_event_t nlay_event_t::build_cnx_destroyed(const std::string &reason )	throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val	= CNX_DESTROYED;
	nlay_event.reason	= reason;
	// return the built object
	return nlay_event;
}

/** \brief return the CNX_DESTROYED reason
 */
const std::string &nlay_event_t::get_cnx_destroyed_reason() const throw()
{
	// sanity check - the event MUST be CNX_DESTROYED
	DBG_ASSERT( is_cnx_destroyed() );
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
bool	nlay_event_t::is_idle_timedout() const throw()
{
	return type_val == IDLE_TIMEDOUT;
}

/** \brief build a nlay_event_t to IDLE_TIMEDOUT (with a possible reason)
 */
nlay_event_t nlay_event_t::build_idle_timedout(const std::string &reason )	throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val	= IDLE_TIMEDOUT;
	nlay_event.reason	= reason;
	// return the built object
	return nlay_event;
}

/** \brief return the IDLE_TIMEDOUT reason
 */
const std::string &nlay_event_t::get_idle_timedout_reason() const throw()
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
bool	nlay_event_t::is_remote_peer_unreachable() const throw()
{
	return type_val == REMOTE_PEER_UNREACHABLE;
}

/** \brief build a nlay_event_t to REMOTE_PEER_UNREACHABLE (with a possible reason)
 */
nlay_event_t nlay_event_t::build_remote_peer_unreachable(const std::string &reason )	throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val	= REMOTE_PEER_UNREACHABLE;
	nlay_event.reason	= reason;
	// return the built object
	return nlay_event;
}

/** \brief return the REMOTE_PEER_UNREACHABLE reason
 */
const std::string &nlay_event_t::get_remote_peer_unreachable_reason() const throw()
{
	// sanity check - the event MUST be REMOTE_PEER_UNREACHABLE
	DBG_ASSERT( is_remote_peer_unreachable() );
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
bool	nlay_event_t::is_cnx_established() const throw()
{
	return type_val == CNX_ESTABLISHED;
}

/** \brief Build a nlay_event_t to CNX_ESTABLISHED
 */
nlay_event_t nlay_event_t::build_cnx_established( nlay_full_t *nlay_full ) 
						throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val	= CNX_ESTABLISHED;
	nlay_event.nlay_full	= nlay_full;
	// return the built object
	return nlay_event;
}

/** \brief return the nlay_full_t when CNX_ESTABLISHED
 */
nlay_full_t *nlay_event_t::get_cnx_established() const throw()
{
	// sanity check - the event MUST be CNX_ESTABLISHED
	DBG_ASSERT( is_cnx_established() );
	// return the nlay_full
	return nlay_full;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PKT_TO_LOWER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PKT_TO_LOWER, false otherwise
 */
bool	nlay_event_t::is_pkt_to_lower()						const throw()
{
	return type_val == PKT_TO_LOWER;
}

/** \brief Build a nlay_event_t to PKT_TO_LOWER
 */
nlay_event_t nlay_event_t::build_pkt_to_lower( pkt_t *pkt ) 			throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val	= PKT_TO_LOWER;
	nlay_event.pkt_ptr	= pkt;
	// return the built object
	return nlay_event;
}

/** \brief return the neoip_nlay_full when PKT_TO_LOWER
 */
pkt_t *nlay_event_t::get_pkt_to_lower()						const throw()
{
	// sanity check - the event MUST be PKT_TO_LOWER
	DBG_ASSERT( is_pkt_to_lower() );
	// return the datum_ptr
	return pkt_ptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PKT_TO_UPPER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PKT_TO_UPPER, false otherwise
 */
bool	nlay_event_t::is_pkt_to_upper()						const throw()
{
	return type_val == PKT_TO_UPPER;
}

/** \brief Build a nlay_event_t to PKT_TO_UPPER
 */
nlay_event_t nlay_event_t::build_pkt_to_upper( pkt_t *pkt ) 			throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val	= PKT_TO_UPPER;
	nlay_event.pkt_ptr	= pkt;
	// return the built object
	return nlay_event;
}

/** \brief return the neoip_nlay_full when PKT_TO_UPPER
 */
pkt_t *nlay_event_t::get_pkt_to_upper()						const throw()
{
	// sanity check - the event MUST be PKT_TO_UPPER
	DBG_ASSERT( is_pkt_to_upper() );
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
bool	nlay_event_t::is_maysend_on() const throw()
{
	return type_val == MAYSEND_ON;
}

/** \brief Build a nlay_event_t to MAYSEND_ON
 */
nlay_event_t nlay_event_t::build_maysend_on() throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val		= MAYSEND_ON;
	// return the built object
	return nlay_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    MAYSEND_OFF
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is MAYSEND_OFF, false otherwise
 */
bool	nlay_event_t::is_maysend_off() const throw()
{
	return type_val == MAYSEND_OFF;
}

/** \brief Build a nlay_event_t to MAYSEND_OFF
 */
nlay_event_t nlay_event_t::build_maysend_off() throw()
{
	nlay_event_t	nlay_event;
	// set the type_val
	nlay_event.type_val		= MAYSEND_OFF;
	// return the built object
	return nlay_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    NEW_MTU
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is NEW_MTU, false otherwise
 */
bool	nlay_event_t::is_new_mtu()						const throw()
{
	return type_val == NEW_MTU;
}

/** \brief Build a nlay_event_t to NEW_MTU
 */
nlay_event_t nlay_event_t::build_new_mtu(size_t mtu_size) 			throw()
{
	nlay_event_t	inet_nlay_event;
	// set the type_val
	inet_nlay_event.type_val		= NEW_MTU;
	inet_nlay_event.mtu_size		= mtu_size;
	// return the built object
	return inet_nlay_event;
}

/** \brief return the udp_full_t when NEW_MTU
 */
size_t	nlay_event_t::get_new_mtu()						const throw()
{
	// sanity check - the event MUST be NEW_MTU
	DBG_ASSERT( is_new_mtu() );
	// return the pkt_ptr
	return mtu_size;
}

NEOIP_NAMESPACE_END

