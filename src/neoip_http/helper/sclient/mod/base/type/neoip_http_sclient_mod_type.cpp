/*! \file
    \brief Implementation of the \ref http_sclient_mod_type_t
*/

/* system include */
/* local include */
#include "neoip_http_sclient_mod_type.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(http_sclient_mod_type_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(http_sclient_mod_type_t	, RAW	, "RAW")
NEOIP_STRTYPE_DEFINITION_ITEM(http_sclient_mod_type_t	, FLV	, "FLV")
NEOIP_STRTYPE_DEFINITION_END(http_sclient_mod_type_t)

NEOIP_NAMESPACE_END




