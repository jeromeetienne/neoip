/*! \file
    \brief Header of the \ref nlay_closure_pkttype_t

*/


#ifndef __NEOIP_NLAY_CLOSURE_PKTTYPE_HPP__ 
#define __NEOIP_NLAY_CLOSURE_PKTTYPE_HPP__ 

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_nlay_pkttype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(nlay_closure_pkttype_t	, NEOIP_PKTTYPE_OFFSET_NLAY_CLOSURE)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_closure_pkttype_t	, CLOSE_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_closure_pkttype_t	, CLOSE_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_closure_pkttype_t	, CLOSE_REPLY_ACK)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_closure_pkttype_t	, CLOSE_NOW)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(nlay_closure_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(nlay_closure_pkttype_t, nlay_pkttype_t)

#if NEOIP_PKTTYPE_NLAY_CLOSURE_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_strtype.hpp constants."
#endif


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_CLOSURE_PKTTYPE_HPP__  */





