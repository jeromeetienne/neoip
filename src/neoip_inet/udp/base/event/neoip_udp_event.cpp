/*! \file
    \brief Implementation of the udp_event
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_udp_event.hpp"
#include "neoip_udp_full.hpp"
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
udp_event_t::udp_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
udp_event_t::~udp_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const udp_event_t & event) throw()
{
	switch( event.type_val ){
	case udp_event_t::NONE:		
		os << "NONE";
		break;
	case udp_event_t::CNX_CLOSED:	
		os << "CNX_CLOSED (reason: " << event.get_cnx_closed_reason() << ")";
		break;
	case udp_event_t::CNX_REFUSED:
		os << "CNX_REFUSED (reason: " << event.get_cnx_refused_reason() << ")";
		break;
	case udp_event_t::CNX_ESTABLISHED:
		os << "CNX_ESTABLISHED (cnx: ";
		if( event.get_cnx_established() )	os << *(event.get_cnx_established());
		else					os << "NULL";
		os << ")";
		break;
	case udp_event_t::RECVED_DATA:
		os << "RECVED_DATA (length=" << (event.get_recved_data())->get_len() << "-byte)";
		break;	
	case udp_event_t::MTU_CHANGE:
		os << "MTU_CHANGE (mtu=" << event.get_mtu_change() << "-byte)";
		break;	
	default: 	DBG_ASSERT(0);
	}
	return os;
}

/** \brief convert the object into a string
 */
std::string	udp_event_t::to_string()		const throw()
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
bool	udp_event_t::is_cnx_closed()						const throw()
{
	return type_val == CNX_CLOSED;
}

/** \brief set event to cnx_closed (with a possible reason)
 */
udp_event_t udp_event_t::build_cnx_closed(const std::string &reason)		throw()
{
	udp_event_t	udp_event;
	// set the type_val
	udp_event.type_val	= CNX_CLOSED;
	udp_event.reason_str	= reason;
	// return the built object
	return udp_event;
}

/** \brief return the cnx_closed reason
 */
const std::string &udp_event_t::get_cnx_closed_reason()				const throw()
{
	// sanity check - the event MUST be CNX_CLOSED
	DBG_ASSERT( is_cnx_closed() );
	// return the reasons
	return reason_str;
	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_REFUSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_REFUSED, false otherwise
 */
bool	udp_event_t::is_cnx_refused()						const throw()
{
	return type_val == CNX_REFUSED;
}

/** \brief set event to cnx_refused (with a possible reason)
 */
udp_event_t udp_event_t::build_cnx_refused(const std::string &reason)		throw()
{
	udp_event_t	udp_event;
	// set the type_val
	udp_event.type_val	= CNX_REFUSED;
	udp_event.reason_str	= reason;
	// return the built object
	return udp_event;
}

/** \brief return the cnx_refused reason
 */
const std::string &udp_event_t::get_cnx_refused_reason()			const throw()
{
	// sanity check - the event MUST be CNX_REFUSED
	DBG_ASSERT( is_cnx_refused() );
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
bool	udp_event_t::is_cnx_established()					const throw()
{
	return type_val == CNX_ESTABLISHED;
}

/** \brief set event to cnx_established
 */
udp_event_t udp_event_t::build_cnx_established(udp_full_t *udp_full)		throw()
{
	udp_event_t	udp_event;
	// set the type_val
	udp_event.type_val	= CNX_ESTABLISHED;
	udp_event.udp_full	= udp_full;
	// return the built object
	return udp_event;
}

/** \brief return the cnx_refused reason
 */
udp_full_t *udp_event_t::get_cnx_established()					const throw()
{
	// sanity check - the event MUST be CNX_ESTABLISHED
	DBG_ASSERT( is_cnx_established() );
	// return the udp_full
	return udp_full;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    RECVED_DATA
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is RECVED_DATA, false otherwise
 */
bool	udp_event_t::is_recved_data()						const throw()
{
	return type_val == RECVED_DATA;
}

/** \brief Build a udp_event_t to RECVED_DATA
 */
udp_event_t udp_event_t::build_recved_data(pkt_t *pkt) 				throw()
{
	udp_event_t	inet_udp_event;
	// set the type_val
	inet_udp_event.type_val		= RECVED_DATA;
	inet_udp_event.pkt_ptr		= pkt;
	// return the built object
	return inet_udp_event;
}

/** \brief return the udp_full_t when RECVED_DATA
 */
pkt_t *	udp_event_t::get_recved_data()						const throw()
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
bool	udp_event_t::is_mtu_change()						const throw()
{
	return type_val == MTU_CHANGE;
}

/** \brief Build a udp_event_t to MTU_CHANGE
 */
udp_event_t udp_event_t::build_mtu_change(size_t mtu_size) 			throw()
{
	udp_event_t	inet_udp_event;
	// set the type_val
	inet_udp_event.type_val		= MTU_CHANGE;
	inet_udp_event.mtu_size		= mtu_size;
	// return the built object
	return inet_udp_event;
}

/** \brief return the udp_full_t when MTU_CHANGE
 */
size_t	udp_event_t::get_mtu_change()						const throw()
{
	// sanity check - the event MUST be MTU_CHANGE
	DBG_ASSERT( is_mtu_change() );
	// return the pkt_ptr
	return mtu_size;
}
NEOIP_NAMESPACE_END



