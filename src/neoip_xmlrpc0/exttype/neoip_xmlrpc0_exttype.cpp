/*! \file
    \brief Implementation of the xmlrpc serialization of 'extended type'
           (aka my own basic types)

- the type found here are
  - datum_t
  - delay_t	(the dateTime.iso8601 of the xmlrpc standard seems to have compatibility issue)
  - uint32_t	(which surprisingly isnt a basic type in xmlrpc ...)
   
*/

/* system include */
#include <iostream>
#include <iomanip>

/* local include */
#include "neoip_xmlrpc_exttype.hpp"
#include "neoip_base64.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           XMLRPC datum_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a datum_t into a xmlrpc
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const datum_t &datum)	throw()
{
	std::string	str	= base64_t::encode(datum.get_data(), datum.get_len());
	// put a string representing the datum_t
	xmlrpc_build << str;
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a datum_t from a xmlrpc
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, datum_t &datum)	throw(xml_except_t)
{
	std::string	str;
	// get value from the xmlrpc
	xmlrpc_parse >> str;
	// convert the value
	datum	= base64_t::decode(str);
	// return the object itself
	return xmlrpc_parse;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           XMLRPC delay_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a delay_t into a xmlrpc string
 * 
 * - TODO: why not use the date/time of xmlrpc ?
 *   - i think there is compatibility issue
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const delay_t &delay)	throw()
{
	std::string	str	= OSTREAMSTR(delay.to_sec_32bit());
	// put a string representing the salut
	xmlrpc_build	<< str;
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a delay_t from a xmlrpc string
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, delay_t &delay)	throw(xml_except_t)
{
	std::string	str;
	// get the value from the xmlrpc
	xmlrpc_parse	>> str;
	// convert the value
	delay	= delay_t::from_sec(atoi(str.c_str()));
	// return the object itself
	return xmlrpc_parse;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           XMLRPC uint32_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a uint32_t into a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const uint32_t &value)	throw()
{
	std::string	str	= OSTREAMSTR(value);
	// put a string representing the value
	xmlrpc_build	<< str;
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a uint32_t from a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, uint32_t &value)	throw(xml_except_t)
{
	std::string	str;
	// get the value from the xmlrpc
	xmlrpc_parse	>> str;
	// convert the value
	value = atoi(str.c_str());
	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END
