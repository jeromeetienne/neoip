/*! \file
    \brief Header of the ntudp_tun packets type \ref ntudp_sock_errcode_t
    

*/


#ifndef __NEOIP_NTUDP_SOCK_ERRCODE_HPP__ 
#define __NEOIP_NTUDP_SOCK_ERRCODE_HPP__ 
/* system include */
/* local include */
#include "neoip_pkt.hpp"
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_nonce_t;

NEOIP_STRTYPE_DECLARATION_START(ntudp_sock_errcode_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_errcode_t	, UNREACH_PEERID)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_errcode_t	, UNREACH_PORTID)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_errcode_t	, ALREADY_BOUND)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_sock_errcode_t	, UNKNOWN_CNX)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(ntudp_sock_errcode_t)
NEOIP_STRTYPE_DECLARATION_END(ntudp_sock_errcode_t	, uint8_t )

pkt_t ntudp_sock_errcode_build_pkt(const ntudp_sock_errcode_t &sock_errcode
					, const ntudp_nonce_t &client_nonce)	throw();



NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_SOCK_ERRCODE_HPP__  */



