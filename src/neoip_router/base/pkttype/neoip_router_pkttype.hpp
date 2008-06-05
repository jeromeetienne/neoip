/*! \file
    \brief Header of the packet type of the router
    

*/


#ifndef __NEOIP_ROUTER_PKTTYPE_HPP__ 
#define __NEOIP_ROUTER_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(router_pkttype_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(router_pkttype_t	, ADDR_NEGO_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(router_pkttype_t	, ADDR_NEGO_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM(router_pkttype_t	, ADDR_NEGO_ACK)
NEOIP_STRTYPE_DECLARATION_ITEM(router_pkttype_t	, FWDED_PKT)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(router_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(router_pkttype_t	, uint8_t )

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_PKTTYPE_HPP__  */



