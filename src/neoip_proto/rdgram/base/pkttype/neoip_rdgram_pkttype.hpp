/*! \file
    \brief Header of the \ref rdgram_pkttype.hpp
*/


#ifndef __NEOIP_RDGRAM_PKTTYPE_HPP__ 
#define __NEOIP_RDGRAM_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_nlay_pkttype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_STRTYPE_DECLARATION_START(rdgram_pkttype_t	, NEOIP_PKTTYPE_OFFSET_RDGRAM)
NEOIP_STRTYPE_DECLARATION_ITEM(rdgram_pkttype_t		, DATA_PKT)
NEOIP_STRTYPE_DECLARATION_ITEM(rdgram_pkttype_t		, ACK_PKT)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(rdgram_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(rdgram_pkttype_t, nlay_pkttype_t)

#if NEOIP_PKTTYPE_RDGRAM_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_strtype.hpp constants."
#endif


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RDGRAM_PKTTYPE_HPP__  */



