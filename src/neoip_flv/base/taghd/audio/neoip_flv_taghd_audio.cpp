/*! \file
    \brief Definition of the \ref flv_taghd_audio_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_flv_taghd_audio.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref flv_taghd_audio_t constant
const size_t	flv_taghd_audio_t::TOTAL_LENGTH	= 1;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	flv_taghd_audio_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( soundtype().is_null() )	return true;
	if( soundsize().is_null() )	return true;
	if( soundrate().is_null() )	return true;
	if( soundformat().is_null() )	return true;
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
std::string	flv_taghd_audio_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[" << "soundtype="	<< soundtype();
	oss << " " << "soundsize="	<< soundsize();
	oss << " " << "soundrate="	<< soundrate();
	oss << " " << "soundformat="	<< soundformat();
	oss << "]";

	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a flv_taghd_audio_t
 */
serial_t& operator << (serial_t& serial, const flv_taghd_audio_t &taghd_audio)	throw()
{
	uint8_t	tmp	= 0;
	// build the bitfield from the flv_taghd_audio_t
	tmp	+= (uint8_t(taghd_audio.soundtype().get_value())	& 0x01) << 0;
	tmp	+= (uint8_t(taghd_audio.soundsize().get_value())	& 0x01) << 1;
	tmp	+= (uint8_t(taghd_audio.soundrate().get_value())	& 0x03) << 2;
	tmp	+= (uint8_t(taghd_audio.soundformat().get_value())	& 0x0F) << 4;
	// serialize the bitfield
	serial	<< tmp;
	// return serial
	return serial;
}

/** \brief unserialze a flv_taghd_audio_t
 */
serial_t& operator >> (serial_t & serial, flv_taghd_audio_t &taghd_audio)	throw(serial_except_t)
{
	flv_soundtype_t		soundtype;
	flv_soundsize_t		soundsize;
	flv_soundrate_t		soundrate;
	flv_soundformat_t	soundformat;

	// reset the destination variable
	taghd_audio	= flv_taghd_audio_t();

	// check that there is enougth data to contain a flv_taghd_audio_t
	if( serial.length() < flv_taghd_audio_t::TOTAL_LENGTH )
		nthrow_serial_plain("not enougth data");

	// unserialize the bitfield
	uint8_t	tmp;
	serial >> tmp;
	// convert the bitfield
	soundtype	= flv_soundtype_t::strtype_enum		((tmp & 0x01) >> 0);
	soundsize	= flv_soundsize_t::strtype_enum		((tmp & 0x02) >> 1);
	soundrate	= flv_soundrate_t::strtype_enum		((tmp & 0x0C) >> 2);
	soundformat	= flv_soundformat_t::strtype_enum	((tmp & 0xF0) >> 4);

	// set the returned variable
	taghd_audio.soundtype(soundtype).soundsize(soundsize).soundrate(soundrate)
			.soundformat(soundformat);

	// if the resulting object is_null(), throw an exception
	if( taghd_audio.is_null() )	nthrow_serial_plain("Invalid null flv_taghd_audio_t");

	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

