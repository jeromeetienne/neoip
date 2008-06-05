/*! \file
    \brief Definition of the \ref http_err_t

*/

/* system include */
/* local include */
#include "neoip_http_err.hpp"
#include "neoip_namespace.hpp"
// include for error convertion
#include "neoip_inet_err.hpp"
#include "neoip_socket_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(http_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(http_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(http_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(http_err_t	, FROM_INET	, "FROM_INET")
NEOIP_ERRTYPE_DEFINITION_ITEM(http_err_t	, FROM_SOCKET	, "FROM_SOCKET")
NEOIP_ERRTYPE_DEFINITION_END(http_err_t)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a inet_err_t into a http_err_t
 */
http_err_t http_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str )	throw()
{
	return http_err_t(http_err_t::FROM_INET, prefix_str + inet_err.to_string());
}

/** \brief convert a socket_err_t into a http_err_t
 */
http_err_t http_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str )	throw()
{
	return http_err_t(http_err_t::FROM_SOCKET, prefix_str + socket_err.to_string());
}


NEOIP_NAMESPACE_END





