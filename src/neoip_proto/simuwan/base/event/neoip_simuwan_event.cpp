/*! \file
    \brief Implementation of the simuwan_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_simuwan_event.hpp"
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
simuwan_event_t::simuwan_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
simuwan_event_t::~simuwan_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const simuwan_event_t &event) throw()
{
	switch( event.type_val ){
	case simuwan_event_t::NONE:
		os << "NONE";
		break;
	case simuwan_event_t::PKT_TO_LOWER:
		os << "PKT_TO_LOWER (" << (event.get_pkt_to_lower())->get_len() << "-byte)";
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
bool	simuwan_event_t::is_pkt_to_lower() const throw()
{
	return type_val == PKT_TO_LOWER;
}

/** \brief Build a simuwan_event_t to PKT_TO_LOWER
 */
simuwan_event_t simuwan_event_t::build_pkt_to_lower( pkt_t *pkt ) 
						throw()
{
	simuwan_event_t	simuwan_event;
	// set the type_val
	simuwan_event.type_val	= PKT_TO_LOWER;
	simuwan_event.pkt_ptr	= pkt;
	// return the built object
	return simuwan_event;
}

/** \brief return the (pkt_t *) when PKT_TO_LOWER
 */
pkt_t *simuwan_event_t::get_pkt_to_lower() const throw()
{
	// sanity check - the event MUST be PKT_TO_LOWER
	DBG_ASSERT( is_pkt_to_lower() );
	// return the datum_ptr
	return pkt_ptr;
}


NEOIP_NAMESPACE_END

