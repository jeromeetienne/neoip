/*! \file
    \brief Implementation of the \ref flv_parse_state_t
*/

/* system include */
/* local include */
#include "neoip_flv_parse_state.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(flv_parse_state_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_parse_state_t	, TOPHD		, "TOPHD")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_parse_state_t	, TAG		, "TAG")
NEOIP_STRTYPE_DEFINITION_END(flv_parse_state_t)

NEOIP_NAMESPACE_END




