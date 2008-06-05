/*! \file
    \brief Implementation of the ordgram_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_ordgram_event.hpp"
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
ordgram_event_t::ordgram_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
ordgram_event_t::~ordgram_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const ordgram_event_t &event) throw()
{
	switch( event.type_val ){
	case ordgram_event_t::NONE:
		os << "NONE";
		break;
	case ordgram_event_t::PKT_TO_LOWER:
		os << "PKT_TO_LOWER (" << (event.get_pkt_to_lower())->get_len() << "-byte)";
		break;	
	case ordgram_event_t::PKT_TO_UPPER:
		os << "PKT_TO_UPPER (" << (event.get_pkt_to_upper())->get_len() << "-byte)";
		break;	
	case ordgram_event_t::MAYSEND_ON:
		os << "MAYSEND_ON";
		break;	
	case ordgram_event_t::MAYSEND_OFF:
		os << "MAYSEND_OFF";
		break;	
	default: 	DBG_ASSERT(0);
	}
	return os;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PKT_TO_LOWER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PKT_TO_LOWER, false otherwise
 */
bool	ordgram_event_t::is_pkt_to_lower() const throw()
{
	return type_val == PKT_TO_LOWER;
}

/** \brief Build a ordgram_event_t to PKT_TO_LOWER
 */
ordgram_event_t ordgram_event_t::build_pkt_to_lower( pkt_t *pkt ) 
						throw()
{
	ordgram_event_t	ordgram_event;
	// set the type_val
	ordgram_event.type_val	= PKT_TO_LOWER;
	ordgram_event.pkt_ptr	= pkt;
	// return the built object
	return ordgram_event;
}

/** \brief return the (pkt_t *) when PKT_TO_LOWER
 */
pkt_t *ordgram_event_t::get_pkt_to_lower() const throw()
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
bool	ordgram_event_t::is_pkt_to_upper() const throw()
{
	return type_val == PKT_TO_UPPER;
}

/** \brief Build a ordgram_event_t to PKT_TO_UPPER
 */
ordgram_event_t ordgram_event_t::build_pkt_to_upper( pkt_t *pkt ) 
						throw()
{
	ordgram_event_t	ordgram_event;
	// set the type_val
	ordgram_event.type_val	= PKT_TO_UPPER;
	ordgram_event.pkt_ptr	= pkt;
	// return the built object
	return ordgram_event;
}

/** \brief return the (pkt_t *) when PKT_TO_UPPER
 */
pkt_t *ordgram_event_t::get_pkt_to_upper() const throw()
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
bool	ordgram_event_t::is_maysend_on() const throw()
{
	return type_val == MAYSEND_ON;
}

/** \brief Build a ordgram_event_t to MAYSEND_ON
 */
ordgram_event_t ordgram_event_t::build_maysend_on() throw()
{
	ordgram_event_t	ordgram_event;
	// set the type_val
	ordgram_event.type_val		= MAYSEND_ON;
	// return the built object
	return ordgram_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    MAYSEND_OFF
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Return true is the type is MAYSEND_OFF, false otherwise
 */
bool	ordgram_event_t::is_maysend_off() const throw()
{
	return type_val == MAYSEND_OFF;
}

/** \brief Build a ordgram_event_t to MAYSEND_OFF
 */
ordgram_event_t ordgram_event_t::build_maysend_off() throw()
{
	ordgram_event_t	ordgram_event;
	// set the type_val
	ordgram_event.type_val		= MAYSEND_OFF;
	// return the built object
	return ordgram_event;
}

NEOIP_NAMESPACE_END

