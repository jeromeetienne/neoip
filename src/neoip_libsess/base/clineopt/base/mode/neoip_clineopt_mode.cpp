/*! \file
    \brief Implementation of the \ref clineopt_mode_t
*/

/* system include */
/* local include */
#include "neoip_clineopt_mode.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(clineopt_mode_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(clineopt_mode_t	, NOTHING	, "NOTHING")
NEOIP_STRTYPE_DEFINITION_ITEM(clineopt_mode_t	, OPTIONAL	, "OPTIONAL")
NEOIP_STRTYPE_DEFINITION_ITEM(clineopt_mode_t	, REQUIRED	, "REQUIRED")
NEOIP_STRTYPE_DEFINITION_END(clineopt_mode_t)

NEOIP_NAMESPACE_END




