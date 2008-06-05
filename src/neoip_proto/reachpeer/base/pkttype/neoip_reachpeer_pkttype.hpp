/*! \file
    \brief Header of the \ref reachpeer_pkttype_t

*/


#ifndef __NEOIP_REACHPEER_PKTTYPE_HPP__ 
#define __NEOIP_REACHPEER_PKTTYPE_HPP__ 

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_nlay_pkttype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(reachpeer_pkttype_t	, NEOIP_PKTTYPE_OFFSET_REACHPEER)
NEOIP_STRTYPE_DECLARATION_ITEM(reachpeer_pkttype_t	, ECHO_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(reachpeer_pkttype_t	, ECHO_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(reachpeer_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(reachpeer_pkttype_t, nlay_pkttype_t)

#if NEOIP_PKTTYPE_REACHPEER_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_strtype.hpp constants."
#endif


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_REACHPEER_PKTTYPE_HPP__  */





