/*! \file
    \brief Implementation of the scnx_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_scnx_event.hpp"
#include "neoip_scnx_full.hpp"
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
scnx_event_t::scnx_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
scnx_event_t::~scnx_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const scnx_event_t &event) throw()
{
	switch( event.type_val ){
	case scnx_event_t::NONE:
		os << "NONE";
		break;
	case scnx_event_t::CNX_REFUSED:
		os << "CNX_REFUSED (reason: " << event.get_cnx_refused_reason() << ")";
		break;
	case scnx_event_t::CNX_ESTABLISHED:
		//os << "CNX_ESTABLISHED (scnx_full_t=" << *event.get_cnx_established();
		os << "CNX_ESTABLISHED";
		break;	
	case scnx_event_t::PKT_TO_LOWER:
		os << "PKT_TO_LOWER (" << (event.get_pkt_to_lower())->get_len() << "-byte)";
		break;	
	default: 	DBG_ASSERT(0);
	}
	return os;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_REFUSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_REFUSED, false otherwise
 */
bool	scnx_event_t::is_cnx_refused() const throw()
{
	return type_val == CNX_REFUSED;
}

/** \brief build a scnx_event_t to CNX_REFUSED (with a possible reason)
 */
scnx_event_t scnx_event_t::build_cnx_refused(const std::string &reason )	throw()
{
	scnx_event_t	scnx_event;
	// set the type_val
	scnx_event.type_val	= CNX_REFUSED;
	scnx_event.reason	= reason;
	// return the built object
	return scnx_event;
}

/** \brief return the CNX_REFUSED reason
 */
const std::string &scnx_event_t::get_cnx_refused_reason() const throw()
{
	// sanity check - the event MUST be CNX_REFUSED
	DBG_ASSERT( is_cnx_refused() );
	// return the reason
	return reason;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_ESTABLISHED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_ESTABLISHED, false otherwise
 */
bool	scnx_event_t::is_cnx_established() const throw()
{
	return type_val == CNX_ESTABLISHED;
}

/** \brief Build a scnx_event_t to CNX_ESTABLISHED
 */
scnx_event_t scnx_event_t::build_cnx_established( scnx_full_t *scnx_full, void *auxnego_ptr) 
						throw()
{
	scnx_event_t	scnx_event;
	// set the type_val
	scnx_event.type_val	= CNX_ESTABLISHED;
	scnx_event.scnx_full	= scnx_full;
	scnx_event.auxnego_ptr	= auxnego_ptr;
	// return the built object
	return scnx_event;
}

/** \brief return the scnx_full_t when CNX_ESTABLISHED
 */
scnx_full_t *scnx_event_t::get_cnx_established(void **auxnego_ptr) const throw()
{
	// sanity check - the event MUST be CNX_ESTABLISHED
	DBG_ASSERT( is_cnx_established() );
	// copy the auxnego_ptr
	if( auxnego_ptr )	*auxnego_ptr	= this->auxnego_ptr;
	// return the scnx_full
	return scnx_full;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PKT_TO_LOWER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PKT_TO_LOWER, false otherwise
 */
bool	scnx_event_t::is_pkt_to_lower()						const throw()
{
	return type_val == PKT_TO_LOWER;
}

/** \brief Build a scnx_event_t to PKT_TO_LOWER
 */
scnx_event_t scnx_event_t::build_pkt_to_lower( pkt_t *pkt ) 			throw()
{
	scnx_event_t	scnx_event;
	// set the type_val
	scnx_event.type_val	= PKT_TO_LOWER;
	scnx_event.pkt_ptr	= pkt;
	// return the built object
	return scnx_event;
}

/** \brief return the neoip_scnx_full when PKT_TO_LOWER
 */
pkt_t *scnx_event_t::get_pkt_to_lower()						const throw()
{
	// sanity check - the event MUST be PKT_TO_LOWER
	DBG_ASSERT( is_pkt_to_lower() );
	// return the datum_ptr
	return pkt_ptr;
}


NEOIP_NAMESPACE_END

