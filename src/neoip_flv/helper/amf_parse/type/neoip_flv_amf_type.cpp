/*! \file
    \brief Implementation of the \ref flv_amf_type_t
*/

/* system include */
/* local include */
#include "neoip_flv_amf_type.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(flv_amf_type_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, NUMBER	, "NUMBER"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, BOOLEAN	, "BOOLEAN"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, STRING	, "STRING"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, OBJECT	, "OBJECT"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, UNDEFINED	, "UNDEFINED"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, MIXEDARRAY	, "MIXEDARRAY"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, ENDOFOBJECT	, "ENDOFOBJECT"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, ARRAY		, "ARRAY"	)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_amf_type_t	, DATE		, "DATE"	)
NEOIP_STRTYPE_DEFINITION_END(flv_amf_type_t)

NEOIP_NAMESPACE_END




