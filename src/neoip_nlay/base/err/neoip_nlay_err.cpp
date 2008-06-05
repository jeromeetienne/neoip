/*! \file
    \brief Definition of the \ref nlay_err_t

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_nlay_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(nlay_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , OK			, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , BOGUS_PKT		, "BOGUS_PKT" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , NEGO_FAILED		, "NEGO_FAILED" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , FROM_SCNX		, "FROM_SCNX" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , FROM_PKTCOMP		, "FROM_PKTCOMP" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , FROM_REACHPEER	, "FROM_REACHPEER" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , FROM_PKTFRAG		, "FROM_PKTFRAG" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , FROM_RDGRAM		, "FROM_RDGRAM" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , FROM_ORDGRAM		, "FROM_ORDGRAM" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nlay_err_t , FROM_SIMUWAN		, "FROM_SIMUWAN" )
NEOIP_ERRTYPE_DEFINITION_END(nlay_err_t)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
// - those function isnt done in nlay_err_t itself as it would require 2
//   implicit convertion and c++ does only one.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a rdgram_err_t into a nlay_err_t
 */
nlay_err_t nlay_err_from_idletimeout(const std::string &str)	throw()
{
	return nlay_err_t(nlay_err_t::FROM_IDLETIMEOUT, str );
}

/** \brief convert a rdgram_err_t into a nlay_err_t
 */
nlay_err_t nlay_err_from_ordgram(const rdgram_err_t &rdgram_err, const std::string &prefix_str )	throw()
{
	return nlay_err_t(nlay_err_t::FROM_ORDGRAM, prefix_str + rdgram_err.to_string() );
}

/** \brief convert a pktcomp_err_t into a nlay_err_t
 */
nlay_err_t nlay_err_from_pktcomp(const pktcomp_err_t &pktcomp_err, const std::string &prefix_str )	throw()
{
	return nlay_err_t(nlay_err_t::FROM_PKTCOMP, prefix_str + pktcomp_err.to_string() );
}

/** \brief convert a pktfrag_err_t into a nlay_err_t
 */
nlay_err_t nlay_err_from_pktfrag(const pktfrag_err_t &pktfrag_err, const std::string &prefix_str )	throw()
{
	return nlay_err_t(nlay_err_t::FROM_PKTFRAG, prefix_str + pktfrag_err.to_string() );
}

/** \brief convert a rdgram_err_t into a nlay_err_t
 */
nlay_err_t nlay_err_from_rdgram(const rdgram_err_t &rdgram_err, const std::string &prefix_str )	throw()
{
	return nlay_err_t(nlay_err_t::FROM_RDGRAM, prefix_str + rdgram_err.to_string() );
}

/** \brief convert a reachpeer_err_t into a nlay_err_t
 */
nlay_err_t nlay_err_from_reachpeer(const reachpeer_err_t &reachpeer_err, const std::string &prefix_str )	throw()
{
	return nlay_err_t(nlay_err_t::FROM_REACHPEER, prefix_str + reachpeer_err.to_string() );
}

/** \brief convert a scnx_err_t into a nlay_err_t
 */
nlay_err_t nlay_err_from_scnx(const scnx_err_t &scnx_err, const std::string &prefix_str )	throw()
{
	return nlay_err_t(nlay_err_t::FROM_SCNX, prefix_str + scnx_err.to_string() );
}

/** \brief convert a simuwan_err_t into a nlay_err_t
 */
nlay_err_t nlay_err_from_simuwan(const simuwan_err_t &simuwan_err, const std::string &prefix_str )	throw()
{
	return nlay_err_t(nlay_err_t::FROM_SIMUWAN, prefix_str + simuwan_err.to_string() );
}


NEOIP_NAMESPACE_END





