/*! \file
    \brief Definition of the \ref libsess_err_t

*/

/* system include */
/* local include */
#include "neoip_libsess_err.hpp"
#include "neoip_namespace.hpp"
// include for error convertion
#include "neoip_file_err.hpp"
#include "neoip_clineopt_err.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_http_err.hpp"

NEOIP_NAMESPACE_BEGIN


NEOIP_ERRTYPE_DEFINITION_START(libsess_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(libsess_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(libsess_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(libsess_err_t	, FROM_INET	, "FROM_INET")
NEOIP_ERRTYPE_DEFINITION_ITEM(libsess_err_t	, FROM_CLINEOPT	, "FROM_CLINEOPT")
NEOIP_ERRTYPE_DEFINITION_ITEM(libsess_err_t	, FROM_XMLRPC	, "FROM_XMLRPC")
NEOIP_ERRTYPE_DEFINITION_ITEM(libsess_err_t	, FROM_HTTP	, "FROM_HTTP")
NEOIP_ERRTYPE_DEFINITION_END(libsess_err_t)


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a file_err_t into a libsess_err_t
 */
libsess_err_t libsess_err_from_file(const file_err_t &file_err, const std::string &prefix_str )	throw()
{
	return libsess_err_t(libsess_err_t::FROM_INET, prefix_str + file_err.to_string());
}

/** \brief convert a clineopt_err_t into a libsess_err_t
 */
libsess_err_t libsess_err_from_clineopt(const clineopt_err_t &clineopt_err, const std::string &prefix_str )	throw()
{
	return libsess_err_t(libsess_err_t::FROM_CLINEOPT, prefix_str + clineopt_err.to_string());
}

/** \brief convert a xmlrpc_err_t into a libsess_err_t
 */
libsess_err_t libsess_err_from_xmlrpc(const xmlrpc_err_t &xmlrpc_err, const std::string &prefix_str )	throw()
{
	return libsess_err_t(libsess_err_t::FROM_XMLRPC, prefix_str + xmlrpc_err.to_string());
}

/** \brief convert a http_err_t into a libsess_err_t
 */
libsess_err_t libsess_err_from_http(const http_err_t &http_err, const std::string &prefix_str )	throw()
{
	return libsess_err_t(libsess_err_t::FROM_HTTP, prefix_str + http_err.to_string());
}

NEOIP_NAMESPACE_END





