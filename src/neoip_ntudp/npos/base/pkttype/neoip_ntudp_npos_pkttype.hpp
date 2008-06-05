/*! \file
    \brief Header of the ntudp_npos packets type \ref ntudp_npos_pkttype_t
    

*/


#ifndef __NEOIP_NTUDP_NPOS_PKTTYPE_HPP__ 
#define __NEOIP_NTUDP_NPOS_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_pkttype.hpp"
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(ntudp_npos_pkttype_t	, NEOIP_NTUDP_PKTTYPE_NTUDP_NPOS_OFFSET)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_npos_pkttype_t	, SADDRECHO_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_npos_pkttype_t	, SADDRECHO_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_npos_pkttype_t	, INETREACH_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_npos_pkttype_t	, INETREACH_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_npos_pkttype_t	, INETREACH_PROBE)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(ntudp_npos_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(ntudp_npos_pkttype_t	, ntudp_pkttype_t )

#if NEOIP_NTUDP_PKTTYPE_NTUDP_NPOS_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_ntudp_pkttype.hpp constants."
#endif


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_PKTTYPE_HPP__  */



