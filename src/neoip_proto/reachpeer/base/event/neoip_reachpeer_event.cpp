/*! \file
    \brief Implementation of the reachpeer_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_reachpeer_event.hpp"
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
reachpeer_event_t::reachpeer_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
reachpeer_event_t::~reachpeer_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const reachpeer_event_t &event) throw()
{
	switch( event.type_val ){
	case reachpeer_event_t::NONE:
		os << "NONE";
		break;
	case reachpeer_event_t::PEER_UNREACH:
		os << "PEER_UNREACH (reason: " << event.get_peer_unreach_reason() << ")";
		break;
	case reachpeer_event_t::PKT_TO_LOWER:
		os << "PKT_TO_LOWER (" << (event.get_pkt_to_lower())->get_len() << "-byte)";
		break;	
	default: 	DBG_ASSERT(0);
	}
	return os;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PEER_UNREACH
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PEER_UNREACH, false otherwise
 */
bool	reachpeer_event_t::is_peer_unreach() const throw()
{
	return type_val == PEER_UNREACH;
}

/** \brief build a reachpeer_event_t to PEER_UNREACH (with a possible reason)
 */
reachpeer_event_t reachpeer_event_t::build_peer_unreach(const std::string &reason)	throw()
{
	reachpeer_event_t	reachpeer_event;
	// set the type_val
	reachpeer_event.type_val	= PEER_UNREACH;
	reachpeer_event.reason	= reason;
	// return the built object
	return reachpeer_event;
}

/** \brief return the PEER_UNREACH reason
 */
const std::string &reachpeer_event_t::get_peer_unreach_reason() const throw()
{
	// sanity check - the event MUST be PEER_UNREACH
	DBG_ASSERT( is_peer_unreach() );
	// return the reason
	return reason;	
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PKT_TO_LOWER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PKT_TO_LOWER, false otherwise
 */
bool	reachpeer_event_t::is_pkt_to_lower()						const throw()
{
	return type_val == PKT_TO_LOWER;
}

/** \brief Build a reachpeer_event_t to PKT_TO_LOWER
 */
reachpeer_event_t reachpeer_event_t::build_pkt_to_lower( pkt_t *pkt ) 			throw()
{
	reachpeer_event_t	reachpeer_event;
	// set the type_val
	reachpeer_event.type_val	= PKT_TO_LOWER;
	reachpeer_event.pkt_ptr	= pkt;
	// return the built object
	return reachpeer_event;
}

/** \brief return the neoip_reachpeer_full when PKT_TO_LOWER
 */
pkt_t *reachpeer_event_t::get_pkt_to_lower()						const throw()
{
	// sanity check - the event MUST be PKT_TO_LOWER
	DBG_ASSERT( is_pkt_to_lower() );
	// return the datum_ptr
	return pkt_ptr;
}


NEOIP_NAMESPACE_END

