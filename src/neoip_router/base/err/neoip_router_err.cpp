/*! \file
    \brief Definition of the \ref router_err_t

*/

/* system include */
/* local include */
#include "neoip_router_err.hpp"
#include "neoip_namespace.hpp"
// include for error convertion
#include "neoip_kad_err.hpp"
#include "neoip_netif_err.hpp"
#include "neoip_socket_err.hpp"
#include "neoip_dnsgrab_err.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_file_err.hpp"
#include "neoip_crypto_err.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DEFINITION_START(router_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, OK			, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, ERROR			, "ERROR" )
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, FROM_KAD		, "FROM_KAD" )
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, FROM_NETIF		, "FROM_NETIF" )
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, FROM_SOCKET		, "FROM_SOCKET" )
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, FROM_DNSGRAB		, "FROM_DNSGRAB" )
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, FROM_NTUDP		, "FROM_NTUDP" )
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, FROM_FILE		, "FROM_FILE" )
NEOIP_ERRTYPE_DEFINITION_ITEM(router_err_t	, FROM_CRYPTO		, "FROM_CRYPTO" )
NEOIP_ERRTYPE_DEFINITION_END(router_err_t)


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a kad_err_t into a router_err_t
 */
router_err_t router_err_from_kad(const kad_err_t &kad_err, const std::string &prefix_str )	throw()
{
	return router_err_t(router_err_t::FROM_KAD, prefix_str + kad_err.to_string() );
}

/** \brief convert a netif_err_t into a router_err_t
 */
router_err_t router_err_from_netif(const netif_err_t &netif_err, const std::string &prefix_str )	throw()
{
	return router_err_t(router_err_t::FROM_NETIF, prefix_str + netif_err.to_string() );
}

/** \brief convert a socket_err_t into a router_err_t
 */
router_err_t router_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str )	throw()
{
	return router_err_t(router_err_t::FROM_SOCKET, prefix_str + socket_err.to_string() );
}

/** \brief convert a dnsgrab_err_t into a router_err_t
 */
router_err_t router_err_from_dnsgrab(const dnsgrab_err_t &dnsgrab_err, const std::string &prefix_str )	throw()
{
	return router_err_t(router_err_t::FROM_DNSGRAB, prefix_str + dnsgrab_err.to_string() );
}

/** \brief convert a ntudp_err_t into a router_err_t
 */
router_err_t router_err_from_ntudp(const ntudp_err_t &ntudp_err, const std::string &prefix_str)	throw()
{
	return router_err_t(router_err_t::FROM_NTUDP, prefix_str + ntudp_err.to_string() );
}

/** \brief convert a file_err_t into a router_err_t
 */
router_err_t router_err_from_file(const file_err_t &file_err, const std::string &prefix_str )	throw()
{
	return router_err_t(router_err_t::FROM_FILE, prefix_str + file_err.to_string() );
}

/** \brief convert a crypto_err_t into a router_err_t
 */
router_err_t router_err_from_crypto(const crypto_err_t &crypto_err, const std::string &prefix_str)throw()
{
	return router_err_t(router_err_t::FROM_CRYPTO, prefix_str + crypto_err.to_string() );
}

NEOIP_NAMESPACE_END





