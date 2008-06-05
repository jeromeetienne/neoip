/*! \file
    \brief Header of the \ref skey_auth_mode_t

*/

/* system include */
/* local include */
#include "neoip_skey_auth_mode.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(skey_auth_mode_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_mode_t	, HMAC	, "HMAC")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_mode_t	, NOKEY	, "NOKEY")
NEOIP_STRTYPE_DEFINITION_END(skey_auth_mode_t)

NEOIP_NAMESPACE_END

