/*! \file
    \brief Header of the ns_packet
*/


#ifndef __NEOIP_PKTFRAG_PKTTYPE_HPP__ 
#define __NEOIP_PKTFRAG_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_nlay_pkttype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_STRTYPE_DECLARATION_START(pktfrag_pkttype_t	, NEOIP_PKTTYPE_OFFSET_PKTFRAG)
NEOIP_STRTYPE_DECLARATION_ITEM(pktfrag_pkttype_t	, PKT_FRAGMENT)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(pktfrag_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(pktfrag_pkttype_t, nlay_pkttype_t)

#if NEOIP_PKTTYPE_PKTFRAG_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_strtype.hpp constants."
#endif


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTFRAG_PKTTYPE_HPP__  */



