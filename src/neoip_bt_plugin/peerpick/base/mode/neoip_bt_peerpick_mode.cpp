/*! \file
    \brief Implementation of the \ref bt_peerpick_mode_t
*/

/* system include */
/* local include */
#include "neoip_bt_peerpick_mode.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_peerpick_mode_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerpick_mode_t	, PLAIN	, "PLAIN")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerpick_mode_t	, RELAY	, "RELAY")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerpick_mode_t	, CASTI	, "CASTI")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerpick_mode_t	, CASTO	, "CASTO")
NEOIP_STRTYPE_DEFINITION_END(bt_peerpick_mode_t)

NEOIP_NAMESPACE_END




