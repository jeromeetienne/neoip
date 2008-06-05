/*! \file
    \brief Implementation of the bt_ezswarm_event_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
bt_ezswarm_event_t::bt_ezswarm_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
bt_ezswarm_event_t::~bt_ezswarm_event_t() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	bt_ezswarm_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case bt_ezswarm_event_t::NONE:			oss << "NONE";					break;
	case bt_ezswarm_event_t::SWARM_EVENT:		oss << "SWARM_EVENT("<<(*swarm_event)<<")";	break;
	case bt_ezswarm_event_t::LEAVE_STATE_PRE:	oss << "LEAVE_STATE_PRE(" <<ezswarm_state<<")";	break;
	case bt_ezswarm_event_t::LEAVE_STATE_POST:	oss << "LEAVE_STATE_POST("<<ezswarm_state<<")";	break;
	case bt_ezswarm_event_t::ENTER_STATE_PRE:	oss << "ENTER_STATE_PRE(" <<ezswarm_state<<")";	break;
	case bt_ezswarm_event_t::ENTER_STATE_POST:	oss << "ENTER_STATE_POST("<<ezswarm_state<<")";	break;
	default: 	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    SWARM_EVENT
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is SWARM_EVENT, false otherwise
 */
bool	bt_ezswarm_event_t::is_swarm_event()					const throw()
{
	return type_val == SWARM_EVENT;
}

/** \brief build a bt_ezswarm_event_t to SWARM_EVENT
 */
bt_ezswarm_event_t bt_ezswarm_event_t::build_swarm_event(const bt_swarm_event_t *swarm_event)	throw()
{
	bt_ezswarm_event_t	ezswarm_event;
	// set the type_val
	ezswarm_event.type_val		= SWARM_EVENT;
	ezswarm_event.swarm_event	= swarm_event;
	// return the built object
	return ezswarm_event;
}

/** \brief return the SWARM_EVENT data
 */
const bt_swarm_event_t *bt_ezswarm_event_t::get_swarm_event()			const throw()
{
	// sanity check - the event MUST be SWARM_EVENT
	DBG_ASSERT( is_swarm_event() );
	// return the bt_err
	return swarm_event;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    LEAVE_STATE_PRE
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is LEAVE_STATE_PRE, false otherwise
 */
bool	bt_ezswarm_event_t::is_leave_state_pre()				const throw()
{
	return type_val == LEAVE_STATE_PRE;
}

/** \brief build a bt_ezswarm_event_t to LEAVE_STATE_PRE (with a possible reason)
 */
bt_ezswarm_event_t bt_ezswarm_event_t::build_leave_state_pre(const bt_ezswarm_state_t &state)	throw()
{
	bt_ezswarm_event_t	ezswarm_event;
	// set the type_val
	ezswarm_event.type_val		= LEAVE_STATE_PRE;
	ezswarm_event.ezswarm_state	= state;
	// return the built object
	return ezswarm_event;
}

/** \brief return the LEAVE_STATE_PRE data
 */
const bt_ezswarm_state_t &	bt_ezswarm_event_t::get_leave_state_pre()	const throw()
{
	// sanity check - the event MUST be LEAVE_STATE_PRE
	DBG_ASSERT( is_leave_state_pre() );
	// return the bt_ezswarm_state_t
	return ezswarm_state;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    LEAVE_STATE_POST
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is LEAVE_STATE_POST, false otherwise
 */
bool	bt_ezswarm_event_t::is_leave_state_post()				const throw()
{
	return type_val == LEAVE_STATE_POST;
}

/** \brief build a bt_ezswarm_event_t to LEAVE_STATE_POST (with a possible reason)
 */
bt_ezswarm_event_t bt_ezswarm_event_t::build_leave_state_post(const bt_ezswarm_state_t &state)	throw()
{
	bt_ezswarm_event_t	ezswarm_event;
	// set the type_val
	ezswarm_event.type_val		= LEAVE_STATE_POST;
	ezswarm_event.ezswarm_state	= state;
	// return the built object
	return ezswarm_event;
}

/** \brief return the LEAVE_STATE_POST data
 */
const bt_ezswarm_state_t &	bt_ezswarm_event_t::get_leave_state_post()	const throw()
{
	// sanity check - the event MUST be LEAVE_STATE_POST
	DBG_ASSERT( is_leave_state_post() );
	// return the bt_ezswarm_state_t
	return ezswarm_state;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    ENTER_STATE_PRE
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is ENTER_STATE_PRE, false otherwise
 */
bool	bt_ezswarm_event_t::is_enter_state_pre()				const throw()
{
	return type_val == ENTER_STATE_PRE;
}

/** \brief build a bt_ezswarm_event_t to ENTER_STATE_PRE (with a possible reason)
 */
bt_ezswarm_event_t bt_ezswarm_event_t::build_enter_state_pre(const bt_ezswarm_state_t &state)	throw()
{
	bt_ezswarm_event_t	ezswarm_event;
	// set the type_val
	ezswarm_event.type_val		= ENTER_STATE_PRE;
	ezswarm_event.ezswarm_state	= state;
	// return the built object
	return ezswarm_event;
}

/** \brief return the ENTER_STATE_PRE data
 */
const bt_ezswarm_state_t &	bt_ezswarm_event_t::get_enter_state_pre()	const throw()
{
	// sanity check - the event MUST be ENTER_STATE_PRE
	DBG_ASSERT( is_enter_state_pre() );
	// return the bt_ezswarm_state_t
	return ezswarm_state;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    ENTER_STATE_POST
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is ENTER_STATE_POST, false otherwise
 */
bool	bt_ezswarm_event_t::is_enter_state_post()				const throw()
{
	return type_val == ENTER_STATE_POST;
}

/** \brief build a bt_ezswarm_event_t to ENTER_STATE_POST (with a possible reason)
 */
bt_ezswarm_event_t bt_ezswarm_event_t::build_enter_state_post(const bt_ezswarm_state_t &state)	throw()
{
	bt_ezswarm_event_t	ezswarm_event;
	// set the type_val
	ezswarm_event.type_val		= ENTER_STATE_POST;
	ezswarm_event.ezswarm_state	= state;
	// return the built object
	return ezswarm_event;
}

/** \brief return the ENTER_STATE_POST data
 */
const bt_ezswarm_state_t &	bt_ezswarm_event_t::get_enter_state_post()	const throw()
{
	// sanity check - the event MUST be ENTER_STATE_POST
	DBG_ASSERT( is_enter_state_post() );
	// return the bt_ezswarm_state_t
	return ezswarm_state;	
}
NEOIP_NAMESPACE_END

