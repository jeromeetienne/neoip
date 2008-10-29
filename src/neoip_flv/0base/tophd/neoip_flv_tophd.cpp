/*! \file
    \brief Definition of the \ref flv_tophd_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_flv_tophd.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref flv_tophd_t constant
const size_t	flv_tophd_t::TOTAL_LENGTH	= 9;
const size_t	flv_tophd_t::OFFSET_VALUE	= 9;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	flv_tophd_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( version() == 0 )	return true;
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
std::string	flv_tophd_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[" << "version="	<< int(version());
	oss << " " << "flag="		<< flag();
	oss << "]";

	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a flv_tophd_t
 */
serial_t& operator << (serial_t& serial, const flv_tophd_t &flv_tophd)		throw()
{
	// serialize the header marker 'FLV' - which is constant
	serial << uint8_t('F') << uint8_t('L') << uint8_t('V');
	// serialize the version
	serial << flv_tophd.version();
	// serialize the flag
	serial << flv_tophd.flag();
	// serialize the offset - aka the total header length - which is constant
	serial << uint32_t(flv_tophd_t::OFFSET_VALUE);
	// return serial
	return serial;
}

/** \brief unserialze a flv_tophd_t
 */
serial_t& operator >> (serial_t & serial, flv_tophd_t &flv_tophd)		throw(serial_except_t)
{
	uint8_t			version;
	flv_tophd_flag_t	flag;
	uint32_t		offset;

	// reset the destination variable
	flv_tophd	= flv_tophd_t();

	// check that there is enougth data to contain a flv_tophd_t
	if( serial.length() < flv_tophd_t::TOTAL_LENGTH )
		nthrow_serial_plain("not enougth data");

	// check the header marker
	uint8_t	c1, c2, c3;
	serial >> c1 >> c2 >> c3;
	if( c1 != 'F' || c2 != 'L' || c3 != 'V' )
		nthrow_serial_plain("Invalid header marker");

	// unserialize the version of this .flv
	serial >> version;
	// unserialize the flv_tophd_flag_t
	serial >> flag;
	// check the value of the offset
	serial >> offset;
	if( offset != flv_tophd_t::OFFSET_VALUE )
		nthrow_serial_plain("Invalid offset length");

	// set the returned variable
	flv_tophd.version(version).flag(flag);
	
	// if the resulting object is_null(), throw an exception
	if( flv_tophd.is_null() )	nthrow_serial_plain("Invalid null flv_tophd_t");
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

