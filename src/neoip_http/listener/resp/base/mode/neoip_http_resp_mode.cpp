/*! \file
    \brief Definition of the http_resp_mode_t
*/


/* system include */
/* local include */
#include "neoip_http_resp_mode.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(http_resp_mode_t,int)
NEOIP_STRTYPE_DEFINITION_ITEM(http_resp_mode_t, ACCEPT_SUBPATH	, "ACCEPT_SUBPATH" )
NEOIP_STRTYPE_DEFINITION_ITEM(http_resp_mode_t, REJECT_SUBPATH	, "REJECT_SUBPATH" )
NEOIP_STRTYPE_DEFINITION_END(http_resp_mode_t)

NEOIP_NAMESPACE_END

