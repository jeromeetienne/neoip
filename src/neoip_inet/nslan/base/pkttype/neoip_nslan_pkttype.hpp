/*! \file
    \brief Header of the \ref nslan_pkttype_t
*/


#ifndef __NEOIP_NSLAN_PKTTYPE_HPP__ 
#define __NEOIP_NSLAN_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(nslan_pkttype_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(nslan_pkttype_t	, RECORD_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(nslan_pkttype_t	, RECORD_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(nslan_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(nslan_pkttype_t	, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NSLAN_PKTTYPE_HPP__  */



