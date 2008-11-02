/*! \file
    \brief Header of the \ref dvar_type_t

*/

/* system include */
/* local include */
#include "neoip_dvar_type.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(dvar_type_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(dvar_type_t	, INTEGER	, "INTERGER")
NEOIP_STRTYPE_DEFINITION_ITEM(dvar_type_t	, DOUBLE	, "DOUBLE")
NEOIP_STRTYPE_DEFINITION_ITEM(dvar_type_t	, STRING	, "STRING")
NEOIP_STRTYPE_DEFINITION_ITEM(dvar_type_t	, BOOLEAN	, "BOOLEAN")
NEOIP_STRTYPE_DEFINITION_ITEM(dvar_type_t	, NIL		, "NIL")
NEOIP_STRTYPE_DEFINITION_ITEM(dvar_type_t	, ARRAY		, "ARRAY")
NEOIP_STRTYPE_DEFINITION_ITEM(dvar_type_t	, MAP		, "MAP")
NEOIP_STRTYPE_DEFINITION_END(dvar_type_t)

NEOIP_NAMESPACE_END

