/*! \file
    \brief Implementation of the \ref nslan_pkttype_t
*/

/* system include */
/* local include */
#include "neoip_nslan_pkttype.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(nslan_pkttype_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(nslan_pkttype_t	, RECORD_REQUEST	, "RECORD_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(nslan_pkttype_t	, RECORD_REPLY		, "RECORD_REPLY")
NEOIP_STRTYPE_DEFINITION_END(nslan_pkttype_t)

NEOIP_NAMESPACE_END




