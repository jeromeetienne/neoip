/*! \file
    \brief Definition of the \ref bt_err_t

*/

/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_namespace.hpp"
// include for error convertion
#include "neoip_http_err.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_socket_err.hpp"
#include "neoip_file_err.hpp"
#include "neoip_rate_err.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_ndiag_err.hpp"
#include "neoip_flv_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(bt_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_HTTP	, "FROM_HTTP")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_KAD	, "FROM_KAD")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_INET	, "FROM_INET")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_SOCKET	, "FROM_SOCKET")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_FILE	, "FROM_FILE")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_RATE	, "FROM_RATE")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_XMLRPC	, "FROM_XMLRPC")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_NDIAG	, "FROM_NDIAG")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_err_t	, FROM_FLV	, "FROM_FLV")
NEOIP_ERRTYPE_DEFINITION_END(bt_err_t)


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a http_err_t into a bt_err_t
 */
bt_err_t bt_err_from_http(const http_err_t &http_err, const std::string &prefix_str )	throw()
{
	return bt_err_t(bt_err_t::FROM_HTTP, prefix_str + http_err.to_string());
}
/** \brief convert a kad_err_t into a bt_err_t
 */
bt_err_t bt_err_from_kad(const kad_err_t &kad_err, const std::string &prefix_str )	throw()
{
	return bt_err_t(bt_err_t::FROM_KAD, prefix_str + kad_err.to_string());
}

/** \brief convert a inet_err_t into a bt_err_t
 */
bt_err_t bt_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str )	throw()
{
	return bt_err_t(bt_err_t::FROM_INET, prefix_str + inet_err.to_string());
}

/** \brief convert a socket_err_t into a bt_err_t
 */
bt_err_t bt_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str )	throw()
{
	return bt_err_t(bt_err_t::FROM_SOCKET, prefix_str + socket_err.to_string());
}

/** \brief convert a file_err_t into a bt_err_t
 */
bt_err_t bt_err_from_file(const file_err_t &file_err, const std::string &prefix_str )	throw()
{
	return bt_err_t(bt_err_t::FROM_FILE, prefix_str + file_err.to_string());
}

/** \brief convert a rate_err_t into a bt_err_t
 */
bt_err_t bt_err_from_rate(const rate_err_t &rate_err, const std::string &prefix_str )	throw()
{
	return bt_err_t(bt_err_t::FROM_RATE, prefix_str + rate_err.to_string());
}

/** \brief convert a xmlrpc_err_t into a bt_err_t
 */
bt_err_t bt_err_from_xmlrpc(const xmlrpc_err_t &xmlrpc_err, const std::string &prefix_str)	throw()
{
	return bt_err_t(bt_err_t::FROM_XMLRPC, prefix_str + xmlrpc_err.to_string());
}

/** \brief convert a ndiag_err_t into a bt_err_t
 */
bt_err_t bt_err_from_ndiag(const ndiag_err_t &ndiag_err, const std::string &prefix_str)	throw()
{
	return bt_err_t(bt_err_t::FROM_NDIAG, prefix_str + ndiag_err.to_string());
}

/** \brief convert a flv_err_t into a bt_err_t
 */
bt_err_t bt_err_from_flv(const flv_err_t &flv_err, const std::string &prefix_str)	throw()
{
	return bt_err_t(bt_err_t::FROM_FLV, prefix_str + flv_err.to_string());
}

NEOIP_NAMESPACE_END





