/*! \file
    \brief Definition of the \ref flv_taghd_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_flv_taghd.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref flv_tophd_t constant
const size_t	flv_taghd_t::TOTAL_LENGTH	= 15;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	flv_taghd_t::is_null()	const throw()
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
std::string	flv_taghd_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[" << "prevtag_size="	<< prevtag_size();
	oss << " " << "type="			<< type();
	oss << " " << "body_length="		<< body_length();
	oss << " " << "timestamp="		<< timestamp();
	oss << " " << "padding="		<< padding();
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

/** \brief serialize a flv_taghd_t
 */
serial_t& operator << (serial_t& serial, const flv_taghd_t &flv_taghd)		throw()
{
	serial << flv_taghd.prevtag_size();
	serial << flv_taghd.type();
	DOSERIAL_24BIT( flv_taghd.body_length() );
	DOSERIAL_24BIT( flv_taghd.timestamp().to_msec_32bit() );
	serial << flv_taghd.padding();
	// return serial
	return serial;
}

/** \brief unserialze a flv_taghd_t
 */
serial_t& operator >> (serial_t & serial, flv_taghd_t &flv_taghd)		throw(serial_except_t)
{
	uint32_t	prevtag_size;
	flv_tagtype_t	type;
	size_t		body_length;
	uint32_t	timestamp_ms;
	uint32_t	padding;

	// reset the destination variable
	flv_taghd	= flv_taghd_t();

	// check that there is enougth data to contain a flv_taghd_t
	if( serial.length() < flv_taghd_t::TOTAL_LENGTH )
		nthrow_serial_plain("not enougth data");

	serial >> prevtag_size;
	serial >> type;
	UNSERIAL_24BIT(body_length);
	UNSERIAL_24BIT(timestamp_ms);
	serial >> padding;

	// set the returned variable
	flv_taghd.prevtag_size(prevtag_size).type(type).body_length(body_length)
				.timestamp(delay_t::from_msec(timestamp_ms))
				.padding(padding);

	// if the resulting object is_null(), throw an exception
	if( flv_taghd.is_null() )	nthrow_serial_plain("Invalid null flv_taghd_t");

	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

