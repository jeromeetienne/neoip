/*! \file
    \brief Header of the \ref pktcomp_pkttype_t
*/


#ifndef __NEOIP_PKTCOMP_PKTTYPE_HPP__ 
#define __NEOIP_PKTCOMP_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_nlay_pkttype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_STRTYPE_DECLARATION_START(pktcomp_pkttype_t	, NEOIP_PKTTYPE_OFFSET_PKTCOMP)
NEOIP_STRTYPE_DECLARATION_ITEM(pktcomp_pkttype_t	, COMPRESSED_PKT)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(pktcomp_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(pktcomp_pkttype_t, nlay_pkttype_t)

#if NEOIP_PKTTYPE_PKTCOMP_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_strtype.hpp constants."
#endif

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTCOMP_PKTTYPE_HPP__  */



