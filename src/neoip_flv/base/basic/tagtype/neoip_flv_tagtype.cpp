/*! \file
    \brief Implementation of the \ref flv_tagtype_t
*/

/* system include */
/* local include */
#include "neoip_flv_tagtype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(flv_tagtype_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_tagtype_t	, AUDIO	, "AUDIO")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_tagtype_t	, VIDEO	, "VIDEO")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_tagtype_t	, META	, "META")
NEOIP_STRTYPE_DEFINITION_END(flv_tagtype_t)

NEOIP_NAMESPACE_END




