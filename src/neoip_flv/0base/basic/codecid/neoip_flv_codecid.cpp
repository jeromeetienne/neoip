/*! \file
    \brief Implementation of the \ref flv_codecid_t
*/

/* system include */
/* local include */
#include "neoip_flv_codecid.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(flv_codecid_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_codecid_t	, SORENSEN_H263	, "SORENSEN_H263")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_codecid_t	, SCREEN_VIDEO	, "SCREEN_VIDEO")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_codecid_t	, ON2_VP6	, "ON2_VP6")
NEOIP_STRTYPE_DEFINITION_END(flv_codecid_t)

NEOIP_NAMESPACE_END




