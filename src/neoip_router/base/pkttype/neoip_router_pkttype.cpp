/*! \file
    \brief Implementation of the \ref router_pkttype_t

*/

/* system include */
/* local include */
#include "neoip_router_pkttype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(router_pkttype_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(router_pkttype_t	, ADDR_NEGO_REQUEST	, "ADDR_NEGO_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(router_pkttype_t	, ADDR_NEGO_REPLY	, "ADDR_NEGO_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(router_pkttype_t	, ADDR_NEGO_ACK		, "ADDR_NEGO_ACK")
NEOIP_STRTYPE_DEFINITION_ITEM(router_pkttype_t	, FWDED_PKT		, "FWDED_PKT")
NEOIP_STRTYPE_DEFINITION_END(router_pkttype_t)


NEOIP_NAMESPACE_END






