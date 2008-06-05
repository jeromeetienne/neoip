/*! \file
    \brief Implementation of the \ref rdgram_pkttype_t
*/

/* system include */
/* local include */
#include "neoip_rdgram_pkttype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(rdgram_pkttype_t	, nlay_pkttype_t)
NEOIP_STRTYPE_DEFINITION_ITEM(rdgram_pkttype_t	, DATA_PKT	, "DATA_PKT")
NEOIP_STRTYPE_DEFINITION_ITEM(rdgram_pkttype_t	, ACK_PKT	, "ACK_PKT")
NEOIP_STRTYPE_DEFINITION_END(rdgram_pkttype_t)

NEOIP_NAMESPACE_END




