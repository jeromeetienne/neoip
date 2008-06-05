/*! \file
    \brief Implementation of the bt_peersrc_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_bt_peersrc_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
bt_peersrc_event_t::bt_peersrc_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
bt_peersrc_event_t::~bt_peersrc_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	bt_peersrc_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case bt_peersrc_event_t::NONE:		oss << "NONE";					break;
	case bt_peersrc_event_t::DOREGISTER:	oss << "DOREGISTER";				break;
	case bt_peersrc_event_t::UNREGISTER:	oss << "UNREGISTER";				break;
	case bt_peersrc_event_t::NEW_PEER:	oss << "NEW_PEER(" << *peersrc_peer << ")";	break;
	default: 	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    DOREGISTER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is DOREGISTER, false otherwise
 */
bool	bt_peersrc_event_t::is_doregister() const throw()
{
	return type_val == DOREGISTER;
}

/** \brief build a bt_peersrc_event_t to DOREGISTER
 */
bt_peersrc_event_t bt_peersrc_event_t::build_doregister()	throw()
{
	bt_peersrc_event_t	peersrc_event;
	// set the type_val
	peersrc_event.type_val		= DOREGISTER;
	// return the built object
	return peersrc_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    UNREGISTER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is UNREGISTER, false otherwise
 */
bool	bt_peersrc_event_t::is_unregister() const throw()
{
	return type_val == UNREGISTER;
}

/** \brief build a bt_peersrc_event_t to UNREGISTER
 */
bt_peersrc_event_t bt_peersrc_event_t::build_unregister()	throw()
{
	bt_peersrc_event_t	peersrc_event;
	// set the type_val
	peersrc_event.type_val	= UNREGISTER;
	// return the built object
	return peersrc_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    NEW_PEER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is NEW_PEER, false otherwise
 */
bool	bt_peersrc_event_t::is_new_peer() const throw()
{
	return type_val == NEW_PEER;
}

/** \brief build a bt_peersrc_event_t to NEW_PEER
 */
bt_peersrc_event_t bt_peersrc_event_t::build_new_peer(bt_peersrc_peer_t *peersrc_peer)	throw()
{
	bt_peersrc_event_t	peersrc_event;
	// set the type_val
	peersrc_event.type_val		= NEW_PEER;
	peersrc_event.peersrc_peer	= peersrc_peer;
	// return the built object
	return peersrc_event;
}

/** \brief return the NEW_PEER data
 */
bt_peersrc_peer_t *	bt_peersrc_event_t::get_new_peer() const throw()
{
	// sanity check - the event MUST be NEW_PEER
	DBG_ASSERT( is_new_peer() );
	// return the bt_err
	return peersrc_peer;	
}


NEOIP_NAMESPACE_END

