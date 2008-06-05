/*! \file
    \brief Definition of the router_itor_t

\par Brief description
- this modules handles 
 -# do query on the underlying name service to get the socket_addr_t of the remote_peerid
 -# spawn_t a cnx_t for each received address
 -# it stops when (i) all cnx_t failed or (ii) a cnx_t succeed. at this time
    all the queued dnsgrab_request_t are replied according to the result.
    - in case of success, a router_full_t spawned
- description of the address negociation protocol can be found in neoip_router_itor_cnx.cpp
  header.

- Jim McCoy <mccoy@mad-scientist.com>
  - ex emails guy from yahoo, got 50millions for it! bastard
  - willing to do stuff with tunnel :)

*/

/* system include */
#include <netinet/ip_icmp.h>
/* local include */
#include "neoip_router_itor.hpp"
#include "neoip_router_itor_cnx.hpp"
#include "neoip_router_full.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_netif_vdev.hpp"
#include "neoip_ippkt_util.hpp"
#include "neoip_kad.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_dnsgrab.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_itor_t::router_itor_t(router_peer_t *router_peer)	throw()
{
	// copy the parameters
	this->router_peer	= router_peer;
	// link this router_itor_t to the router_peer_t
	router_peer->itor_dolink(this);
}

/** \brief Destructor
 */
router_itor_t::~router_itor_t()	throw()
{
	// unlink this router_itor_t from the router_peer_t
	router_peer->itor_unlink(this);
	// close all pending router_itor_t
	while(!cnx_db.empty())	nipmem_delete cnx_db.front();
	// delete the kad_query if needed
	nipmem_zdelete kad_query;
}

/** \brief autodelete the router_itor_t when it has been unsuccessfull
 */
