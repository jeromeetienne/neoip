/*! \file
    \brief Implementation of the \ref bt_io_mode_t
*/

/* system include */
/* local include */
#include "neoip_bt_io_mode.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_io_mode_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_io_mode_t	, SFILE		, "SFILE")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_io_mode_t	, PFILE		, "PFILE")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_io_mode_t	, CACHE		, "CACHE")
NEOIP_STRTYPE_DEFINITION_END(bt_io_mode_t)

NEOIP_NAMESPACE_END




