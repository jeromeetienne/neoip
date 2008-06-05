/*! \file
    \brief Header of the \ref reachpeer_pkttype_t

*/

/* system include */
/* local include */
#include "neoip_reachpeer_pkttype.hpp"
#include "neoip_assert.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(reachpeer_pkttype_t	, nlay_pkttype_t)
NEOIP_STRTYPE_DEFINITION_ITEM(reachpeer_pkttype_t	, ECHO_REQUEST	, "ECHO_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(reachpeer_pkttype_t	, ECHO_REPLY	, "ECHO_REPLY")
NEOIP_STRTYPE_DEFINITION_END(reachpeer_pkttype_t)

NEOIP_NAMESPACE_END

