/*! \file
    \brief Definition of the \ref reachpeer_err_t

*/

/* system include */
/* local include */
#include "neoip_reachpeer_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DEFINITION_START(reachpeer_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(reachpeer_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(reachpeer_err_t	, BOGUS_PKT	, "BOGUS_PKT")
NEOIP_ERRTYPE_DEFINITION_ITEM(reachpeer_err_t	, BAD_PARAM	, "BAD_PARAM")
NEOIP_ERRTYPE_DEFINITION_END(reachpeer_err_t)

NEOIP_NAMESPACE_END





