/*! \file
    \brief Implementation of the \ref ntudp_npos_pkttype_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_pkttype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(ntudp_npos_pkttype_t	, ntudp_pkttype_t)
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_npos_pkttype_t	, SADDRECHO_REQUEST	, "SADDRECHO_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_npos_pkttype_t	, SADDRECHO_REPLY	, "SADDRECHO_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_npos_pkttype_t	, INETREACH_REQUEST	, "INETREACH_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_npos_pkttype_t	, INETREACH_REPLY	, "INETREACH_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_npos_pkttype_t	, INETREACH_PROBE	, "INETREACH_PROBE")
NEOIP_STRTYPE_DEFINITION_END(ntudp_npos_pkttype_t)

NEOIP_NAMESPACE_END






