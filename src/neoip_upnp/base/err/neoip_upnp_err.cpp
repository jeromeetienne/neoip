/*! \file
    \brief Definition of the \ref upnp_err_t

*/

/* system include */
/* local include */
#include "neoip_upnp_err.hpp"
#include "neoip_namespace.hpp"
// include for error convertion
#include "neoip_http_err.hpp"
#include "neoip_inet_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(upnp_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(upnp_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(upnp_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(upnp_err_t	, SOAP_FAULT	, "SOAP_FAULT")
NEOIP_ERRTYPE_DEFINITION_ITEM(upnp_err_t	, FROM_HTTP	, "FROM_HTTP")
NEOIP_ERRTYPE_DEFINITION_ITEM(upnp_err_t	, FROM_INET	, "FROM_INET")
NEOIP_ERRTYPE_DEFINITION_END(upnp_err_t)


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a http_err_t into a upnp_err_t
 */
upnp_err_t upnp_err_from_http(const http_err_t &http_err, const std::string &prefix_str )	throw()
{
	return upnp_err_t(upnp_err_t::FROM_HTTP, prefix_str + http_err.to_string());
}

/** \brief convert a inet_err_t into a upnp_err_t
 */
upnp_err_t upnp_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str )	throw()
{
	return upnp_err_t(upnp_err_t::FROM_INET, prefix_str + inet_err.to_string());
}


NEOIP_NAMESPACE_END





