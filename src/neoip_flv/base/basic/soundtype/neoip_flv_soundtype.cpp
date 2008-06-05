/*! \file
    \brief Implementation of the \ref flv_soundtype_t
*/

/* system include */
/* local include */
#include "neoip_flv_soundtype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(flv_soundtype_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundtype_t	, MONO	, "MONO"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundtype_t	, STEREO, "STEREO"	)
NEOIP_STRTYPE_DEFINITION_END(flv_soundtype_t)

NEOIP_NAMESPACE_END




