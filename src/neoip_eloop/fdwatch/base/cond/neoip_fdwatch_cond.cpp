/*! \file
    \brief Implementation of the fdwatch_cond_t
    
- TODO should be ported on top of bitflag_t

*/

/* system include */
#include <iostream>
#include <iomanip>
/* local include */
#include "neoip_fdwatch_cond.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN;

// definition of \ref fdwatch_cond_t constant
const fdwatch_cond_t	fdwatch_cond_t::NONE		= fdwatch_cond_t(0);
const fdwatch_cond_t	fdwatch_cond_t::INPUT		= fdwatch_cond_t(1 << 0);
const fdwatch_cond_t	fdwatch_cond_t::OUTPUT		= fdwatch_cond_t(1 << 1);
const fdwatch_cond_t	fdwatch_cond_t::ERROR		= fdwatch_cond_t(1 << 2);
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     arithmetic operator
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Perform a boolean OR operation
 */
fdwatch_cond_t &	fdwatch_cond_t::operator |=(const fdwatch_cond_t &other)	throw()
{
	// do the operation
	*this	= fdwatch_cond_t( to_uint32() | other.to_uint32() );
	// return the object itself
	return *this;
}

/** \brief Perform a boolean AND operation
 */
fdwatch_cond_t&	fdwatch_cond_t::operator &=(const fdwatch_cond_t &other)	throw()
{
	// do the operation
	*this	= fdwatch_cond_t( to_uint32() & other.to_uint32() );
	// return the object itself
	return *this;
}

/** \brief Perform a boolean XOR operation
 */
fdwatch_cond_t&	fdwatch_cond_t::operator ^=(const fdwatch_cond_t &other)	throw()
{
	// do the operation
	*this	= fdwatch_cond_t( to_uint32() ^ other.to_uint32() );
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
std::string	fdwatch_cond_t::to_string()	const throw()
{
	std::ostringstream      oss;
	oss << "[";
	if( is_input() )	oss << "(input)";
	if( is_output() )	oss << "(output)";
	if( is_error() )	oss << "(error)";
	oss << "]";
	// return the built string
	return oss.str();	
}


NEOIP_NAMESPACE_END
