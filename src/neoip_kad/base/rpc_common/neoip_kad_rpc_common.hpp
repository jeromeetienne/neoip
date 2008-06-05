/*! \file
    \brief Header of the kad_rpc_common_t class
    
*/


#ifndef __NEOIP_KAD_RPC_COMMON_HPP__ 
#define __NEOIP_KAD_RPC_COMMON_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_pkt.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_addr_t;
class	kad_nonceid_t;
class	kad_realmid_t;
class	ipport_addr_t;

/** \brief bunch of static function common to rpc handling
 */
class kad_rpc_common_t {
public:
	static kad_err_t 	parse_req_header(kad_peer_t *kad_peer, pkt_t &pkt, kad_pkttype_t &pkttype
						, kad_addr_t &source_addr
						, kad_nonceid_t &request_nonce)		throw();
	static pkt_t		build_req_header(kad_peer_t *kad_peer, const kad_addr_t &remote_addr
						, const kad_pkttype_t &kad_pkttype
						, const kad_nonceid_t &request_nonce)	throw();
	static kad_err_t 	parse_rep_header(kad_peer_t *kad_peer, pkt_t &pkt, kad_pkttype_t &pkttype
						, kad_addr_t &source_addr
						, kad_nonceid_t &request_nonce)		throw();
	static pkt_t		build_rep_header(kad_peer_t *kad_peer, const ipport_addr_t &remote_oaddr
						, const kad_pkttype_t &pkttype
						, const kad_nonceid_t &request_nonce)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPC_COMMON_HPP__  */



