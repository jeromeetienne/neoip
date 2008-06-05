/*! \file
    \brief Implementation of the \ref bt_utmsgtype_t
*/

/* system include */
/* local include */
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_utmsgtype_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_utmsgtype_t	, HANDSHAKE	, "HANDSHAKE")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_utmsgtype_t	, UTPEX		, "UTPEX")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_utmsgtype_t	, BYTEACCT	, "BYTEACCT")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_utmsgtype_t	, PUNAVAIL	, "PUNAVAIL")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_utmsgtype_t	, PIECEWISH	, "PIECEWISH")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_utmsgtype_t	, BCAST		, "BCAST")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_utmsgtype_t	, FSTART	, "FSTART")
NEOIP_STRTYPE_DEFINITION_END(bt_utmsgtype_t)

NEOIP_NAMESPACE_END




