/*! \file
    \brief Definition of the \ref socket_err_t.cpp

*/

/* system include */
/* local include */
#include "neoip_socket_err.hpp"
// include for error convertion
#include "neoip_nlay_err.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_crypto_err.hpp"
#include "neoip_slay_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(socket_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, BAD_PARAM	, "BAD_PARAM")
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, BAD_ADDR	, "BAD_ADDR")
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, FROM_NLAY	, "FROM_NLAY")
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, FROM_INET	, "FROM_INET")
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, FROM_NTUDP	, "FROM_NTUDP")
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, FROM_CRYPTO	, "FROM_CRYPTO")
NEOIP_ERRTYPE_DEFINITION_ITEM(socket_err_t	, FROM_SLAY	, "FROM_SLAY")
NEOIP_ERRTYPE_DEFINITION_END(socket_err_t)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a nlay_err_t into a socket_err_t
 */
socket_err_t socket_err_from_nlay(const nlay_err_t &nlay_err, const std::string &prefix_str )	throw()
{
	return socket_err_t(socket_err_t::FROM_NLAY, prefix_str + nlay_err.to_string() );
}

/** \brief convert a inet_err_t into a socket_err_t
 */
socket_err_t socket_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str )	throw()
{
	return socket_err_t(socket_err_t::FROM_INET, prefix_str + inet_err.to_string() );
}

/** \brief convert a ntudp_err_t into a socket_err_t
 */
socket_err_t socket_err_from_ntudp(const ntudp_err_t &ntudp_err, const std::string &prefix_str )	throw()
{
	return socket_err_t(socket_err_t::FROM_NTUDP, prefix_str + ntudp_err.to_string() );
}

/** \brief convert a crypto_err_t into a socket_err_t
 */
socket_err_t socket_err_from_crypto(const crypto_err_t &crypto_err, const std::string &prefix_str )	throw()
{
	return socket_err_t(socket_err_t::FROM_CRYPTO, prefix_str + crypto_err.to_string() );
}

/** \brief convert a slay_err_t into a socket_err_t
 */
socket_err_t socket_err_from_slay(const slay_err_t &slay_err, const std::string &prefix_str )	throw()
{
	return socket_err_t(socket_err_t::FROM_SLAY, prefix_str + slay_err.to_string() );
}

NEOIP_NAMESPACE_END




