/*! \file
    \brief Header of the extended type of xmlrpc parse/build
    
*/


#ifndef __NEOIP_XMLRPC_EXTTYPE_HPP__ 
#define __NEOIP_XMLRPC_EXTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_xmlrpc_parse.hpp"
#include "neoip_xmlrpc_build.hpp"
#include "neoip_datum.hpp"
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN


/*************** datum_t	***********************************************/
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const datum_t &datum)throw();
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, datum_t &datum)	throw(xml_except_t);
/*************** delay_t	***********************************************/
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const delay_t &delay)throw();
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, delay_t &delay)	throw(xml_except_t);
/*************** uint32_t	***********************************************/
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const uint32_t &value)throw();
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, uint32_t &value)	throw(xml_except_t);
/*************** uint64_t	***********************************************/
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const uint64_t &value)throw();
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, uint64_t &value)	throw(xml_except_t);
/*************** float	***********************************************/
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const float &value)	throw();
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, float &value)	throw(xml_except_t);
#ifdef __APPLE__	// just a workaround because i forgot many serialisation of size_t
/*************** size_t	***********************************************/
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const size_t &value)throw();
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, size_t &value)	throw(xml_except_t);
#endif

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_EXTTYPE_HPP__  */



