/*! \file
    \brief Implementation of the nslan_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_nslan_event.hpp"
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
nslan_event_t::nslan_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
nslan_event_t::~nslan_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const nslan_event_t &event) throw()
{
	switch( event.type_val ){
	case nslan_event_t::NONE:
		os << "NONE";
		break;
	case nslan_event_t::TIMEDOUT:	
		os << "TIMEDOUT (" << event.get_timedout_reason() << ")";
		break;	
	case nslan_event_t::GOT_RECORD:{
		ipport_addr_t	saddr;
		nslan_rec_t	record	= event.get_got_record(&saddr);
		os << "GOT_RECORD (record=" << record << " source_addr=" << saddr << ")";
		break;}
	default: 	DBG_ASSERT(0);
	}
	return os;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    TIMEDOUT
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is TIMEDOUT, false otherwise
 */
bool	nslan_event_t::is_timedout() const throw()
{
	return type_val == TIMEDOUT;
}

/** \brief Build a nslan_event_t to TIMEDOUT
 */
nslan_event_t nslan_event_t::build_timedout(const std::string &reason) 	throw()
{
	nslan_event_t	nslan_event;
	// set the type_val
	nslan_event.type_val	= TIMEDOUT;
	nslan_event.reason	= reason;
	// return the built object
	return nslan_event;
}

/** \brief return the (pkt_t *) when TIMEDOUT
 */
const std::string &nslan_event_t::get_timedout_reason() const throw()
{
	// sanity check - the event MUST be TIMEDOUT
	DBG_ASSERT( is_timedout() );
	// return the reason
	return reason;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    GOT_RECORD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is GOT_RECORD, false otherwise
 */
bool	nslan_event_t::is_got_record() const throw()
{
	return type_val == GOT_RECORD;
}

/** \brief Build a nslan_event_t to GOT_RECORD
 */
nslan_event_t nslan_event_t::build_got_record(const nslan_rec_t &nslan_rec
					, const ipport_addr_t &source_addr) 	throw()
{
	nslan_event_t	nslan_event;
	// set the type_val
	nslan_event.type_val	= GOT_RECORD;
	nslan_event.nslan_rec			= nslan_rec;
	nslan_event.nslan_rec_source_addr	= source_addr;
	// return the built object
	return nslan_event;
}

/** \brief return the nslan_rec_t when GOT_RECORD
 */
const nslan_rec_t &nslan_event_t::get_got_record(ipport_addr_t *source_addr) const throw()
{
	// sanity check - the event MUST be GOT_RECORD
	DBG_ASSERT( is_got_record() );
	// set the source_addr if not null
	if( source_addr )	*source_addr	= nslan_rec_source_addr;
	// return the nslan_rec
	return nslan_rec;
}

NEOIP_NAMESPACE_END

