/*! \file
    \brief Definition of the \ref ndiag_cacheport_item_t

*/


/* system include */
/* local include */
#include "neoip_ndiag_cacheport_item.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ndiag_cacheport_item_t and return value ala strcmp/memcmp
 */
int	ndiag_cacheport_item_t::compare(const ndiag_cacheport_item_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// handle the key
	if( key() < other.key() )		return -1;
	if( key() > other.key() )		return +1;
	// handle the sockfam
	if( sockfam() < other.sockfam() )	return -1;
	if( sockfam() > other.sockfam() )	return +1;
	// handle the port
	if( port() < other.port() )		return -1;
	if( port() > other.port() )		return +1;
	// here both are considered equal, so return 0
	return 0;
}
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	ndiag_cacheport_item_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[key="		<< key();
	oss << " sockfam="	<< sockfam();
	oss << " port="		<< port();
	oss << "]";
	// return the just built string
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ndiag_cacheport_item_t
 */
serial_t& operator << (serial_t& serial, const ndiag_cacheport_item_t &cacheport_item)		throw()
{
	// serialize the ndiag_cacheport_item_t
	serial << cacheport_item.key();
	serial << cacheport_item.sockfam();
	serial << cacheport_item.port();
	// return serial
	return serial;
}

/** \brief unserialze a ndiag_cacheport_t
 * 
 * - support null ndiag_cacheport_t
 */
serial_t& operator >> (serial_t & serial, ndiag_cacheport_item_t &cacheport_item)throw(serial_except_t)
{
	std::string	key;
	upnp_sockfam_t	sockfam;
	uint16_t	port;
	// unserialize the ndiag_cacheport_item_t fields
	serial >> key;
	serial >> sockfam;
	serial >> port;
	// set the destination value
	cacheport_item	= ndiag_cacheport_item_t(key, sockfam, port);
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

