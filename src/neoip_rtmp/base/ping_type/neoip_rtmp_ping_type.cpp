/*! \file
    \brief Implementation of the \ref rtmp_ping_type_t

\par Brief description


- TODO get a full list from the intenet and put it there

*/

/* system include */
/* local include */
#include "neoip_rtmp_ping_type.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(rtmp_ping_type_t	, uint16_t)
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, CLEAR		, "CLEAR")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, PLAYBUFFERCLR	, "PLAYBUFFERCLR")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, UNKNOWN2	, "UNKNOWN2")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, CLIENTBUFFER	, "CLIENTBUFFER")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, STREAMRESET	, "STREAMRESET")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, UNKNOWN5	, "UNKNOWN5")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, PINGCLIENT	, "PINGCLIENT")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, PONGSERVER	, "PONGSERVER")
NEOIP_STRTYPE_DEFINITION_ITEM(rtmp_ping_type_t	, UNKNOWN8	, "UNKNOWN8")
NEOIP_STRTYPE_DEFINITION_END(rtmp_ping_type_t)

NEOIP_NAMESPACE_END




