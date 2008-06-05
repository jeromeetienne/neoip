/*! \file
    \brief Implementation of the \ref bt_alloc_policy_t
*/

/* system include */
/* local include */
#include "neoip_bt_alloc_policy.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_alloc_policy_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_alloc_policy_t	, FULL	, "FULL")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_alloc_policy_t	, HOLE	, "HOLE")
NEOIP_STRTYPE_DEFINITION_END(bt_alloc_policy_t)

NEOIP_NAMESPACE_END




