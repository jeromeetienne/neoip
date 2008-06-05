/*! \file
    \brief Definition of the http_version_t
*/


/* system include */
/* local include */
#include "neoip_http_version.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(http_version_t,int)
NEOIP_STRTYPE_DEFINITION_ITEM(http_version_t, V1_0	, "HTTP/1.0" )
NEOIP_STRTYPE_DEFINITION_ITEM(http_version_t, V1_1	, "HTTP/1.1" )
NEOIP_STRTYPE_DEFINITION_END(http_version_t)

NEOIP_NAMESPACE_END