bool	router_itor_t::autodelete_due2err()	throw()
{
	// put the remote_peerid in the itor_negcache
	router_peer->itor_negcache.insert(remote_peerid());
	// send an ICMP error to each pending packet still queued in innerpkt_queue
	// - NOTE: ok without risk of duplication as this remove the item as they are used
	while( !innerpkt_queue.empty() ){
		netif_vdev_t *	catchall_netif	= router_peer->catchall_netif;
		// get the first packet of the queue
		innerpkt_t innerpkt	= innerpkt_queue.front();
		// remove the first packet of the queue
		innerpkt_queue.pop_front();
		// create alias variable to ease readability
		const uint16_t &ethtype	= innerpkt.first;
		const pkt_t &	pkt	= innerpkt.second;
		// if the ethertype is NOT ipv4, goto the next packet
		if(ethtype != netif_vdev_t::TYPE_IP4)	continue;
		// build the icmp packet to reply
		pkt_t	icmp_pkt= ippkt_util_t::build_icmp4_pkt(ippkt_util_t::get_dst_addr(pkt)
				, ippkt_util_t::get_src_addr(pkt)
				, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, 0, pkt.to_datum());
		// send the icmp packet
		catchall_netif->send_pkt(netif_vdev_t::TYPE_IP4, icmp_pkt);
	}
	// notify all pending dnsgrab_request_t as not_found
	// - NOTE: ok without risk of duplication as this remove the item as they are used
	notify_dnsgrab(NULL);
	// autodelete
	nipmem_delete this;
	// always return false - for convenience with the 'dontkeep' mechanism for callback
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup the router
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
router_err_t router_itor_t::start(const router_peerid_t &m_remote_peerid
					, const router_name_t &m_remote_dnsname)	throw()
{
	const router_profile_t &profile	= router_peer->get_profile();	
	// log to debug
	KLOG_DBG("enter remote peerid=" << remote_peerid.to_canonical_string() );
	
	// sanity check - this MUST be the only router_itor_t to run on this remote_peerid_t
	DBG_ASSERT( !router_peer->itor_by_remote_peerid(m_remote_peerid) );
	// sanity check - the m_remote_dnsname MUST be is_fully_qualified()
	DBG_ASSERT( m_remote_dnsname.is_fully_qualified() );
	
	// copy the parameters
	this->m_remote_peerid	= m_remote_peerid;
	this->m_remote_dnsname	= m_remote_dnsname;

	// init some variable for the kad_query_t
	kad_keyid_t	record_keyid	= remote_peerid().to_canonical_string();
	kad_peer_t *	kad_peer	= router_peer->get_kad_peer();
	kad_err_t	kad_err;
	// create and start the kad_query_t
	kad_query	= nipmem_new kad_query_t();
	kad_err		= kad_query->start(kad_peer, record_keyid, 0, delay_t::INFINITE, this, NULL);
	if( kad_err.failed() )	return router_err_from_kad(kad_err);

	// init the itor_expire_timeout
	expire_timeout.start(profile.itor_expire_delay(), this, NULL);
	
	// return no error
	return router_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool router_itor_t::neoip_timeout_expire_cb(void *cb_userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("timedout after " << OSTREAMSTR(expire_timeout.get_period()) );
	// autodelete the router_itor_t
	return autodelete_due2err();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       innerpkt_queue
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Queue a inner packet to this router_itor_t
 * 
 * - similar to 'arp packet queue' in rfc1122.2.3.2.2 "The link layer SHOULD save (rather 
 *   than discard) at least one (the latest) packet of each set of packets destined to the
 *   same unresolved IP address, and transmit the saved packet when the address has been 
 *   resolved."
 */
router_itor_t &	router_itor_t::queue_inner_pkt(uint16_t ethertype, const pkt_t &pkt)		throw()
{
	const router_profile_t &profile	= router_peer->get_profile();
	// if the innerpkt_queue already reached its maximum size, remove one pkt to leave room to new one
	if(innerpkt_queue.size() == profile.itor_innerpkt_queue_maxlen())	innerpkt_queue.pop_front();
	// add the pkt_t to the innerpkt_queue
	innerpkt_queue.push_back( std::make_pair(ethertype, pkt) );
	// sanity check - the innerpkt
	DBG_ASSERT( innerpkt_queue.size() <= profile.itor_innerpkt_queue_maxlen() );
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                              dnsgrab_req_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Queue a dnsgrab_request for this router_itor_t
 */
router_itor_t &	router_itor_t::queue_dnsgrab_request(const dnsgrab_request_t &dnsgrab_request)	throw()
{
	// add the dnsgrab_request_t to the dnsgrab_req_db
	dnsgrab_req_db.push_back(dnsgrab_request);
	// return the object itself
	return *this;
}

/** \brief Notify all the queue dnsgrab_request_t of the result
 * 
 * - if the router_full_t is null, notify notfound
 * - else notify its newly established inner address
 */
void	router_itor_t::notify_dnsgrab(const router_full_t *router_full)	throw()
{
	dnsgrab_t *		dnsgrab = router_peer->get_dnsgrab();
	dnsgrab_request_t	request;
	// go thru the whole dnsgrab_req_db
	while( !dnsgrab_req_db.empty() ){
		// get the first dnsgrab_request_t from the list
		request = dnsgrab_req_db.front();
		// remove it from the list
		dnsgrab_req_db.pop_front();
		// set the dnsgrab_request_t according to the result
		if( router_full )	router_peer->set_dnsreq_reply_cnxfull(request, router_full);
		else			router_peer->set_dnsreq_reply_notfound(request);
		// notify the reply
		dnsgrab->notify_reply(request);
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_query_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a command in a kad_query_t has an event to notify
 */
bool	router_itor_t::neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << kad_event);
	// sanity check - the kad_event MUST be query_ok()
	DBG_ASSERT( kad_event.is_query_ok() );
	// sanity check - the reporter cb_kad_query MUST be the local one
	DBG_ASSERT( kad_query == &cb_kad_query );
	// sanity check - the kad_event_t MUST be a kad_event_t::RECDUPS
	// - kad_event_t::TIMEDOUT is possible on kad_query but not here as the timeout is delay_t::NEVER
	// - TODO there is an issue with that... as i no more do delay_t::NEVER
	// - sort out who does the expire_delay.. one should exist
	DBG_ASSERT( kad_event.is_recdups() );

	// copy the kad_recdups_t before deleting the command
	kad_recdups_t	kad_recdups	= kad_event.get_recdups(NULL);

	// delete the kad_query_t and mark is unused
	nipmem_zdelete	kad_query;

	// if some records have been found, handle the received kad_recdups_t	
	if( !kad_recdups.empty() )	handle_recved_kad_recdups(kad_recdups);

	// if cnx_cb is now empty, no router_itor_cnx_t is running so notify the faillure
	if( cnx_db.empty() )		return autodelete_due2err();

	// return 'dontkeep' as the command has just been deleted
	return false;
}


/** \brief Handle the received kad_recdups
 */
void	router_itor_t::handle_recved_kad_recdups(const kad_recdups_t &kad_recdups)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// go thru all the received kad_rec_t
	for(size_t i = 0; i < kad_recdups.size(); i++){
		const kad_rec_t &	kad_rec	= kad_recdups[i];
		bytearray_t		payload( kad_rec.get_payload() );
		socket_addr_t		remote_oaddr;
		// parse the incoming packet
		try {
			payload	>> remote_oaddr;
		}catch(serial_except_t &e){
			// log the event
			KLOG_ERR("Can't parse peer record " << kad_rec << " due to " << e.what() );
			// goto the next record even if it is a serious error which MUST NOT happen
			continue;
		}
		// handle this particular remote_addr
		handle_recved_remote_oaddr(remote_oaddr);
	}
}

/** \brief Handle the reception of a socket_addr_t from the kad_query_t
 */
void	router_itor_t::handle_recved_remote_oaddr(const socket_addr_t &remote_oaddr)	throw()
{
	router_err_t	router_err;
	// log to debug
	KLOG_DBG("enter remote_oaddr=" << remote_oaddr);

	// try to find another cnx_t running on this remote_addr
	std::list<router_itor_cnx_t *>::iterator	iter;
	for( iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		router_itor_cnx_t *	itor_cnx = *iter;
		if( remote_oaddr == itor_cnx->get_remote_oaddr() )	break;
	}
	// if one have been found, do nothing and return now
	if( iter != cnx_db.end() )	return;

	// launch a cnx_t on this remote_addr
	router_itor_cnx_t *	itor_cnx;
	itor_cnx	= nipmem_new router_itor_cnx_t(this);
	router_err	= itor_cnx->start(remote_oaddr, this, NULL);
	if( router_err.failed() )	nipmem_delete itor_cnx;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 router_itor_cnx_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref router_itor_cnx_t when a connection is established
 */
bool	router_itor_t::neoip_router_itor_cnx_cb(void *cb_userptr, router_itor_cnx_t &cb_itor_cnx
					, socket_full_t *socket_full
					, const ip_addr_t &local_iaddr, const ip_addr_t &remote_iaddr
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	router_err_t		router_err;
	router_itor_cnx_t *	itor_cnx	= &cb_itor_cnx;
	// log to debug
	KLOG_DBG("enter");
	
	// if the router_itor_cnx_t has reported faillure
	if( socket_full == NULL ){
		// delete the router_itor_cnx_t
		nipmem_zdelete	itor_cnx;
		// if cnx_cb is now empty, notify the faillure
		if( cnx_db.empty() )	return autodelete_due2err();
		// return 'dontkeep' as the itor_cnx is deleted at this point
		return false;
	}
	// NOTE: here the router_itor_cnx_t has reported success

	// TODO here put a sanity check on the remote_peerid being the expected one
	
	// spawn a router_full_t from the socket_full_t and the chosen_itor/resp_iaddr
	router_full_t *	router_full;
	router_full	= nipmem_new router_full_t(router_peer);
	router_err	= router_full->start(socket_full, local_iaddr, remote_iaddr,estapkt_in,estapkt_out);
	if( router_err.failed() ){
		nipmem_delete router_full;
		return autodelete_due2err();
	}

	// if the router_full_t has been successfully started, notify dnsgrab with the remote_iaddr
	notify_dnsgrab(router_full);

	// if the innerpkt_queue is not empty, try to flush it thru the just built router_full_t
	// - similar to 'arp packet queue' in rfc1122.2.3.2.2
	while( !innerpkt_queue.empty() ){
		// get the first packet of the queue
		innerpkt_t innerpkt	= innerpkt_queue.front();
		// remove the first packet of the queue
		innerpkt_queue.pop_front();
		// forward it thru the router_full_t
		router_full->forward_pkt(innerpkt.first, innerpkt.second);
	}
	
	// autodelete
	nipmem_delete this;
	// return dontkeep
	return false;
}
NEOIP_NAMESPACE_END

