/*! \file
    \brief Implementation of the \ref rtmp_resp_cnx_state_t
*/

/* system include */
/* local include */
#include "neoip_rtmp_resp_cnx_state.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(rtmp_resp_cnx_state_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_resp_cnx_state_t	, WAITING_ITORSYN	, "WAITING_ITORSYN")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_resp_cnx_state_t	, WAITING_ITORACK	, "WAITING_ITORACK")
NEOIP_STRTYPE_DEFINITION_END(rtmp_resp_cnx_state_t)

NEOIP_NAMESPACE_END




