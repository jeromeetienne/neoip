/*! \file
    \brief Implementation of the \ref flv_frametype_t
*/

/* system include */
/* local include */
#include "neoip_flv_frametype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(flv_frametype_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_frametype_t	, KEY			, "KEY")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_frametype_t	, INTER			, "INTER")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_frametype_t	, INTER_DISPOSABLE	, "INTER_DISPOSABLE")
NEOIP_STRTYPE_DEFINITION_END(flv_frametype_t)

NEOIP_NAMESPACE_END




