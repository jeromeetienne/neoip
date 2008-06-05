/*! \file
    \brief Definition of the http_method_t
*/


/* system include */
/* local include */
#include "neoip_http_method.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(http_method_t,int)
NEOIP_STRTYPE_DEFINITION_ITEM(http_method_t, GET	, "GET" )
NEOIP_STRTYPE_DEFINITION_ITEM(http_method_t, POST	, "POST" )
NEOIP_STRTYPE_DEFINITION_ITEM(http_method_t, HEAD	, "HEAD" )
NEOIP_STRTYPE_DEFINITION_END(http_method_t)

NEOIP_NAMESPACE_END

