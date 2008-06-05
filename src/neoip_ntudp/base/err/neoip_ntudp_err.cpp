/*! \file
    \brief Definition of the \ref ntudp_err_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_namespace.hpp"

// list of include for the convertions
#include "neoip_kad_err.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_netif_err.hpp"
#include "neoip_ndiag_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(ntudp_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(ntudp_err_t	, OK			, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(ntudp_err_t	, ERROR			, "ERROR" )
NEOIP_ERRTYPE_DEFINITION_ITEM(ntudp_err_t	, ALREADY_BOUND		, "ALREADY_BOUND" )
NEOIP_ERRTYPE_DEFINITION_ITEM(ntudp_err_t	, FROM_KAD		, "FROM_KAD" )
NEOIP_ERRTYPE_DEFINITION_ITEM(ntudp_err_t	, FROM_INET		, "FROM_INET" )
NEOIP_ERRTYPE_DEFINITION_ITEM(ntudp_err_t	, FROM_NETIF		, "FROM_NETIF" )
NEOIP_ERRTYPE_DEFINITION_ITEM(ntudp_err_t	, FROM_NDIAG		, "FROM_NDIAG" )
NEOIP_ERRTYPE_DEFINITION_END(ntudp_err_t)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a kad_err_t into a ntudp_err_t
 * 
 * - this function isnt done in ntudp_err_t itself as it would require 2
 *   implicit convertion and c++ does only one.
 */
ntudp_err_t ntudp_err_from_kad(const kad_err_t &kad_err, const std::string &prefix_str )	throw()
{
	if( kad_err == kad_err_t::OK )	return ntudp_err_t::OK;
	return ntudp_err_t(ntudp_err_t::FROM_KAD, prefix_str + kad_err.to_string() );
}

/** \brief convert a inet_err_t into a ntudp_err_t
 * 
 * - this function isnt done in ntudp_err_t itself as it would require 2
 *   implicit convertion and c++ does only one.
 */
ntudp_err_t ntudp_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str )	throw()
{
	if( inet_err == inet_err_t::OK )	return ntudp_err_t::OK;
	return ntudp_err_t(ntudp_err_t::FROM_INET, prefix_str + inet_err.to_string() );
}

/** \brief convert a netif_err_t into a ntudp_err_t
 * 
 * - this function isnt done in ntudp_err_t itself as it would require 2
 *   implicit convertion and c++ does only one.
 */
ntudp_err_t ntudp_err_from_netif(const netif_err_t &netif_err, const std::string &prefix_str )	throw()
{
	if( netif_err == netif_err_t::OK )	return ntudp_err_t::OK;
	return ntudp_err_t(ntudp_err_t::FROM_NETIF, prefix_str + netif_err.to_string() );
}

/** \brief convert a ndiag_err_t into a ntudp_err_t
 * 
 * - this function isnt done in ntudp_err_t itself as it would require 2
 *   implicit convertion and c++ does only one.
 */
ntudp_err_t ntudp_err_from_ndiag(const ndiag_err_t &ndiag_err, const std::string &prefix_str )	throw()
{
	if( ndiag_err == ndiag_err_t::OK )	return ntudp_err_t::OK;
	return ntudp_err_t(ntudp_err_t::FROM_NDIAG, prefix_str + ndiag_err.to_string() );
}

NEOIP_NAMESPACE_END





