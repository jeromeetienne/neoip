/*! \file
    \brief Implementation of the \ref oload_mod_type_t
*/

/* system include */
/* local include */
#include "neoip_oload_mod_type.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(oload_mod_type_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(oload_mod_type_t	, RAW	, "RAW")
NEOIP_STRTYPE_DEFINITION_ITEM(oload_mod_type_t	, FLV	, "FLV")
NEOIP_STRTYPE_DEFINITION_END(oload_mod_type_t)

NEOIP_NAMESPACE_END




