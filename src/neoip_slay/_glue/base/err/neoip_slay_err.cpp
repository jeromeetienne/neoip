/*! \file
    \brief Definition of the \ref slay_err_t.cpp

*/

/* system include */
/* local include */
#include "neoip_slay_err.hpp"
// include for error convertion
#include "neoip_crypto_err.hpp"
#include "neoip_bt_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(slay_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(slay_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(slay_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(slay_err_t	, FROM_CRYPTO	, "FROM_CRYPTO")
NEOIP_ERRTYPE_DEFINITION_ITEM(slay_err_t	, FROM_BT	, "FROM_BT")
NEOIP_ERRTYPE_DEFINITION_END(slay_err_t)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a crypto_err_t into a slay_err_t
 */
slay_err_t slay_err_from_crypto(const crypto_err_t &crypto_err, const std::string &prefix_str )	throw()
{
	return slay_err_t(slay_err_t::FROM_CRYPTO, prefix_str + crypto_err.to_string() );
}

/** \brief convert a bt_err_t into a slay_err_t
 */
slay_err_t slay_err_from_bt(const bt_err_t &bt_err, const std::string &prefix_str )	throw()
{
	return slay_err_t(slay_err_t::FROM_BT, prefix_str + bt_err.to_string() );
}

NEOIP_NAMESPACE_END




