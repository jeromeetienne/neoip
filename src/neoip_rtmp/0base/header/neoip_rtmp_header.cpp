/*! \file
    \brief Definition of the \ref rtmp_header_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_rtmp_header.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref flv_tophd_t constant
const size_t	rtmp_header_t::TOTAL_LENGTH	= 12;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	rtmp_header_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( type().is_null() )	return true;
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
std::string	rtmp_header_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[" << "channel_id="	<< channel_id();
	oss << " " << "timestamp="	<< timestamp();
	oss << " " << "body_length="	<< body_length();
	oss << " " << "type="		<< type();
	oss << " " << "stream_id="	<< stream_id();
	oss << "]";

	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//! special serial for serializing 32bit type into 24bit in serial_t
#define DOSERIAL_24BIT(value)					\
	do {							\
		DBG_ASSERT( value <= 0xFFFFFF );		\
		serial << uint8_t( (value >> 16) & 0xFF );	\
		serial << uint8_t( (value >>  8) & 0xFF );	\
		serial << uint8_t( (value >>  0) & 0xFF );	\
	} while(0)
//! special serial for unserializing 32bit type from 24bit in serial_t
#define UNSERIAL_24BIT(value)					\
	do {							\
		uint8_t	tmp;					\
		value	= 0;					\
		serial >> tmp;					\
		value	+= uint32_t(tmp) << 16;			\
		serial >> tmp;					\
		value	+= uint32_t(tmp) <<  8;			\
		serial >> tmp;					\
		value	+= uint32_t(tmp) <<  0;			\
	} while(0)

/** \brief serialize a rtmp_header_t
 */
serial_t& operator << (serial_t& serial, const rtmp_header_t &rtmp_header)		throw()
{
	serial << rtmp_header.channel_id();
	DOSERIAL_24BIT( rtmp_header.timestamp().to_msec_32bit() );
	DOSERIAL_24BIT( rtmp_header.body_length() );
	serial << rtmp_header.type();
	serial << rtmp_header.stream_id();
	// return serial
	return serial;
}

/** \brief unserialze a rtmp_header_t
 */
serial_t& operator >> (serial_t & serial, rtmp_header_t &rtmp_header)		throw(serial_except_t)
{
	uint8_t		channel_id;
	uint32_t	timestamp_ms;
	size_t		body_length;
	rtmp_type_t	type;
	uint32_t	stream_id;

	// reset the destination variable
	rtmp_header	= rtmp_header_t();

	// check that there is enougth data to contain a rtmp_header_t
	if( serial.length() < rtmp_header_t::TOTAL_LENGTH )
		nthrow_serial_plain("not enougth data");

	// TODO handle the 2 first bit which give the length of the header
	serial >> channel_id;
	UNSERIAL_24BIT(timestamp_ms);
	UNSERIAL_24BIT(body_length);
	serial >> type;
	serial >> stream_id;

	// set the returned variable
	rtmp_header.channel_id(channel_id).timestamp(delay_t::from_msec(timestamp_ms))
				.body_length(body_length).type(type)
				.stream_id(stream_id);

	// if the resulting object is_null(), throw an exception
	if( rtmp_header.is_null() )	nthrow_serial_plain("Invalid null rtmp_header_t");

	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

