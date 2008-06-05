/*! \file
    \brief Implementation of the bt_httpo_event_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_httpo_event.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
bt_httpo_event_t::bt_httpo_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
bt_httpo_event_t::~bt_httpo_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	bt_httpo_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case bt_httpo_event_t::NONE:		oss << "NONE";					break;
	case bt_httpo_event_t::CNX_ESTABLISHED:	oss << "CNX_ESTABLISHED(" << httpo_full << ")";	break;
	case bt_httpo_event_t::CNX_CLOSED:	oss << "CNX_CLOSED";				break;
	default: 	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_ESTABLISHED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_ESTABLISHED, false otherwise
 */
bool	bt_httpo_event_t::is_cnx_established() const throw()
{
	return type_val == CNX_ESTABLISHED;
}

/** \brief build a bt_httpo_event_t to CNX_ESTABLISHED (with a possible reason)
 */
bt_httpo_event_t bt_httpo_event_t::build_cnx_established(bt_httpo_full_t *httpo_full) throw()
{
	bt_httpo_event_t	httpo_event;
	// set the type_val
	httpo_event.type_val	= CNX_ESTABLISHED;
	httpo_event.httpo_full	= httpo_full;
	// return the built object
	return httpo_event;
}

/** \brief return the CNX_ESTABLISHED data
 */
bt_httpo_full_t *	bt_httpo_event_t::get_cnx_established() const throw()
{
	// sanity check - the event MUST be CNX_ESTABLISHED
	DBG_ASSERT( is_cnx_established() );
	// return the bt_httpo_full_t
	return httpo_full;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_CLOSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_CLOSED, false otherwise
 */
bool	bt_httpo_event_t::is_cnx_closed() const throw()
{
	return type_val == CNX_CLOSED;
}

/** \brief build a bt_httpo_event_t to CNX_CLOSED (with a possible reason)
 */
bt_httpo_event_t bt_httpo_event_t::build_cnx_closed()			throw()
{
	bt_httpo_event_t	httpo_event;
	// set the type_val
	httpo_event.type_val	= CNX_CLOSED;
	// return the built object
	return httpo_event;
}

NEOIP_NAMESPACE_END

