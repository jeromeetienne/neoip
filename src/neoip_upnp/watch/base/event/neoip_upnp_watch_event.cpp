/*! \file
    \brief Implementation of the upnp_watch_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_upnp_watch_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
upnp_watch_event_t::upnp_watch_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
upnp_watch_event_t::~upnp_watch_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	upnp_watch_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case upnp_watch_event_t::NONE:			oss << "NONE";				break;
	case upnp_watch_event_t::UPNPDISC_CHANGED:	oss << "UPNPDISC_CHANGED";		break;
	case upnp_watch_event_t::EXTIPADDR_CHANGED:	oss << "EXTIPADDR_CHANGED";		break;
	default: 	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    UPNPDISC_CHANGED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is UPNPDISC_CHANGED, false otherwise
 */
bool	upnp_watch_event_t::is_upnpdisc_changed() const throw()
{
	return type_val == UPNPDISC_CHANGED;
}

/** \brief build a upnp_watch_event_t to UPNPDISC_CHANGED
 */
upnp_watch_event_t upnp_watch_event_t::build_upnpdisc_changed()	throw()
{
	upnp_watch_event_t	peersrc_event;
	// set the type_val
	peersrc_event.type_val		= UPNPDISC_CHANGED;
	// return the built object
	return peersrc_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    EXTIPADDR_CHANGED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is EXTIPADDR_CHANGED, false otherwise
 */
bool	upnp_watch_event_t::is_extipaddr_changed() const throw()
{
	return type_val == EXTIPADDR_CHANGED;
}

/** \brief build a upnp_watch_event_t to EXTIPADDR_CHANGED
 */
upnp_watch_event_t upnp_watch_event_t::build_extipaddr_changed()	throw()
{
	upnp_watch_event_t	peersrc_event;
	// set the type_val
	peersrc_event.type_val	= EXTIPADDR_CHANGED;
	// return the built object
	return peersrc_event;
}


NEOIP_NAMESPACE_END

