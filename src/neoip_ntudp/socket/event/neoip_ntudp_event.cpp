/*! \file
    \brief Implementation of the ntudp_event
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_ntudp_event.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
ntudp_event_t::ntudp_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
ntudp_event_t::~ntudp_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const ntudp_event_t & event) throw()
{
	switch( event.type_val ){
	case ntudp_event_t::NONE:		
		os << "NONE";
		break;
	case ntudp_event_t::CNX_CLOSED:	
		os << "CNX_CLOSED (reason: " << event.get_cnx_closed_reason() << ")";
		break;
	case ntudp_event_t::CNX_ESTA_TIMEDOUT:
		os << "CNX_ESTA_TIMEDOUT (reason: " << event.get_cnx_esta_timedout_reason() << ")";
		break;
	case ntudp_event_t::CNX_ESTABLISHED:
		os << "CNX_ESTABLISHED (cnx: ";
#if 0
		if( event.get_cnx_established() )	os << *(event.get_cnx_established());
		else					os << "NULL";
#endif
		os << ")";
		break;
	case ntudp_event_t::UNKNOWN_HOST:
		os << "UNKNOWN_HOST (reason: " << event.get_unknown_host_reason() << ")";
		break;
	case ntudp_event_t::NO_ROUTE_TO_HOST:
		os << "NO_ROUTE_TO_HOST (reason: " << event.get_no_route_to_host_reason() << ")";
		break;
	case ntudp_event_t::RECVED_DATA:
		os << "RECVED_DATA (length=" << (event.get_recved_data())->get_len() << "-byte)";
		break;	
	case ntudp_event_t::MTU_CHANGE:
		os << "MTU_CHANGE (mtu=" << event.get_mtu_change() << "-byte)";
		break;	
	default: 	DBG_ASSERT(0);
	}
	return os;
}

/** \brief convert the object into a string
 */
