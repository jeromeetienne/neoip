/*! \file
    \brief Header of the \ref compress_type_t

*/

/* system include */
/* local include */
#include "neoip_compress_type.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(compress_type_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(compress_type_t	, NULL_COMP	, "NULL_COMP")
NEOIP_STRTYPE_DEFINITION_ITEM(compress_type_t	, DEFLATE	, "DEFLATE")
NEOIP_STRTYPE_DEFINITION_ITEM(compress_type_t	, GZIP		, "GZIP")
NEOIP_STRTYPE_DEFINITION_ITEM(compress_type_t	, ZLIB		, "ZLIB")
NEOIP_STRTYPE_DEFINITION_ITEM(compress_type_t	, BZIP		, "BZIP")
NEOIP_STRTYPE_DEFINITION_END(compress_type_t)

NEOIP_NAMESPACE_END

