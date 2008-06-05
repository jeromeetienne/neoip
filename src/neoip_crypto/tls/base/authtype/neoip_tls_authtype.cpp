/*! \file
    \brief Header of the \ref tls_authtype_t

*/

/* system include */
/* local include */
#include "neoip_tls_authtype.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(tls_authtype_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(tls_authtype_t	, ANON	, "ANON")
NEOIP_STRTYPE_DEFINITION_ITEM(tls_authtype_t	, CERT	, "CERT")
NEOIP_STRTYPE_DEFINITION_END(tls_authtype_t)

NEOIP_NAMESPACE_END