std::string	ntudp_event_t::to_string()		const throw()
{
	std::ostringstream      oss;
	oss << *this;
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_CLOSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_CLOSED, false otherwise
 */
bool	ntudp_event_t::is_cnx_closed()						const throw()
{
	return type_val == CNX_CLOSED;
}

/** \brief set event to cnx_closed (with a possible reason)
 */
ntudp_event_t ntudp_event_t::build_cnx_closed(const std::string &reason)		throw()
{
	ntudp_event_t	ntudp_event;
	// set the type_val
	ntudp_event.type_val	= CNX_CLOSED;
	ntudp_event.reason_str	= reason;
	// return the built object
	return ntudp_event;
}

/** \brief return the cnx_closed reason
 */
const std::string &ntudp_event_t::get_cnx_closed_reason()				const throw()
{
	// sanity check - the event MUST be CNX_CLOSED
	DBG_ASSERT( is_cnx_closed() );
	// return the reasons
	return reason_str;
	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_ESTA_TIMEDOUT
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_ESTA_TIMEDOUT, false otherwise
 */
bool	ntudp_event_t::is_cnx_esta_timedout()						const throw()
{
	return type_val == CNX_ESTA_TIMEDOUT;
}

/** \brief set event to cnx_esta_timedout (with a possible reason)
 */
ntudp_event_t ntudp_event_t::build_cnx_esta_timedout(const std::string &reason)		throw()
{
	ntudp_event_t	ntudp_event;
	// set the type_val
	ntudp_event.type_val	= CNX_ESTA_TIMEDOUT;
	ntudp_event.reason_str	= reason;
	// return the built object
	return ntudp_event;
}

/** \brief return the cnx_esta_timedout reason
 */
const std::string &ntudp_event_t::get_cnx_esta_timedout_reason()			const throw()
{
	// sanity check - the event MUST be CNX_ESTA_TIMEDOUT
	DBG_ASSERT( is_cnx_esta_timedout() );
	// return the reasons
	return reason_str;
	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_ESTABLISHED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_ESTABLISHED, false otherwise
 */
bool	ntudp_event_t::is_cnx_established()					const throw()
{
	return type_val == CNX_ESTABLISHED;
}

/** \brief set event to cnx_established
 */
ntudp_event_t ntudp_event_t::build_cnx_established(ntudp_full_t *ntudp_full)		throw()
{
	ntudp_event_t	ntudp_event;
	// set the type_val
	ntudp_event.type_val	= CNX_ESTABLISHED;
	ntudp_event.ntudp_full	= ntudp_full;
	// return the built object
	return ntudp_event;
}

/** \brief return the cnx_established reason
 */
ntudp_full_t *ntudp_event_t::get_cnx_established()					const throw()
{
	// sanity check - the event MUST be CNX_ESTABLISHED
	DBG_ASSERT( is_cnx_established() );
	// return the ntudp_full
	return ntudp_full;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    UNKNOWN_HOST
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is UNKNOWN_HOST, false otherwise
 */
bool	ntudp_event_t::is_unknown_host()						const throw()
{
	return type_val == UNKNOWN_HOST;
}

/** \brief set event to unknown_host (with a possible reason)
 */
ntudp_event_t ntudp_event_t::build_unknown_host(const std::string &reason)		throw()
{
	ntudp_event_t	ntudp_event;
	// set the type_val
	ntudp_event.type_val	= UNKNOWN_HOST;
	ntudp_event.reason_str	= reason;
	// return the built object
	return ntudp_event;
}

/** \brief return the unknown_host reason
 */
const std::string &ntudp_event_t::get_unknown_host_reason()			const throw()
{
	// sanity check - the event MUST be UNKNOWN_HOST
	DBG_ASSERT( is_unknown_host() );
	// return the reasons
	return reason_str;
	
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    NO_ROUTE_TO_HOST
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is NO_ROUTE_TO_HOST, false otherwise
 */
bool	ntudp_event_t::is_no_route_to_host()						const throw()
{
	return type_val == NO_ROUTE_TO_HOST;
}

/** \brief set event to no_route_to_host (with a possible reason)
 */
ntudp_event_t ntudp_event_t::build_no_route_to_host(const std::string &reason)		throw()
{
	ntudp_event_t	ntudp_event;
	// set the type_val
	ntudp_event.type_val	= NO_ROUTE_TO_HOST;
	ntudp_event.reason_str	= reason;
	// return the built object
	return ntudp_event;
}

/** \brief return the no_route_to_host reason
 */
const std::string &ntudp_event_t::get_no_route_to_host_reason()			const throw()
{
	// sanity check - the event MUST be NO_ROUTE_TO_HOST
	DBG_ASSERT( is_no_route_to_host() );
	// return the reasons
	return reason_str;
	
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    RECVED_DATA
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is RECVED_DATA, false otherwise
 */
bool	ntudp_event_t::is_recved_data()						const throw()
{
	return type_val == RECVED_DATA;
}

/** \brief Build a ntudp_event_t to RECVED_DATA
 */
ntudp_event_t ntudp_event_t::build_recved_data(pkt_t *pkt) 				throw()
{
	ntudp_event_t	inet_ntudp_event;
	// set the type_val
	inet_ntudp_event.type_val		= RECVED_DATA;
	inet_ntudp_event.pkt_ptr		= pkt;
	// return the built object
	return inet_ntudp_event;
}

/** \brief return the ntudp_full_t when RECVED_DATA
 */
pkt_t *	ntudp_event_t::get_recved_data()						const throw()
{
	// sanity check - the event MUST be RECVED_DATA
	DBG_ASSERT( is_recved_data() );
	// return the pkt_ptr
	return pkt_ptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    MTU_CHANGE
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is MTU_CHANGE, false otherwise
 */
bool	ntudp_event_t::is_mtu_change()						const throw()
{
	return type_val == MTU_CHANGE;
}

/** \brief Build a ntudp_event_t to MTU_CHANGE
 */
ntudp_event_t ntudp_event_t::build_mtu_change(size_t mtu_size) 			throw()
{
	ntudp_event_t	inet_ntudp_event;
	// set the type_val
	inet_ntudp_event.type_val		= MTU_CHANGE;
	inet_ntudp_event.mtu_size		= mtu_size;
	// return the built object
	return inet_ntudp_event;
}

/** \brief return the ntudp_full_t when MTU_CHANGE
 */
size_t	ntudp_event_t::get_mtu_change()						const throw()
{
	// sanity check - the event MUST be MTU_CHANGE
	DBG_ASSERT( is_mtu_change() );
	// return the pkt_ptr
	return mtu_size;
}
NEOIP_NAMESPACE_END



