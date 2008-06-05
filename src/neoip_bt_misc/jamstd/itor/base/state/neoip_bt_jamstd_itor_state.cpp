/*! \file
    \brief Implementation of the \ref bt_jamstd_itor_state_t

*/

/* system include */
/* local include */
#include "neoip_bt_jamstd_itor_state.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_jamstd_itor_state_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_jamstd_itor_state_t, CNXESTA_WAIT	, "CNXESTA_WAIT")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_jamstd_itor_state_t, CNXAUTH_WAIT	, "CNXAUTH_WAIT")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_jamstd_itor_state_t, PADAUTH_WAIT	, "PADAUTH_WAIT")
NEOIP_STRTYPE_DEFINITION_END(bt_jamstd_itor_state_t)

NEOIP_NAMESPACE_END




