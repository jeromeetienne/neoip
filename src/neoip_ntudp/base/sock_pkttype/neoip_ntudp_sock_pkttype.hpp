/*! \file
    \brief Header of the ntudp_tun packets type \ref ntudp_sock_pkttype_t
    

*/


#ifndef __NEOIP_NTUDP_SOCK_PKTTYPE_HPP__ 
#define __NEOIP_NTUDP_SOCK_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_pkttype.hpp"
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_nonce_t;

NEOIP_STRTYPE_DECLARATION_START(ntudp_sock_pkttype_t	, NEOIP_NTUDP_PKTTYPE_SOCK_OFFSET)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, DIRECT_CNX_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, DIRECT_CNX_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, ESTARELAY_CNX_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, ESTARELAY_CNX_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, ESTARELAY_CNX_I2R_ACK)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, ESTARELAY_CNX_R2I_ACK)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, REVERSE_CNX_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, REVERSE_CNX_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, REVERSE_CNX_I2R_ACK)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, REVERSE_CNX_R2I_ACK)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_pkttype_t	, ERROR_PKT)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(ntudp_sock_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(ntudp_sock_pkttype_t	, ntudp_pkttype_t )

#if NEOIP_NTUDP_PKTTYPE_SOCK_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_ntudp_pkttype.hpp constants."
#endif

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_SOCK_PKTTYPE_HPP__  */



