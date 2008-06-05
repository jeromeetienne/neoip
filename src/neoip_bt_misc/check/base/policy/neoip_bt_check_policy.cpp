/*! \file
    \brief Implementation of the \ref bt_check_policy_t

*/

/* system include */
/* local include */
#include "neoip_bt_check_policy.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_check_policy_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_check_policy_t	, SUBFILE_EXIST	, "SUBFILE_EXIST")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_check_policy_t	, PARTIAL_PIECE	, "PARTIAL_PIECE")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_check_policy_t	, EVERY_PIECE	, "EVERY_PIECE")
NEOIP_STRTYPE_DEFINITION_END(bt_check_policy_t)

NEOIP_NAMESPACE_END




