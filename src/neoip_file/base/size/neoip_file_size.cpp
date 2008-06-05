/*! \file
    \brief Definition of the \ref file_size_t

- TODO handle the file_size_t::MAX as delay_t::INFINITE in the arithmetic operators
- TODO put the MAX_VAL/NONE_VAL as uint64_t constant in .hpp
- TODO inline the comparison too
  - apparently it appear surprisingly high when cpu profiling

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_file_size.hpp"
#include "neoip_string.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref file_aio_profile_t constant
const file_size_t	file_size_t::MAX	= file_size_t::MAX_VAL;
const file_size_t	file_size_t::NONE	= file_size_t::NONE_VAL;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   static ctor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Return a file_size_t from this string
 * 
 * - it handled all the string_t::to_uint64() cases (aka with units e.g. 23k)
 * - additionnally if str == "max" it return file_size_t::MAX
 * - additionnally if str == "none" it returns file_size_t::NONE
 */
file_size_t	file_size_t::from_str(const std::string &str)	throw()
{
	// handle the special cases for file_size_t::NONE and file_size_t::MAX
	if( !string_t::casecmp(str,"none") )	return file_size_t::NONE;
	if( !string_t::casecmp(str,"max") )	return file_size_t::MAX;
	// if not a special case, simply forward to string_t::to_uint64()
	return string_t::to_uint64(str);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string file_size_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )		return "null";
	// handle the max case
	if( *this == MAX )	return "MAX";
	// build the string
	oss << offset;
	// return the just built string
	return oss.str();
}


/** \brief convert the object into a string
 */
std::string file_size_t::to_human_string()			const throw()
{
	// handle the null case
	if( is_null() )		return "null";
	// handle the max case
	if( *this == MAX )	return "MAX";
	// build the string
	return string_t::size_string(offset);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a file_size_t
 * 
 * - support null file_size_t
 */
serial_t& operator << (serial_t& serial, const file_size_t &file_size)		throw()
{
	// serialize each field of the object
	serial << file_size.offset;
	// return serial
	return serial;
}

/** \brief unserialze a file_size_t
 * 
 * - support null file_size_t
 */
serial_t& operator >> (serial_t & serial, file_size_t &file_size)		throw(serial_except_t)
{	
	uint64_t	offset;
	// reset the destination variable
	file_size	= file_size_t();
	// unserialize the data
	serial >> offset;
	// set the returned variable
	file_size	= file_size_t(offset);
	// return serial
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc file_size_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for file_size_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const file_size_t &file_size)	throw()
{
	// sanity check - the file_size_t MUST be is_uint64_ok();
	DBG_ASSERT( file_size.is_uint64_ok() );
	// serialize the data
	xmlrpc_build << file_size.to_uint64();
	// return the object itself
	return xmlrpc_build;
}

/** \brief unserialize xmlrpc for file_size_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, file_size_t &file_size)	throw(xml_except_t)
{
	uint64_t	tmp;
	// unserialize the data
	xmlrpc_parse >> tmp;
	// set the unserialized value in the file_size_t
	file_size	= file_size_t(tmp);;
	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END;






