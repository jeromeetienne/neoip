/*! \file
    \brief Implementation of the \ref router_acl_type_t

*/

/* system include */
/* local include */
#include "neoip_router_acl_type.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(router_acl_type_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(router_acl_type_t	, ACCEPT	, "ACCEPT")
NEOIP_STRTYPE_DEFINITION_ITEM(router_acl_type_t	, REJECT	, "REJECT")
NEOIP_STRTYPE_DEFINITION_END(router_acl_type_t)

NEOIP_NAMESPACE_END






