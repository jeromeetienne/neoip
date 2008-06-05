/*! \file
    \brief Header of the \ref skey_ciph_mode_t

*/

/* system include */
/* local include */
#include "neoip_skey_ciph_mode.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(skey_ciph_mode_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_mode_t	, ECB	, "ECB")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_mode_t	, CTR	, "CTR")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_mode_t	, CBC	, "CBC")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_mode_t	, STREAM, "STREAM")
NEOIP_STRTYPE_DEFINITION_END(skey_ciph_mode_t)

NEOIP_NAMESPACE_END

