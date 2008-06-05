/*! \file
    \brief Definition of the \ref rdgram_err_t

*/

/* system include */
/* local include */
#include "neoip_rdgram_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(rdgram_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(rdgram_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(rdgram_err_t	, BOGUS_PKT	, "BOGUS_PKT")
NEOIP_ERRTYPE_DEFINITION_ITEM(rdgram_err_t	, SENDBUF_FULL	, "SENDBUF_FULL")
NEOIP_ERRTYPE_DEFINITION_END(rdgram_err_t)

NEOIP_NAMESPACE_END





