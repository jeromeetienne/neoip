/*! \file
    \brief Definition of the \ref bt_handshake_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_handshake.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_handshake_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( protocolid().empty() )	return true;
	if( infohash().is_null() )	return true;
	if( peerid().is_null() )	return true;
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
std::string	bt_handshake_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "protocolid="	<< protocolid();
	oss << " " << "protoflag="	<< protoflag();
	oss << " " << "infohash="	<< infohash();
	oss << " " << "peerid="		<< peerid();
	
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_handshake_t
 */
serial_t& operator << (serial_t& serial, const bt_handshake_t &handshake)		throw()
{
	// serialize the peerid/portid
	serial << uint8_t(handshake.protocolid().size());
	serial.append( handshake.protocolid().c_str(), handshake.protocolid().size());
	serial << handshake.protoflag();
	serial << handshake.infohash();
	serial << handshake.peerid();
	// return serial
	return serial;
}

/** \brief unserialze a bt_handshake_t
 */
serial_t& operator >> (serial_t & serial, bt_handshake_t &handshake)		throw(serial_except_t)
{
	uint8_t		protid_len;
	std::string	protid_str;
	bt_protoflag_t	protoflag;
	bt_id_t		infohash;
	bt_id_t		peerid;

	// reset the destination variable
	handshake	= bt_handshake_t();

	// unserialize the data
	serial >> protid_len;
	// read the protid_str
	if( serial.get_len() < (ssize_t)protid_len)	nthrow_serial_plain("protocolid Short");
	protid_str	= std::string((char*)serial.get_data(), protid_len);
	serial.consume( protid_len );
	serial >> protoflag;
	serial >> infohash;
	serial >> peerid;

	// set the returned variable
	handshake.protocolid(protid_str).protoflag(protoflag).infohash(infohash).peerid(peerid);
	// return serial
	return serial;
}
NEOIP_NAMESPACE_END

