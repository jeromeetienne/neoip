/*! \file
    \brief Implementation of the \ref flv_soundrate_t
*/

/* system include */
/* local include */
#include "neoip_flv_soundrate.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(flv_soundrate_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundrate_t	, RATE_5_5KHZ	, "5_5KHZ")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundrate_t	, RATE_11_KHZ	, "11_KHZ")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundrate_t	, RATE_22_KHZ	, "22_KHZ")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundrate_t	, RATE_44_KHZ	, "44_KHZ")
NEOIP_STRTYPE_DEFINITION_END(flv_soundrate_t)

NEOIP_NAMESPACE_END




