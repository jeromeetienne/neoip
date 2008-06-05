/*! \file
    \brief Definition of the \ref flv_taghd_video_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_flv_taghd_video.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref flv_taghd_video_t constant
const size_t	flv_taghd_video_t::TOTAL_LENGTH	= 1;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	flv_taghd_video_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( codecid().is_null() )	return true;
	if( frametype().is_null() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}
	

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	flv_taghd_video_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[" << "codecid="	<< codecid();
	oss << " " << "frametype="	<< frametype();
	oss << "]";

	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a flv_taghd_video_t
 */
serial_t& operator << (serial_t& serial, const flv_taghd_video_t &taghd_video)	throw()
{
	uint8_t	tmp	= 0;
	// build the bitfield from the flv_taghd_video_t
	tmp	+= (uint8_t(taghd_video.codecid().get_value())	& 0x0F) << 0;
	tmp	+= (uint8_t(taghd_video.frametype().get_value())& 0x0F) << 4;
	// serialize the bitfield
	serial	<< tmp;
	// return serial
	return serial;
}

/** \brief unserialze a flv_taghd_video_t
 */
serial_t& operator >> (serial_t & serial, flv_taghd_video_t &taghd_video)	throw(serial_except_t)
{
	flv_codecid_t	codecid;
	flv_frametype_t	frametype;

	// reset the destination variable
	taghd_video	= flv_taghd_video_t();

	// check that there is enougth data to contain a flv_taghd_video_t
	if( serial.length() < flv_taghd_video_t::TOTAL_LENGTH )
		nthrow_serial_plain("not enougth data");

	// unserialize the bitfield
	uint8_t	tmp;
	serial >> tmp;
	// convert the bitfield
	codecid		= flv_codecid_t::strtype_enum	((tmp & 0x0F) >> 0);
	frametype	= flv_frametype_t::strtype_enum	((tmp & 0xF0) >> 4);

	// set the returned variable
	taghd_video.codecid(codecid).frametype(frametype);

	// if the resulting object is_null(), throw an exception
	if( taghd_video.is_null() )	nthrow_serial_plain("Invalid null flv_taghd_video_t");

	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

