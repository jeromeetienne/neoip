/*! \file
    \brief Implementation of the \ref bt_pselect_policy_t
*/

/* system include */
/* local include */
#include "neoip_bt_pselect_policy.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_pselect_policy_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_pselect_policy_t	, FIXED	, "FIXED")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_pselect_policy_t	, SLIDE	, "SLIDE")
NEOIP_STRTYPE_DEFINITION_END(bt_pselect_policy_t)

NEOIP_NAMESPACE_END




