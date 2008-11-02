/*! \file
    \brief Implementation of the \ref rtmp_type_t

\par Brief description


- TODO get a full list from the intenet and put it there

*/

/* system include */
/* local include */
#include "neoip_rtmp_type.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(rtmp_type_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_type_t	, PING		, "PING")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_type_t	, CLIENTBW	, "CLIENTBW")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_type_t	, SERVERBW	, "SERVERBW")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_type_t	, AUDIO		, "AUDIO")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_type_t	, VIDEO		, "VIDEO")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_type_t	, META		, "META")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_type_t	, INVOKE	, "INVOKE")
NEOIP_STRTYPE_DEFINITION_END(rtmp_type_t)

NEOIP_NAMESPACE_END




