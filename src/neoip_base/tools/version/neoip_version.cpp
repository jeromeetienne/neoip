/*! \file
    \brief Implementation of the version_t

*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_version.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief constructor based on a string
 */
version_t::version_t(const char *str)	throw()
{
	std::vector<std::string>	part = string_t::split(OSTREAMSTR(str), ".", 2);
	// report an error - TODO quite crappy to assert... (i dont want to do all the isnull stuff now)
	if( part.size() != 2 )	DBG_ASSERT( 0 );
	// set the value
	v_major	= atoi( part[0].c_str() );
	v_minor	= atoi( part[1].c_str() );
}

/** \brief return true if the two version are compatible (aka have the same major)
 */
bool version_t::is_compatible( const version_t &other ) const throw()
{
	return this->get_major() == other.get_major();
}

/** \brief overload the << operator
 */
std::string version_t::to_string()	const throw()
{
	std::ostringstream	oss;
	oss << (int)get_major() << "." << (int)get_minor();
	return oss.str();
}

/** \brief compare 2 version_t and return value ala strcmp/memcmp
 */
int	version_t::compare( const version_t &other )	const throw()
{
	// compare major
	if( get_major() > other.get_major() )	return +1;
	if( get_major() < other.get_major() )	return -1;

	// compare minor
	if( get_minor() > other.get_minor() )	return +1;
	if( get_minor() < other.get_minor() )	return -1;
	
	// here they are both equal
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial version_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

serial_t& operator << ( serial_t& serial, const version_t &version )	throw()
{
	return serial << version.get_major() << version.get_minor();
}

serial_t& operator >> ( serial_t& serial, version_t &version )		throw(serial_except_t)
{
	return serial >> version.v_major >> version.v_minor;
}

NEOIP_NAMESPACE_END
