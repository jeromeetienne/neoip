/*! \file
    \brief Definition of the \ref xmlrpc_err_t

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_err.hpp"
#include "neoip_namespace.hpp"
// include for error convertion
#include "neoip_http_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(xmlrpc_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(xmlrpc_err_t	, OK		, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(xmlrpc_err_t	, ERROR		, "ERROR" )
NEOIP_ERRTYPE_DEFINITION_ITEM(xmlrpc_err_t	, FROM_HTTP	, "FROM_HTTP" )
NEOIP_ERRTYPE_DEFINITION_END(xmlrpc_err_t)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a http_err_t into a xmlrpc_err_t
 */
xmlrpc_err_t xmlrpc_err_from_http(const http_err_t &http_err, const std::string &prefix_str )	throw()
{
	return xmlrpc_err_t(xmlrpc_err_t::FROM_HTTP, prefix_str + http_err.to_string());
}

NEOIP_NAMESPACE_END





