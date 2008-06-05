/*! \file
    \brief Implementation of the tcp_event
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_tcp_event.hpp"
#include "neoip_tcp_full.hpp"
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
tcp_event_t::tcp_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
tcp_event_t::~tcp_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const tcp_event_t & event) throw()
{
	switch( event.type_val ){
	case tcp_event_t::NONE:		
		os << "NONE";
		break;
	case tcp_event_t::CNX_CLOSED:	
		os << "CNX_CLOSED (reason: " << event.get_cnx_closed_reason() << ")";
		break;
	case tcp_event_t::CNX_REFUSED:
		os << "CNX_REFUSED (reason: " << event.get_cnx_refused_reason() << ")";
		break;
	case tcp_event_t::CNX_ESTABLISHED:
		os << "CNX_ESTABLISHED (cnx: ";
		if(event.get_cnx_established())	os << *event.get_cnx_established();
		else				os << "NULL";
		os << ")";
		break;
	case tcp_event_t::RECVED_DATA:
		os << "RECVED_DATA (" << (event.get_recved_data())->get_len() << "-byte)";
		break;	
	case tcp_event_t::MAYSEND_ON:
		os << "MAYSEND_ON";
		break;
	case tcp_event_t::MAYSEND_OFF:
		os << "MAYSEND_OFF";
		break;
	default: 	DBG_ASSERT(0);
	}
	return os;
}

/** \brief convert the object into a string
 */
std::string	tcp_event_t::to_string()		const throw()
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
bool	tcp_event_t::is_cnx_closed()						const throw()
{
	return type_val == CNX_CLOSED;
}

/** \brief set event to cnx_closed (with a possible reason)
 */
tcp_event_t tcp_event_t::build_cnx_closed(const std::string &reason)		throw()
{
	tcp_event_t	tcp_event;
	// set the type_val
	tcp_event.type_val	= CNX_CLOSED;
	tcp_event.reason_str	= reason;
	// return the built object
	return tcp_event;
}

/** \brief return the cnx_closed reason
 */
const std::string &tcp_event_t::get_cnx_closed_reason()				const throw()
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
bool	tcp_event_t::is_cnx_refused()						const throw()
{
	return type_val == CNX_REFUSED;
}

/** \brief set event to cnx_refused (with a possible reason)
 */
tcp_event_t tcp_event_t::build_cnx_refused(const std::string &reason)		throw()
{
	tcp_event_t	tcp_event;
	// set the type_val
	tcp_event.type_val	= CNX_REFUSED;
	tcp_event.reason_str	= reason;
	// return the built object
	return tcp_event;
}

/** \brief return the cnx_refused reason
 */
const std::string &tcp_event_t::get_cnx_refused_reason()			const throw()
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
bool	tcp_event_t::is_cnx_established()					const throw()
{
	return type_val == CNX_ESTABLISHED;
}

/** \brief set event to cnx_established
 */
tcp_event_t tcp_event_t::build_cnx_established(tcp_full_t *tcp_full)		throw()
{
	tcp_event_t	tcp_event;
	// set the type_val
	tcp_event.type_val	= CNX_ESTABLISHED;
	tcp_event.tcp_full	= tcp_full;
	// return the built object
	return tcp_event;
}

/** \brief return the cnx_refused reason
 */
tcp_full_t *tcp_event_t::get_cnx_established()					const throw()
{
	// sanity check - the event MUST be CNX_ESTABLISHED
	DBG_ASSERT( is_cnx_established() );
	// return the tcp_full
	return tcp_full;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    RECVED_DATA
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is RECVED_DATA, false otherwise
 */
bool	tcp_event_t::is_recved_data()						const throw()
{
	return type_val == RECVED_DATA;
}

/** \brief Build a tcp_event_t to RECVED_DATA
 */
tcp_event_t tcp_event_t::build_recved_data(pkt_t *pkt) 				throw()
{
	tcp_event_t	tcp_event;
	// set the type_val
	tcp_event.type_val		= RECVED_DATA;
	tcp_event.pkt_ptr		= pkt;
	// return the built object
	return tcp_event;
}

/** \brief return the tcp_full_t when RECVED_DATA
 */
pkt_t *	tcp_event_t::get_recved_data()						const throw()
{
	// sanity check - the event MUST be RECVED_DATA
	DBG_ASSERT( is_recved_data() );
	// return the pkt_ptr
	return pkt_ptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    MAYSEND_ON
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is MAYSEND_ON, false otherwise
 */
bool	tcp_event_t::is_maysend_on() const throw()
{
	return type_val == MAYSEND_ON;
}

/** \brief set event to MAYSEND_ON (with a possible reason)
 */
tcp_event_t tcp_event_t::build_maysend_on( void ) throw()
{
	tcp_event_t	tcp_event;
	// set the type_val
	tcp_event.type_val	= MAYSEND_ON;
	// return the built object
	return tcp_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    MAYSEND_OFF
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is MAYSEND_OFF, false otherwise
 */
bool	tcp_event_t::is_maysend_off() const throw()
{
	return type_val == MAYSEND_OFF;
}

/** \brief set event to MAYSEND_OFF (with a possible reason)
 */
tcp_event_t tcp_event_t::build_maysend_off( void ) throw()
{
	tcp_event_t	tcp_event;
	// set the type_val
	tcp_event.type_val	= MAYSEND_OFF;
	// return the built object
	return tcp_event;
}


NEOIP_NAMESPACE_END



