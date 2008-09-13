/*! \file
    \brief Implementation of the xmlrpc serialization of 'extended type'
           (aka my own basic types)

- the type found here are
  - datum_t	
  - delay_t	(the dateTime.iso8601 of the xmlrpc standard seems to have compatibility issue)
  - uint32_t	(which surprisingly isnt a basic type in xmlrpc ...)
  - uint64_t	(which surprisingly isnt a basic type in xmlrpc ...)
  - float
   
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

/** \brief Put a delay_t into a xmlrpc double
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const delay_t &delay)	throw()
{
	// put a double representing the delay in second
	xmlrpc_build	<< delay.to_sec_double();
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a delay_t from a xmlrpc double
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, delay_t &delay)	throw(xml_except_t)
{
	double	sec_double;
	// get the value from the xmlrpc
	xmlrpc_parse	>> sec_double;
	// convert the value
	delay	= delay_t::from_msec(uint64_t(sec_double*1000.0));
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
	// put a string representing the value
	xmlrpc_build	<< double(value);
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a uint32_t from a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, uint32_t &value)	throw(xml_except_t)
{
	double	tmp;
	// get the value from the xmlrpc
	xmlrpc_parse	>> tmp;
	// convert the value
	value = uint32_t(tmp);
	// return the object itself
	return xmlrpc_parse;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           XMLRPC uint64_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a uint64_t into a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const uint64_t &value)	throw()
{
	// put a string representing the value
	xmlrpc_build	<< double(value);
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a uint64_t from a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, uint64_t &value)	throw(xml_except_t)
{
	double	tmp;
	// get the value from the xmlrpc
	xmlrpc_parse	>> tmp;
	// convert the value
	value = uint64_t(tmp);
	// return the object itself
	return xmlrpc_parse;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           XMLRPC float
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a float into a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const float &value)	throw()
{
	// put a string representing the value
	xmlrpc_build	<< double(value);
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a float from a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, float &value)	throw(xml_except_t)
{
	double	tmp;
	// get the value from the xmlrpc
	xmlrpc_parse	>> tmp;
	// convert the value
	value = float(tmp);
	// return the object itself
	return xmlrpc_parse;
}

#ifdef __APPLE__	// just a workaround because i forgot many serialisation of size_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           XMLRPC size_t
// - NOTE: this is only a kudge because i got many size_t serialization i forgot
//   - it triggered no error on linux/win32 but cause compilation error on macos
//   - so this is just a workaround... this is a temporary fix
//   - the trick is to handle it as a uint32_t serialisation
//   - same thing on xmlrpc_build_t/xmlrpc_parse_t and serial_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a uint32_t into a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const size_t &value)	throw()
{
	// put a string representing the value
	return xmlrpc_build	<< uint32_t(value);
}

/** \brief Get a uint32_t from a xmlrpc string (because xmlrpc doesnt support unsigned integer)
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, size_t &value)	throw(xml_except_t)
{
	uint32_t	tmp;
	// get the value from the xmlrpc
	xmlrpc_parse	>> tmp;
	// copy the uint32_t into value
	value	= tmp;
	// return the object itself
	return xmlrpc_parse;
}
#endif


NEOIP_NAMESPACE_END
