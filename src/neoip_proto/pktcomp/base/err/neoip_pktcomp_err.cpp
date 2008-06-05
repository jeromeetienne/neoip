/*! \file
    \brief Definition of the \ref pktcomp_err_t

*/

/* system include */
/* local include */
#include "neoip_pktcomp_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(pktcomp_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(pktcomp_err_t	, OK			, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(pktcomp_err_t	, WOULD_EXPAND		, "WOULD_EXPAND")
NEOIP_ERRTYPE_DEFINITION_ITEM(pktcomp_err_t	, CANT_DECOMPRESS	, "CANT_DECOMPRESS")
NEOIP_ERRTYPE_DEFINITION_ITEM(pktcomp_err_t	, BOGUS_PKT		, "BOGUS_PKT")
NEOIP_ERRTYPE_DEFINITION_END(pktcomp_err_t)

NEOIP_NAMESPACE_END





