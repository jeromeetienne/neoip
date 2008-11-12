/*! \file
    \brief Definition of the \ref rtmp_pkthd_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_rtmp_pkthd.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref flv_tophd_t constant
const size_t	rtmp_pkthd_t::CHUNK_MAXLEN	= 128;
const size_t	rtmp_pkthd_t::PKTHD_MAXLEN	= 12;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	rtmp_pkthd_t::is_null()	const throw()
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
std::string	rtmp_pkthd_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[" << "channel_id="	<< int(channel_id());
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

/** \brief serialize a rtmp_pkthd_t
 *
 * - always serialize the whole rtmp_pkthd_t
 * - dont use packet compression
 */
serial_t& operator << (serial_t& serial, const rtmp_pkthd_t &rtmp_pkthd)		throw()
{
	serial << rtmp_pkthd.channel_id();
	DOSERIAL_24BIT( rtmp_pkthd.timestamp().to_msec_32bit() );
	DOSERIAL_24BIT( rtmp_pkthd.body_length() );
	serial << rtmp_pkthd.type();
	// stream_id is coded in little endian so swapping it before encoding in usual network big endian
	serial << NEOIP_BSWAP32(rtmp_pkthd.stream_id());
	// return serial
	return serial;
}

/** \brief unserialze a rtmp_pkthd_t
 */
serial_t& operator >> (serial_t & serial, rtmp_pkthd_t &rtmp_pkthd)		throw(serial_except_t)
{
	static const size_t	pkthd_len_arr[]	= {12, 8, 4, 1};
	size_t			pkthd_len;
	uint8_t			first_byte;
	uint8_t		channel_id;
	uint32_t	timestamp_ms;
	size_t		body_length;
	rtmp_type_t	type;
	uint32_t	stream_id;

	// DO NOT reset the destination variable
	// - this is a trick to allow the caller to set some default values
	// - this is used for handle the "packet header compression" of rtmp_parse_t

	// get the first byte
	serial >> first_byte;
	pkthd_len	= pkthd_len_arr[(first_byte >> 6) & 0x3];
	// get and set the channel_id
	channel_id	= (first_byte & 0x3f);
	rtmp_pkthd.channel_id(channel_id);
	// get and set the timestamp IIF pkthd_len >= 4
	if( pkthd_len >= 4 ){
		UNSERIAL_24BIT(timestamp_ms);
		rtmp_pkthd.timestamp(delay_t::from_msec(timestamp_ms));
	}
	// get and set the body_length+type IIF pkthd_len >= 8
	if( pkthd_len >= 8 ){
		UNSERIAL_24BIT(body_length);
		rtmp_pkthd.body_length(body_length);
		serial >> type;
		rtmp_pkthd.type(type);
	}
	// get and set the stream_id IIF pkthd_len == 12
	if( pkthd_len == 12 ){
		serial >> stream_id;
		// stream_id is coded in little endian so swapping it before encoding in usual network big endian
		rtmp_pkthd.stream_id(NEOIP_BSWAP32(stream_id));
	}

	// if the resulting object is_null(), throw an exception
	if( rtmp_pkthd.is_null() )	nthrow_serial_plain("Invalid null rtmp_pkthd_t");

	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

