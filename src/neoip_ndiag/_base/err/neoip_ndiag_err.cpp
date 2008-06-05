/*! \file
    \brief Definition of the \ref ndiag_err_t

*/

/* system include */
/* local include */
#include "neoip_ndiag_err.hpp"
#include "neoip_namespace.hpp"
// include for error convertion
#include "neoip_upnp_err.hpp"
#include "neoip_file_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(ndiag_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(ndiag_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(ndiag_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_ITEM(ndiag_err_t	, FROM_UPNP	, "FROM_UPNP")
NEOIP_ERRTYPE_DEFINITION_ITEM(ndiag_err_t	, FROM_FILE	, "FROM_FILE")
NEOIP_ERRTYPE_DEFINITION_END(ndiag_err_t)



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert a upnp_err_t into a ndiag_err_t
 */
ndiag_err_t ndiag_err_from_upnp(const upnp_err_t &upnp_err, const std::string &prefix_str )	throw()
{
	return ndiag_err_t(ndiag_err_t::FROM_UPNP, prefix_str + upnp_err.to_string());
}

/** \brief convert a file_err_t into a ndiag_err_t
 */
ndiag_err_t ndiag_err_from_file(const file_err_t &file_err, const std::string &prefix_str )	throw()
{
	return ndiag_err_t(ndiag_err_t::FROM_FILE, prefix_str + file_err.to_string());
}

NEOIP_NAMESPACE_END





