/*! \file
    \brief Implementation of the flv_parse_event_t

*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_flv_parse_event.hpp"
#include "neoip_flv_err.hpp"
#include "neoip_flv_tophd.hpp"
#include "neoip_flv_taghd.hpp"
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
flv_parse_event_t::flv_parse_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
flv_parse_event_t::~flv_parse_event_t() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	flv_parse_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( value() ){
	case flv_parse_event_t::NONE:		oss << "NONE";				break;
	case flv_parse_event_t::ERROR:		oss << "ERROR(" << flv_err << ")";	break;
	case flv_parse_event_t::TOPHD:		oss << "TOPHD(" << tophd << ")";	break;
	case flv_parse_event_t::TAG:		oss << "TAG(taghd=" << taghd << " tag_data.size="
							<< tagdata.size() <<")";	break;
	default: 	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			misc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the length of the data for this flv_parse_event_t
 */
size_t		flv_parse_event_t::byte_length()	const throw()
{
	size_t	bytelen;
	// sanity check - this object MUST be is_tophd() or is_tag()
	DBG_ASSERT( is_tophd() || is_tag() );
	// determine the bytelen according to the flv_parse_event_t type
	if( is_tophd() )	bytelen	= flv_tophd_t::TOTAL_LENGTH;
	else			bytelen	= flv_taghd_t::TOTAL_LENGTH + taghd.body_length();
	// return the bytelen
	return	bytelen;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    ERROR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is ERROR, false otherwise
 */
bool	flv_parse_event_t::is_error() const throw()
{
	return type_val == ERROR;
}

/** \brief build a flv_parse_event_t to ERROR (with a possible reason)
 */
flv_parse_event_t flv_parse_event_t::build_error(const flv_err_t &flv_err)	throw()
{
	flv_parse_event_t	swarm_event;
	// set the type_val
	swarm_event.type_val	= ERROR;
	swarm_event.flv_err	= flv_err;
	// return the built object
	return swarm_event;
}

/** \brief return the ERROR data
 */
const flv_err_t &	flv_parse_event_t::get_error() const throw()
{
	// sanity check - the event MUST be ERROR
	DBG_ASSERT( is_error() );
	// return the flv_err
	return flv_err;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    TOPHD
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is TOPHD, false otherwise
 */
bool	flv_parse_event_t::is_tophd() const throw()
{
	return type_val == TOPHD;
}

/** \brief build a flv_parse_event_t to TOPHD (with a possible reason)
 */
flv_parse_event_t flv_parse_event_t::build_tophd(const flv_tophd_t &tophd)	throw()
{
	flv_parse_event_t	swarm_event;
	// set the type_val
	swarm_event.type_val	= TOPHD;
	swarm_event.tophd	= tophd;
	// return the built object
	return swarm_event;
}

/** \brief return the TOPHD data
 */
const flv_tophd_t & 	flv_parse_event_t::get_tophd() const throw()
{
	// sanity check - the event MUST be TOPHD
	DBG_ASSERT( is_tophd() );
	// return the flv_tophd_t
	return tophd;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    TAG
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is TAG, false otherwise
 */
bool	flv_parse_event_t::is_tag() const throw()
{
	return type_val == TAG;
}

/** \brief build a flv_parse_event_t to TAG
 */
flv_parse_event_t flv_parse_event_t::build_tag(const flv_taghd_t &taghd
						, const datum_t &tagdata)	throw()
{
	flv_parse_event_t	swarm_event;
	// set the type_val
	swarm_event.type_val	= TAG;
	swarm_event.taghd	= taghd;
	swarm_event.tagdata	= tagdata;
	// return the built object
	return swarm_event;
}

/** \brief return the TAG data
 */
const flv_taghd_t &	flv_parse_event_t::get_tag(datum_t *tagdata_out) const throw()
{
	// sanity check - the event MUST be TAG
	DBG_ASSERT( is_tag() );
	// set the tagdata_out
	if( tagdata_out )	*tagdata_out	= tagdata;
	// return the flv_tafhd_t
	return taghd;	
}

NEOIP_NAMESPACE_END

