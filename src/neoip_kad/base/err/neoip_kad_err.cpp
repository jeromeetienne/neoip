/*! \file
    \brief Definition of the \ref kad_err_t

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_namespace.hpp"

// include for error conversion
#include "neoip_inet_err.hpp"
#include "neoip_file_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(kad_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(kad_err_t	, OK			, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(kad_err_t	, ERROR			, "ERROR" )
NEOIP_ERRTYPE_DEFINITION_ITEM(kad_err_t	, FROM_INET		, "FROM_INET" )
NEOIP_ERRTYPE_DEFINITION_ITEM(kad_err_t	, FROM_FILE		, "FROM_FILE" )
NEOIP_ERRTYPE_DEFINITION_END(kad_err_t)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a inet_err_t into a kad_err_t
 */
kad_err_t kad_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str )	throw()
{
	return kad_err_t(kad_err_t::FROM_INET, prefix_str + inet_err.to_string() );
}

/** \brief convert a file_err_t into a kad_err_t
 */
kad_err_t kad_err_from_file(const file_err_t &file_err, const std::string &prefix_str )	throw()
{
	return kad_err_t(kad_err_t::FROM_FILE, prefix_str + file_err.to_string() );
}

NEOIP_NAMESPACE_END





