/*! \file
    \brief Implementation of the rdgram_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_rdgram_event.hpp"
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
rdgram_event_t::rdgram_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
rdgram_event_t::~rdgram_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const rdgram_event_t &event) throw()
{
	switch( event.type_val ){
	case rdgram_event_t::NONE:
		os << "NONE";
		break;
	case rdgram_event_t::PKT_TO_LOWER:
		os << "PKT_TO_LOWER (" << (event.get_pkt_to_lower())->get_len() << "-byte)";
		break;	
	case rdgram_event_t::MAYSEND_ON:
		os << "MAYSEND_ON";
		break;	
	case rdgram_event_t::MAYSEND_OFF:
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
bool	rdgram_event_t::is_pkt_to_lower() const throw()
{
	return type_val == PKT_TO_LOWER;
}

/** \brief Build a rdgram_event_t to PKT_TO_LOWER
 */
rdgram_event_t rdgram_event_t::build_pkt_to_lower( pkt_t *pkt ) 
						throw()
{
	rdgram_event_t	rdgram_event;
	// set the type_val
	rdgram_event.type_val	= PKT_TO_LOWER;
	rdgram_event.pkt_ptr	= pkt;
	// return the built object
	return rdgram_event;
}

/** \brief return the (pkt_t *) when PKT_TO_LOWER
 */
pkt_t *rdgram_event_t::get_pkt_to_lower() const throw()
{
	// sanity check - the event MUST be PKT_TO_LOWER
	DBG_ASSERT( is_pkt_to_lower() );
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
bool	rdgram_event_t::is_maysend_on() const throw()
{
	return type_val == MAYSEND_ON;
}

/** \brief Build a rdgram_event_t to MAYSEND_ON
 */
rdgram_event_t rdgram_event_t::build_maysend_on() throw()
{
	rdgram_event_t	rdgram_event;
	// set the type_val
	rdgram_event.type_val		= MAYSEND_ON;
	// return the built object
	return rdgram_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    MAYSEND_OFF
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Return true is the type is MAYSEND_OFF, false otherwise
 */
bool	rdgram_event_t::is_maysend_off() const throw()
{
	return type_val == MAYSEND_OFF;
}

/** \brief Build a rdgram_event_t to MAYSEND_OFF
 */
rdgram_event_t rdgram_event_t::build_maysend_off() throw()
{
	rdgram_event_t	rdgram_event;
	// set the type_val
	rdgram_event.type_val		= MAYSEND_OFF;
	// return the built object
	return rdgram_event;
}

NEOIP_NAMESPACE_END

