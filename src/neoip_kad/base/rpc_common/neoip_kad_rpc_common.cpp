/*! \file
    \brief Definition of the \ref kad_rpc_common_t class

*/

/* system include */
/* local include */
#include "neoip_kad_rpc_common.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Parse REQUEST header
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse a request pkt header
 */
kad_err_t kad_rpc_common_t::parse_req_header(kad_peer_t *kad_peer, pkt_t &pkt, kad_pkttype_t &kad_pkttype
						, kad_addr_t &source_addr
						, kad_nonceid_t &request_nonce)	throw()
{
	const kad_rpc_profile_t &	profile	= kad_peer->get_profile().rpc();
	kad_realmid_t			realmid;
	kad_peerid_t			dest_peerid;
	// log to debug
	KLOG_DBG("enter pkt=" << &pkt );
	// set the pkttype_profile_t in the kad_pkttype_t
	kad_pkttype	= kad_pkttype_t(kad_peer->get_profile().pkttype());	
	try {
		// read the packet type
		pkt >> kad_pkttype;
		// read the realm id - if realmid_in_pkt_ok()
		if( profile.realmid_in_pkt_ok() )		pkt >> realmid;
		// get the destination peerid from the incoming packet - if dest_peerid_in_pkt_ok()
		if( profile.dest_peerid_in_pkt_ok() )		pkt >> dest_peerid;
		// read the source_addr
		pkt >> source_addr;
		// read the request nonce - if req_nonce_in_pkt_ok()
		if( profile.req_nonce_in_pkt_ok() )		pkt >> request_nonce;
	}catch(serial_except_t &e){
		return kad_err_t(kad_err_t::ERROR, "Cant parse incoming packet due to " + e.what() );
	}
	// if the realmid doesnt match, log and exit
	if( profile.realmid_in_pkt_ok() && realmid != kad_peer->get_realmid() )
		return kad_err_t(kad_err_t::ERROR, "invalid realmid");
	// if the peerid doesnt match, log and exit
	if( profile.dest_peerid_in_pkt_ok() && !dest_peerid.is_null() && dest_peerid != kad_peer->local_peerid() )
		return kad_err_t(kad_err_t::ERROR, "invalid peerid");

	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Build REQUEST header
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief build a request header
 */
pkt_t	kad_rpc_common_t::build_req_header(kad_peer_t *kad_peer, const kad_addr_t &remote_addr
					, const kad_pkttype_t &kad_pkttype
					, const kad_nonceid_t &request_nonce)	throw()
{
	const kad_rpc_profile_t &	profile	= kad_peer->get_profile().rpc();	
	pkt_t				pkt;
	// build the packet header
	pkt << kad_pkttype;
	// set the destination realm_id - if realmid_in_pkt_ok()
	if( profile.realmid_in_pkt_ok() )		pkt << kad_peer->get_realmid();
	// set the destination peerid - if dest_peerid_in_pkt_ok()
	if( profile.dest_peerid_in_pkt_ok() )		pkt << remote_addr.peerid();
	// set the source_addr of the local node
	pkt << kad_peer->srckaddr_build(remote_addr.oaddr());
	// set the request's nonce - if req_nonce_in_pkt_ok()
	if( profile.req_nonce_in_pkt_ok() )		pkt << request_nonce;
	return pkt;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Parse REPLY header
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse a reply pkt header
 */
kad_err_t kad_rpc_common_t::parse_rep_header(kad_peer_t *kad_peer, pkt_t &pkt, kad_pkttype_t &kad_pkttype
						, kad_addr_t &source_addr
						, kad_nonceid_t &request_nonce)	throw()
{
	const kad_rpc_profile_t &	profile	= kad_peer->get_profile().rpc();	
	kad_realmid_t			realmid;
	// set the pkttype_profile_t in the kad_pkttype_t
	kad_pkttype	= kad_pkttype_t(kad_peer->get_profile().pkttype());
	// parse the packet itself
	try {
		// read the packet type
		pkt >> kad_pkttype;
		// read the realm id - if realmid_in_pkt_ok()
		if( profile.realmid_in_pkt_ok() )	pkt >> realmid;
		// read the source_addr
		pkt >> source_addr;
		// read the request nonce - if req_nonce_in_pkt_ok()
		if( profile.req_nonce_in_pkt_ok() )	pkt >> request_nonce;
	}catch(serial_except_t &e){
		return kad_err_t(kad_err_t::ERROR, "Cant parse incoming packet due to " + e.what() );
	}
	// if the realmid doesnt match, log and exit
	if( profile.realmid_in_pkt_ok() && realmid != kad_peer->get_realmid() )
		return kad_err_t(kad_err_t::ERROR, "invalid realmid");
	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Build REPLY header
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief build a reply header (the same for each RPC reply)
 */
pkt_t	kad_rpc_common_t::build_rep_header(kad_peer_t *kad_peer, const ipport_addr_t &remote_oaddr
				, const kad_pkttype_t &kad_pkttype
				, const kad_nonceid_t &request_nonce)		throw()
{
	const kad_rpc_profile_t &	profile	= kad_peer->get_profile().rpc();		
	pkt_t				pkt;
	// put the packet type
	pkt << kad_pkttype;
	// insert the realm id - if realmid_in_pkt_ok()
	if( profile.realmid_in_pkt_ok() )	pkt << kad_peer->get_realmid();
	// put the source_addr
	pkt << kad_peer->srckaddr_build(remote_oaddr);
	// put the remote nonce of the request - if req_nonce_in_pkt_ok()
	if( profile.req_nonce_in_pkt_ok())	pkt << request_nonce;
	// return the just build packet
	return pkt;
}


NEOIP_NAMESPACE_END


