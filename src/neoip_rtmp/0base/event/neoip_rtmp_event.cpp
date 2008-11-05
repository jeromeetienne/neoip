/*! \file
    \brief Implementation of the rtmp_event_t

*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_rtmp_event.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_rtmp_pkthd.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
rtmp_event_t::rtmp_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
rtmp_event_t::~rtmp_event_t() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	rtmp_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( value() ){
	case rtmp_event_t::NONE:	oss << "NONE";				break;
	case rtmp_event_t::ERROR:	oss << "ERROR(" << rtmp_err << ")";	break;
	case rtmp_event_t::PACKET:	oss << "PACKET(pkthd=" << pkthd << " pktdata.size="
						<< pktdata.size() <<")";	break;
	default: 	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    ERROR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is ERROR, false otherwise
 */
bool	rtmp_event_t::is_error() const throw()
{
	return type_val == ERROR;
}

/** \brief build a rtmp_event_t to ERROR (with a possible reason)
 */
rtmp_event_t rtmp_event_t::build_error(const rtmp_err_t &rtmp_err)	throw()
{
	rtmp_event_t	swarm_event;
	// set the type_val
	swarm_event.type_val	= ERROR;
	swarm_event.rtmp_err	= rtmp_err;
	// return the built object
	return swarm_event;
}

/** \brief return the ERROR data
 */
const rtmp_err_t &	rtmp_event_t::get_error() const throw()
{
	// sanity check - the event MUST be ERROR
	DBG_ASSERT( is_error() );
	// return the rtmp_err
	return rtmp_err;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    PACKET
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PACKET, false otherwise
 */
bool	rtmp_event_t::is_packet() const throw()
{
	return type_val == PACKET;
}

/** \brief build a rtmp_event_t to PACKET
 */
rtmp_event_t rtmp_event_t::build_packet(const rtmp_pkthd_t &pkthd
						, const datum_t &pktdata)	throw()
{
	rtmp_event_t	swarm_event;
	// set the type_val
	swarm_event.type_val	= PACKET;
	swarm_event.pkthd	= pkthd;
	swarm_event.pktdata	= pktdata;
	// return the built object
	return swarm_event;
}

/** \brief return the PACKET data
 */
const rtmp_pkthd_t &	rtmp_event_t::get_packet(datum_t *pktdata_out) const throw()
{
	// sanity check - the event MUST be PACKET
	DBG_ASSERT( is_packet() );
	// set the pktdata_out
	if( pktdata_out )	*pktdata_out	= pktdata;
	// return the rtmp_pkthd_t
	return pkthd;
}

NEOIP_NAMESPACE_END

