/*! \file
    \brief Definition of the \ref crypto_err_t

*/

/* system include */
#include <gnutls/gnutls.h>
/* local include */
#include "neoip_crypto_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DEFINITION_START(crypto_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(crypto_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(crypto_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(crypto_err_t	, FROM_GNUTLS	, "FROM_GNUTLS")
NEOIP_ERRTYPE_DEFINITION_END(crypto_err_t)


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a inet_err_t into a crypto_err_t
 */
crypto_err_t crypto_err_from_gnutls(int gnutls_err, const std::string &prefix_str)	throw()
{
	std::string	gnutls_err_str	= gnutls_strerror(gnutls_err);
	return crypto_err_t(crypto_err_t::FROM_GNUTLS, prefix_str + gnutls_err_str);
}

NEOIP_NAMESPACE_END





