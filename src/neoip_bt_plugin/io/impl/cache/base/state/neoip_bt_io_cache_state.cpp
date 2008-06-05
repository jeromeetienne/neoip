/*! \file
    \brief Implementation of the \ref bt_io_cache_state_t
*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_state.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_io_cache_state_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_io_cache_state_t	, CLEAN		, "CLEAN")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_io_cache_state_t	, DIRTY		, "DIRTY")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_io_cache_state_t	, CLEANING	, "CLEANING")
NEOIP_STRTYPE_DEFINITION_END(bt_io_cache_state_t)

NEOIP_NAMESPACE_END




