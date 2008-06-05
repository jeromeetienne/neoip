/*! \file
    \brief Header of the ntudp_tun packets type \ref ntudp_tunl_pkttype_t
    

*/


#ifndef __NEOIP_NTUDP_TUNL_PKTTYPE_HPP__ 
#define __NEOIP_NTUDP_TUNL_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_pkttype.hpp"
#include "neoip_pkt.hpp"
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	datum_t;
class	ntudp_peerid_t;
class	ntudp_nonce_t;

NEOIP_STRTYPE_DECLARATION_START(ntudp_tunl_pkttype_t	, NEOIP_NTUDP_PKTTYPE_TUNL_OFFSET)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_tunl_pkttype_t	, REGISTER_REQUEST)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_tunl_pkttype_t	, REGISTER_REPLY)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_tunl_pkttype_t	, PKT_SRV2CLI)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_tunl_pkttype_t	, PKT_CLI2SRV)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_tunl_pkttype_t	, PKT_EXT2SRV)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(ntudp_tunl_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(ntudp_tunl_pkttype_t	, ntudp_pkttype_t )

#if NEOIP_NTUDP_PKTTYPE_TUNL_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_ntudp_pkttype.hpp constants."
#endif


/*************** packet building	*******************************/
pkt_t ntudp_tunl_pkttype_build_pkt_ext2srv(const ntudp_nonce_t &nonce, const ntudp_peerid_t &dest_peerid
							, const datum_t &datum)	throw();

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_TUNL_PKTTYPE_HPP__  */



