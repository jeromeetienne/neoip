/*! \file
    \brief Header of the packet type of the router
    

*/


#ifndef __NEOIP_ROUTER_ACL_TYPE_HPP__ 
#define __NEOIP_ROUTER_ACL_TYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(router_acl_type_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(router_acl_type_t	, ACCEPT)
NEOIP_STRTYPE_DECLARATION_ITEM(router_acl_type_t	, REJECT)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(router_acl_type_t)
NEOIP_STRTYPE_DECLARATION_END(router_acl_type_t	, uint8_t )

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ACL_TYPE_HPP__  */



