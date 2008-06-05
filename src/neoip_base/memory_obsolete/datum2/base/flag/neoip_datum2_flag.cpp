/*! \file
    \brief Implementation of the datum2_flag_t

*/

/* system include */
#include <iostream>
#include <iomanip>

/* local include */
#include "neoip_datum2_flag.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;


// definition of \ref datum2_flag_t constant
const datum2_flag_t	datum2_flag_t::NOCOPY	= datum2_flag_t(1 << 0);
const datum2_flag_t	datum2_flag_t::SECMEM	= datum2_flag_t(1 << 1);
const datum2_flag_t	datum2_flag_t::FLAG_DFL	= datum2_flag_t(0);
// end of constants definition

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      inheritance
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a datum2_flag_t of all the inheritable flag
 */
datum2_flag_t	datum2_flag_t::inheritance()	const throw()
{
	// copy this object
	datum2_flag_t	tmp	= *this;
	// remove the non inheritable flags
	if( is_nocopy() )	tmp ^= NOCOPY;
	// return the result
	return tmp;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     arithmetic operator
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Perform a boolean OR operation
 */
datum2_flag_t &	datum2_flag_t::operator |=(const datum2_flag_t &other)	throw()
{
	// do the operation
	*this	= datum2_flag_t( to_uint32() | other.to_uint32() );
	// return the object itself
	return *this;
}

/** \brief Perform a boolean AND operation
 */
datum2_flag_t&		datum2_flag_t::operator &=(const datum2_flag_t &other)	throw()
{
	// do the operation
	*this	= datum2_flag_t( to_uint32() & other.to_uint32() );
	// return the object itself
	return *this;
}

/** \brief Perform a boolean XOR operation
 */
datum2_flag_t&		datum2_flag_t::operator ^=(const datum2_flag_t &other)	throw()
{
	// do the operation
	*this	= datum2_flag_t( to_uint32() ^ other.to_uint32() );
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   convert this object to a string
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert this object to a std::string
 */
std::string	datum2_flag_t::to_string()	const throw()
{
	std::ostringstream      oss;
	oss << "[";
	if( is_nocopy() )	oss << "(nocopy)";
	if( is_secmem() )	oss << "(secmem)";
	oss << "]";
	// return the built string
	return oss.str();	
}


NEOIP_NAMESPACE_END
