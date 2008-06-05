/*! \file
    \brief Header of the \ref nlay_closure_pkttype_t

*/

/* system include */
/* local include */
#include "neoip_nlay_closure_pkttype.hpp"
#include "neoip_assert.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(nlay_closure_pkttype_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(nlay_closure_pkttype_t	, CLOSE_REQUEST		, "CLOSE_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(nlay_closure_pkttype_t	, CLOSE_REPLY		, "CLOSE_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(nlay_closure_pkttype_t	, CLOSE_REPLY_ACK	, "CLOSE_REPLY_ACK")
NEOIP_STRTYPE_DEFINITION_ITEM(nlay_closure_pkttype_t	, CLOSE_NOW		, "CLOSE_NOW")
NEOIP_STRTYPE_DEFINITION_END(nlay_closure_pkttype_t)

NEOIP_NAMESPACE_END

