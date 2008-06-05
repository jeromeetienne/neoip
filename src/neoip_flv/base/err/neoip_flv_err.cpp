/*! \flv
    \brief Definition of the \ref flv_err_t

*/

/* system include */
/* local include */
#include "neoip_flv_err.hpp"
#include "neoip_namespace.hpp"
// include for error convertion
#include "neoip_socket_err.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DEFINITION_START(flv_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(flv_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(flv_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(flv_err_t	, FROM_SOCKET	, "FROM_SOCKET")
NEOIP_ERRTYPE_DEFINITION_END(flv_err_t)


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a socket_err_t into a flv_err_t
 */
flv_err_t flv_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str )	throw()
{
	return flv_err_t(flv_err_t::FROM_SOCKET, prefix_str + socket_err.to_string());
}

NEOIP_NAMESPACE_END





