/*! \file
    \brief Definition of the \ref kad_peer_t::rpc_server_t
    
*/

/* system include */
/* local include */
#include "neoip_kad_peer_rpc_server.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_kad_rpc_common.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_peer_t::rpc_server_t::rpc_server_t(kad_peer_t *kad_peer)	throw()
{
	// log to debug
	KLOG_DBG("enter");	
	// copy the parameter
	this->kad_peer	= kad_peer;
}

/** \brief Destructor
 */
kad_peer_t::rpc_server_t::~rpc_server_t()				throw()
{
	// log to debug
	KLOG_DBG("enter");	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the request pkt from the kad_srvcnx_t
 * 
 * @return a pkt_t of the reply (if is_null(), dont reply anything)
 */
pkt_t	kad_peer_t::rpc_server_t::handle_request_pkt(pkt_t &pkt, const ipport_addr_t &local_oaddr
					, const ipport_addr_t &remote_oaddr)	throw()
{
	kad_err_t	kad_err;
	pkt_t		pkt_reply;
	kad_pkttype_t	pkttype;
	kad_addr_t	source_addr;
	kad_nonceid_t	request_nonce;
	// log to debug
	KLOG_DBG("enter. received pkt " << pkt);
	// update the statistic
	// - can't be in kad_srvcnx_t as it is the statistic is per kad_peer_t
	kad_peer->stat.nb_byte_recv	+= pkt.get_len();
	// parse the request pkt header
	kad_err	= kad_rpc_common_t::parse_req_header(kad_peer, pkt, pkttype, source_addr, request_nonce);
	if( kad_err.failed() ){
		KLOG_ERR("Cant parse incoming packet due to " << kad_err );
		return pkt_t();
	}

	// log to debug	
	KLOG_DBG("pkttype=" << pkttype << " FROM source_addr=" << source_addr << " remote_oaddr=" << remote_oaddr);

	// - in paper section 2.1, "When a Kademlia node receives any message (re-
	//   quest or reply) from another node, it updates the appropriate k-bucket 
	//   for the sender’s node ID."
	kad_peer->srckaddr_parse(source_addr, remote_oaddr);
	
	// set the kad_addr_filter 
	// - it allow to remove all the address which will be out of scope of the remote_oaddr
	// - e.g. if the remote oaddr is a public ip, remove all non public
	// - e.g. if the remote oaddr is a private/linklocal, remove the localhost
	kad_addr_t::filter_fct_t	kad_addr_filter	= NULL;
	if( remote_oaddr.get_ipaddr().is_public() )	kad_addr_filter =kad_addr_t::filter_fct_dstpublic;
	if( remote_oaddr.get_ipaddr().is_private() )	kad_addr_filter =kad_addr_t::filter_fct_dstprivate;
	if( remote_oaddr.get_ipaddr().is_linklocal() )	kad_addr_filter =kad_addr_t::filter_fct_dstlinklocal;
	if( remote_oaddr.get_ipaddr().is_localhost() )	kad_addr_filter =kad_addr_t::filter_fct_dstlocalhost;
	
	// build the request context
	request_t	request(pkttype, pkt, request_nonce, local_oaddr, remote_oaddr, kad_addr_filter);

	// handle the pkt according to its type
	if( pkttype.is_PING_REQUEST() )			pkt_reply = handle_ping_request(request);
	else if( pkttype.is_STORE_REQUEST() )		pkt_reply = handle_store_request(request);
	else if( pkttype.is_FINDNODE_REQUEST() )	pkt_reply = handle_findnode_request(request);
	else if( pkttype.is_FINDSOMEVAL_REQUEST() )	pkt_reply = handle_findsomeval_request(request);
	else if( pkttype.is_FINDALLVAL_REQUEST() )	pkt_reply = handle_findallval_request(request);
	else if( pkttype.is_DELETE_REQUEST() )		pkt_reply = handle_delete_request(request);
	else	DBG_ASSERT(0);

	// update the statistic
	// - can't be in kad_srvcnx_t as it is the statistic is per kad_peer_t
	kad_peer->stat.nb_byte_sent	+= pkt_reply.get_len();
	
	// return the reply
	return pkt_reply;	
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Build path_id to compute the cookie_id_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a pathid from the local/remote addresses
 * 
 * - needed to build the cookie_id_t
 */
datum_t kad_peer_t::rpc_server_t::build_pathid(const request_t &request) const throw()
{
	serial_t	serial;
	// the cookie include the remote ip address
	// - the port is not included as the client may reuse the cookie
	//   computed from it with another port
	//   - e.g. if the itor local port is dynamic (which is most likely :)
	serial << request.remote_oaddr.get_ipaddr();
	// the cookie includes the local address
	serial << request.local_oaddr;
	// log to debug
	KLOG_DBG("build a pathid based on remote ip " << request.remote_oaddr.get_ipaddr()
					<< " and local ipport " << request.local_oaddr);
	// return the result
	return datum_t(serial.get_data(), serial.get_len());
}

/** \brief Build a reply header
 */
pkt_t kad_peer_t::rpc_server_t::build_rep_header(const kad_pkttype_t &pkttype, const request_t &request) throw()
{
	return kad_rpc_common_t::build_rep_header(kad_peer, request.remote_oaddr, pkttype, request.nonce);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       handle request for specific RPC
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle a PING request
 */
pkt_t	kad_peer_t::rpc_server_t::handle_ping_request(const request_t &request)	throw()
{
	const kad_rpc_profile_t &	profile	= kad_peer->get_profile().rpc();
	// update the statistic
	kad_peer->stat.nb_ping_recv++;
	// if the rpc isn't allowed by the profile, discard the request and return now
	if( !profile.ping_rpc_ok() )	return pkt_t();

	// build the reply
	pkt_t	pkt_reply = build_rep_header(request.pkttype.PING_REPLY(), request);
	// echo the request.remote_oaddr ip_addr_t
	// - NOTE: it serves as kad_listener_t::local_ipaddr_pview() discovery
	pkt_reply << request.remote_oaddr.get_ipaddr();
	// log to debug
	KLOG_DBG("remote_ipaddr_pview=" << request.remote_oaddr.get_ipaddr());
	// return the packet reply
	return pkt_reply;
}

/** \brief handle a STORE request
 */
pkt_t	kad_peer_t::rpc_server_t::handle_store_request(const request_t &request)	throw()
{
	const kad_rpc_profile_t &profile= kad_peer->get_profile().rpc();
	datum_t		pathid_req	= build_pathid(request);
	pkt_t		pkt_request	= request.pkt;
	kad_recdups_t	kad_recdups;
	cookie_id_t	cookie_id;
	// update the statistic
	kad_peer->stat.nb_store_recv++;
	// if the rpc isn't allowed by the profile, discard the request and return now
	if( !profile.store_rpc_ok() )	return pkt_t();
	// parse the incoming packet
	try{
		// get the kad_recdups_t to store
		pkt_request >> kad_recdups;
		// get the cookie token - if cookie_store_ok()
		if( profile.cookie_store_ok() )	pkt_request >> cookie_id;
	}catch(serial_except_t &e){
		KLOG_ERR("Unable to parse a store request due to " << e.what());
		return pkt_t();
	}

	// if the cookie is invalid, log the event and discard the request - if cookie_store_ok()
	// - never put it in the cookie database, as the cookie isnt computed over a nonce, so 
	//   several requests may provide the same cookie token and still be valid.
	if( profile.cookie_store_ok() && !cookie_db.check(pathid_req, cookie_id) ){
		KLOG_NOTICE("Received an invalid cookie");
		return pkt_t();
	}

	// process all the records of the kad_recdups_t
	for( size_t i = 0; i < kad_recdups.size(); i++ ){
		kad_rec_t	kad_rec = kad_recdups[i];
		// if the record is not acceptable, return now, dont reply and discard the rest of recdups
		if( !kad_peer->remote_db->may_accept(kad_rec) )	return pkt_t();
		// put the record in the remote database
		kad_peer->remote_db->update(kad_rec);
	}
	
	// build the reply
	return build_rep_header(request.pkttype.STORE_REPLY(), request);
}

/** \brief handle a FINDNODE request
 */
pkt_t	kad_peer_t::rpc_server_t::handle_findnode_request(const request_t &request)	throw()
{
	const kad_rpc_profile_t &profile= kad_peer->get_profile().rpc();	
	datum_t		pathid_req	= build_pathid(request);	
	pkt_t		pkt_request	= request.pkt;
	kad_targetid_t	targetid;
	// update the statistic
	kad_peer->stat.nb_findnode_recv++;
	// if the rpc isn't allowed by the profile, discard the request and return now
	if( !profile.findnode_rpc_ok() )	return pkt_t();
	// get the target_id
	try{
		pkt_request >> targetid;
	}catch(serial_except_t &e){
		KLOG_ERR("Unable to parse a find request due to " << e.what());
		return pkt_t();
	}

	// get the n closest nodes to this peerid from the kbucket
	kad_addr_arr_t	kad_addr_arr = kad_peer->kbucket->get_nclosest_addr(targetid
						, kad_peer->get_profile().kbucket().nb_replication()
						, request.kad_addr_filter);
	// build the reply
	pkt_t	pkt_reply = build_rep_header(request.pkttype.FINDNODE_REPLY(), request);
	// put the kad_addr_arr
	pkt_reply << kad_addr_arr;
	// put the cookie after the kad_addr_arr - if cookie_in_reply_ok()
	if( profile.cookie_in_reply_ok() )
		pkt_reply << cookie_db.generate(pathid_req);
	// log to debug
	KLOG_DBG("kad_addr_arr=" << kad_addr_arr);
	// return the packet reply
	return pkt_reply;
}

/** \brief handle a FINDSOMEVAL request
 * 
 * - from the paper, "findval behaves like findnode - returning IP address, UDP port,
 *   No de ID triples—with one exception. If the RPC recipient has received a store
 *   RPC for the key, it just returns the stored value."
 */
pkt_t	kad_peer_t::rpc_server_t::handle_findsomeval_request(const request_t &request)	throw()
{
	const kad_rpc_profile_t &profile= kad_peer->get_profile().rpc();	
	datum_t		pathid_req	= build_pathid(request);	
	pkt_t		pkt_request	= request.pkt;
	kad_keyid_t	keyid;
	uint32_t	requested_nb_rec;
	// update the statistic
	kad_peer->stat.nb_findsomeval_recv++;
	// if the rpc isn't allowed by the profile, discard the request and return now
	if( !profile.findsomeval_rpc_ok() )	return pkt_t();
	// parse the incoming packet
	try{
		// get the keyid of the records
		pkt_request >> keyid;
		// get the maximum number of records to reply
		pkt_request >> requested_nb_rec;
	}catch(serial_except_t &e){
		KLOG_ERR("Unable to parse a find request due to " << e.what());
		return pkt_t();
	}

	// init the reply
	pkt_t	pkt_reply = build_rep_header(request.pkttype.FINDSOMEVAL_REPLY(), request);
	// try to get record from the database
	kad_recdups_t	kad_recdups = kad_peer->remote_db->find_by_keyid(keyid);

	// log to debug
	KLOG_DBG("request nb record=" << requested_nb_rec << " keyid=" << keyid
					<< " nb found=" << kad_recdups.size());

	// if some records have been found, reply the record
	if( kad_recdups.size() ){
		// randomize kad_recdups order to provide a random sample
		// - it MUST be done before truncate or any size measurement
		kad_recdups.randomize_order();
		// compute the maximum number of records which fit in the maximum packet size
		size_t	pkt_max_len	= profile.replypkt_maxlen();
		size_t	max_nb_rec_pkt 	= kad_recdups.nb_serialized_rec_in_size( pkt_max_len );
		// the number of nodes to reply is the minimum between max_nb_rec_pkt and requested_nb_rec
		if( requested_nb_rec && max_nb_rec_pkt > requested_nb_rec )
			max_nb_rec_pkt = requested_nb_rec;
		// compute the number of records to delete
		size_t	nb_rec_to_del	= kad_recdups.size() - max_nb_rec_pkt;
		// remove the nodes in excess at the tail of kad_recdups
		kad_recdups.truncate_at_tail(nb_rec_to_del);
		
		// put record_found at 1 in the reply
		pkt_reply << uint8_t(1);
		// set has_more_record
		pkt_reply << uint8_t(nb_rec_to_del ? 1 : 0);	
		// put the kad_recdups in the reply
		pkt_reply << kad_recdups;
	}else{
		// if no record have been found, reply the nclosest nodes
		kad_addr_arr_t	kad_addr_arr= kad_peer->kbucket->get_nclosest_addr( keyid
						, kad_peer->get_profile().kbucket().nb_replication()
						, request.kad_addr_filter);
		// put record_found at 0 in the reply
		pkt_reply << uint8_t(0);
		// put the kad_addr_arr in the reply
		pkt_reply << kad_addr_arr;
		// put the cookie after the kad_addr_arr - if cookie_in_reply_ok()
		if( profile.cookie_in_reply_ok() )
			pkt_reply << cookie_db.generate(pathid_req);	
	}
	// return the reply
	return pkt_reply;
}

/** \brief handle a FINDALLVAL request
 * 
 * - This is a modification of the original FINDONEVAL to be able to return several records
 *   matching the key and with to dump the whole database of a given node
 * - it returns a list of records ordered by their record id
 *   - if recid_gt is not null, the return record id are ALL greater than recid_gt except
 *     if keyid_ge is set and has been used
 *   - if keyid_ge is true, and if there is norecord matching keyid,recid_gt, the 'next' key
 *     is used with a null recid_gt
 * - from the paper, "findoneval behaves like findnode - returning IP address, UDP port,
 *   No de ID triples—with one exception. If the RPC recipient has received a store
 *   RPC for the key, it just returns the stored value."
 */
pkt_t	kad_peer_t::rpc_server_t::handle_findallval_request(const request_t &request)	throw()
{
	const kad_rpc_profile_t &profile= kad_peer->get_profile().rpc();	
	datum_t		pathid_req	= build_pathid(request);	
	pkt_t		pkt_request	= request.pkt;
	kad_recid_t	recid_gt;
	kad_keyid_t	keyid;
	uint8_t		keyid_ge;
	kad_recdups_t	kad_recdups;
	uint32_t	requested_nb_rec;	
	// update the statistic
	kad_peer->stat.nb_findallval_recv++;
	// if the rpc isn't allowed by the profile, discard the request and return now
	if( !profile.findallval_rpc_ok() )	return pkt_t();
	// get the keyid
	try{
		pkt_request >> recid_gt;
		pkt_request >> keyid;
		pkt_request >> keyid_ge;
		pkt_request >> requested_nb_rec;
	}catch(serial_except_t &e){
		KLOG_ERR("Unable to parse a find request due to " << e.what());
		return pkt_t();
	}

	// try to get record for this keyid
	kad_recdups	= kad_peer->remote_db->find_by_keyid(keyid, recid_gt);
	// if no record is found for this keyid and key_ge is set, try with the next key
	if( kad_recdups.empty() && keyid_ge ){
		// get the next keyid which has records in the database
		keyid		= kad_peer->remote_db->get_next_keyid(keyid);
		// try to get records with this new keyid
		// - the recid_gt is reset, as it is a new key
		// - works even if there is no more key. as get_next_keyid() will return null
		//   and it is not allowed to have a record with a null keyid, so none will match
		kad_recdups	= kad_peer->remote_db->find_by_keyid(keyid, kad_recid_t());
	}

	// sort the found records in increasing order by their record id
	kad_recdups.sort_by_recid();
	// compute the maximum number of records which fit in the maximum packet size
	size_t	pkt_max_len	= profile.replypkt_maxlen();
	size_t	max_nb_rec_pkt 	= kad_recdups.nb_serialized_rec_in_size(pkt_max_len);
	// if the requester asks for less than max_nb_rec_pkt, clamp max_nb_rec_pkt
	if( requested_nb_rec && max_nb_rec_pkt > requested_nb_rec )
		max_nb_rec_pkt = requested_nb_rec;
	// compute the number of records to delete
	size_t	nb_rec_to_del	= kad_recdups.size() - max_nb_rec_pkt;
	// remove the nodes in excess at the tail of kad_recdups
	kad_recdups.truncate_at_tail(nb_rec_to_del);

	// init the reply
	pkt_t	pkt_reply = build_rep_header(request.pkttype.FINDALLVAL_REPLY(), request);	
	// set has_more_record
	pkt_reply << uint8_t(nb_rec_to_del ? 1 : 0);
	// put the prepared kad_recdups
	pkt_reply << kad_recdups;
	
	// return the reply
	return pkt_reply;
}

/** \brief handle a DELETE request
 */
pkt_t	kad_peer_t::rpc_server_t::handle_delete_request(const request_t &request)	throw()
{
	const kad_rpc_profile_t &profile= kad_peer->get_profile().rpc();	
	datum_t		pathid_req	= build_pathid(request);	
	pkt_t		pkt_request	= request.pkt;
	kad_keyid_t	keyid;
	kad_recid_t	recid;
	cookie_id_t	cookie_id;
	// update the statistic
	kad_peer->stat.nb_delete_recv++;
	// if the rpc isn't allowed by the profile, discard the request and return now
	if( !profile.delete_rpc_ok() )	return pkt_t();
	// get the keyid, recid
	try{
		pkt_request >> recid;
		// get the cookie token - if cookie_delete_ok()
		if( profile.cookie_delete_ok() )	pkt_request >> cookie_id;
	}catch(serial_except_t &e){
		KLOG_ERR("Unable to parse a find request due to " << e.what());
		return pkt_t();
	}

	// if the cookie is invalid, log the event and discard the request - if cookie_store_ok()
	// - never put it in the cookie database, as the cookie isnt computed over a nonce, so 
	//   several requests may provide the same cookie token and still be valid.
	if( profile.cookie_delete_ok() && !cookie_db.check(pathid_req, cookie_id) ){
		KLOG_NOTICE("Received an invalid cookie");
		return pkt_t();
	}
	
	// remove the record - works even if the record is not in the database
	kad_peer->remote_db->remove(recid);

	// build the reply
	return build_rep_header(request.pkttype.DELETE_REPLY(), request);
}

NEOIP_NAMESPACE_END;






