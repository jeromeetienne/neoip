/*! \file
    \brief Header of the \ref kad_peer_t
    
*/


#ifndef __NEOIP_KAD_PEER_RPC_SERVER_HPP__ 
#define __NEOIP_KAD_PEER_RPC_SERVER_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_peer.hpp"
#include "neoip_pkt.hpp"
#include "neoip_cookie.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for kad_peer_t::rpc_server_t
 */
class kad_peer_t::rpc_server_t : NEOIP_COPY_CTOR_DENY {
private:
	kad_peer_t *	kad_peer;	//!< backpointer to the kad_peer_t
	cookie_db_t	cookie_db;	//!< to handle the cookie of the received RPC

	/** \brief Context of a given request
	 */
	struct request_t {
		kad_pkttype_t	pkttype;	//!< the request kad_pkttype_t
		pkt_t		pkt;		//!< the packet containing the request
		kad_nonceid_t	nonce;		//!< the kad_nonceid_t from the request
		ipport_addr_t	local_oaddr;	//!< the local outter address from the connection which received this packet
		ipport_addr_t	remote_oaddr;	//!< the remote outter address from the connection which received this packet
		kad_addr_t::filter_fct_t kad_addr_filter;	//!< the filter for the kad_addr
		//! Constructor
		request_t(const kad_pkttype_t &pkttype, const pkt_t &pkt, const kad_nonceid_t &nonce
				, const ipport_addr_t &local_oaddr, const ipport_addr_t &remote_oaddr
				, kad_addr_t::filter_fct_t kad_addr_filter)		throw()
				: pkttype(pkttype), pkt(pkt), nonce(nonce), local_oaddr(local_oaddr)
				, remote_oaddr(remote_oaddr), kad_addr_filter(kad_addr_filter) {}
	};
	
	
	datum_t		build_pathid(const request_t &request) 			const throw();
	pkt_t		build_rep_header(const kad_pkttype_t &pkttype
						, const request_t &request)	throw();
	
	/*************** Handler for each request type	***********************/
	pkt_t		handle_ping_request(const request_t &request)		throw();
	pkt_t		handle_store_request(const request_t &request)		throw();
	pkt_t		handle_findnode_request(const request_t &request)	throw();
	pkt_t		handle_findsomeval_request(const request_t &request)	throw();
	pkt_t		handle_findallval_request(const request_t &request)	throw();
	pkt_t		handle_delete_request(const request_t &request)		throw();
public:
	/*************** ctor/dtor	***************************************/
	rpc_server_t(kad_peer_t *kad_peer)	throw();
	~rpc_server_t()				throw();

	// to handle the remote request from the kad_listener_t
	pkt_t	handle_request_pkt(pkt_t &pkt, const ipport_addr_t &local_oaddr
					, const ipport_addr_t &remote_oaddr)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_PEER_RPC_SERVER_HPP__  */










