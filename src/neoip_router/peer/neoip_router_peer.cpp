/*! \file
    \brief Definition of the router_peer_t

- TODO have ntudp_peer passed by the caller
  - application of the regularity rules
  - the same for the kad_peer_t passed to the ntudp_peer_t
  - have a router_ezpeer_t handling router_peer_t and satelite object
    - same as the bt_session_t and bt_ezsession_t

- similar issue with the ntudp_peer_t
  - it create a kad_listener_t and kad_peer_t and udp_vresp_t
  - those should not be inside it
  - code a ntudp_ezpeer_t which does it
  - ntudp_peer_t only receive a kad_peer_t and a udp_vresp_t

\par About mobility implementation
- GOAL: to be able to keep the applications connection when the remote peer
  is renumering its ip address.
- TODO need more testing to ensure what is the good policy
- renumering may happen in several cases such as
  - mobility: a host is moving from one point to another with another ip address
  - adsl modem: from time to time, asdl modem reboot or renumber 
- NOTE: apparently closing the netif_vdev_t of router_full_t freeze a tcp
  connection on linux
  - even if another netif_vdev_t with the same ip address and proper routing
    it reenable later, the tcp connection doesnt goes on
  - POSSIBLE SOLUTION:
    - maintaining a netif_vdev_t cache
    - when the socket_full_t of router_full_t closes, the netif_vdev_t is 
      passed to the cache
    - when the netif_vdev_t stays in the cache for too long, it is closed
    - if a router_full_t is restarted for this destination, it steals the 
      netif_vdev_t from the cache
    - in theory the tcp connection should goes on, TODO to test
    - packet received by the netif_vdev_t in the cache should trigger router_itor_t
      the same way it is done when received by the catchall_netif
    - the same destination is NEVER in the netif_vdev_t cache and in the router_full_t

\par Possible Improvement: handling several local identity
- i dunno exactly why but i fell it will be necessary to handle several local
  identities in the future
- use case:
  - i have several friends with which i have configured selfsigned identity
    in a ssh-like PKI
  - then i decide to register another identity, but authsigned, to be reachable by 
    more people
  - all my friends need to reconfigure in order to still be reachable by me
  - this dammages the flexibility and reduce the possibility to changes identity.
- POSSIBLE SOLUTION:
  - support several router_lident_t in router_peer_t
  - support several router_resp_t, one per router_lident_t
  - have a configuration file with a routing table to determine which router_lident_t
    based on the destination router_name_t
  - in the router_itor_t, use this routing table to choose the x509_cert_t to send.
- another possibility would be to support this directly in the scnx layer...
  - not sure it would be an advantage
  - maybe a mechanism could automatize the certificate selection and thus avoid
    the configuration of the identity routing table.
    - but currently i dont see how it could be done
    - nor im convinced it could be done
  - it may be argued as more bloated
  - i think it may end up exactly as with the same solution as above but all 
    in the scnx layer instead of the router_itor_t

\par Note about the router_profile_t::tunnel_stub()
- this is a debug option allowing me to run 2 routers on the same box
  - it is normally impossible due to the kernel routing which got all the
    packet before userspace
- the trick is to get one full blown router and several tunnel_stub router
- a tunnel_stub router acts as a router BUT dont interfere with the local host
  - so a tunnel_stub is able to run on a normal user (no need for root)
  - a tunnel_stub DO NOT have a netif_vdev
  - a tunnel_stub DO NOT have a dnsgrab

\par Possible improvement for routerapps
- do a dyndns like
  - all that with a root certificate
  - the timeout of the certificate is tunable depending on the client
- do a global dhcp
  - to allocate global address and not only link2link address
  - with a tuneable timeout depending on the client

\par Possible improvement - have up/down script when a tunnel goes up/down
- this allows to trigger operations before starting forwarding
  - typically to setup a firewall on the connection
- FROM HERE: it is only vague notes taken when thinking about it
- ok but how to code it ? those up/down script are async and consume time
  - what happen if a connection is currently closing down while another one
    attempts to start again on the same host ?
  - i dont have enought information/memory on router_peer_t to code this now
  - additionnaly ubuntu defaults to 'no firewall' and doesnt seem to have
    trouble about it. so it is not a big emergency
- how to handle closeure in route_full_t
  - race: curren is closing and another is starting
    - conflict
    - must be deterministic/clean
  - how the identity is handled ? how do i know which one conflict
    - hostname ? peerid ?
- what parameters for the script ?
  - the ip address, the netif name, the fully-qualified-dnsname
- what about :
  1. wait for any full in closing to go off
  2. start full 
- another:
  - ignore any connect attempt if full is closing
  - simple
  - this race is rare so no efficiency trouble
  - have little impact on the code which is good
- do i need state in the router_full_t ?
  - initing: running up
  - fwding
  - closing: running down
- need more info/memory about router_peer_t
  - e.g. addr nego is already somewhere and is a 'initing'
  - up script is at the same level
*/

/* system include */
#include <netinet/ip_icmp.h>
/* local include */
#include "neoip_router_peer.hpp"
#include "neoip_router_resp.hpp"
#include "neoip_router_itor.hpp"
#include "neoip_router_full.hpp"
#include "neoip_pkt.hpp"
#include "neoip_ippkt_util.hpp"
#include "neoip_netif_vdev.hpp"
#include "neoip_netif_addr.hpp"
#include "neoip_dnsgrab.hpp"
#include "neoip_dnsgrab_request.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_kad.hpp"
#include "neoip_kad_publish.hpp"
#include "neoip_socket.hpp"
#include "neoip_ip_netaddr_arr.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_string.hpp"
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
router_peer_t::router_peer_t()	throw()
{
	// zero some field
	catchall_netif	= NULL;
	dnsgrab		= NULL;
	ntudp_peer	= NULL;
	kad_publish	= NULL;
	router_resp	= NULL;
}

/** \brief Destructor
 */
router_peer_t::~router_peer_t()	throw()
{
	// close all pending router_itor_t
	while(!itor_db.empty())	nipmem_delete	itor_db.front();
	// close all pending router_full_t
	while(!full_db.empty())	nipmem_delete	full_db.front();
	// delete router_resp_t if needed
	nipmem_zdelete	router_resp;
	// delete dnsgrab_t if needed
	nipmem_zdelete	dnsgrab;
	// delete catchall_netif if needed - linux remove the routes by itself
	nipmem_zdelete	catchall_netif;	
	// delete kad_publish_t if needed
	nipmem_zdelete	kad_publish;
	// delete ntudp_peer_t if needed
	nipmem_zdelete	ntudp_peer;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup the router
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
router_peer_t &	router_peer_t::set_profile(const router_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check().succeed() );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Set the acache for this object
 * 
 * - TODO why is this a special function... how is it different from rident_arr or rootca_arr ?
 */
router_peer_t &	router_peer_t::set_acache(const router_acache_t &m_acache)	throw()
{	
	// copy the parameter
	this->m_acache	= m_acache;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
router_err_t router_peer_t::start(const ipport_aview_t &ipport_listen, const router_lident_t &m_lident
					, const router_rident_arr_t &m_rident_arr
					, const router_rootca_arr_t &m_rootca_arr
					, const router_acl_t &m_acl
					, const ip_netaddr_arr_t &ip_netaddr_arr)	throw()
{
	router_err_t	router_err;
	// log to debug
	KLOG_ERR("enter ipport_listen_aview=" << ipport_listen);
	// copy some parameter
	this->m_lident		= m_lident;
	this->m_rident_arr	= m_rident_arr;
	this->m_rootca_arr	= m_rootca_arr;
	this->m_acl		= m_acl;
	this->ip_netaddr_arr	= ip_netaddr_arr;
	
	// sanity check - the ip_netaddr_arr MUST NOT be empty
	DBG_ASSERT( !ip_netaddr_arr.empty() );
	
	// set the default ttl for the entried in itor_negcache
	itor_negcache.set_default_ttl(profile.itor_negcache_ttl());
	
	// TODO the choice of the catchall addr MUST be reviewed
	// - should i use the same algo as in the address negociation ?
	catchall_iaddr	= get_avail_iaddr()[0].min_value();

	// setup the catchall_netif IIF profile.tunnel_stub() IS NOT set
	if( !profile.tunnel_stub() ){
		router_err	= catchall_netif_ctor();
		if( router_err.failed() )	return router_err;
	}

	// create the dnsgrab_t IIF profile.tunnel_stub() IS NOT set
	if( !profile.tunnel_stub() ){
		dnsgrab_err_t	dnsgrab_err;
		dnsgrab		= nipmem_new dnsgrab_t(this, NULL);
		dnsgrab_err	= dnsgrab->start(profile.dnsgrab_arg());
		if( dnsgrab_err.failed() )	return router_err_from_dnsgrab(dnsgrab_err);
	}

	// start the ntudp_peer
	// TODO all the start() parameter must be set by the caller
	// - a ntudp_peer_arg_t containing all those ?
	// - what about the ntudp_profile_t ?
	// - what about letting the caller set the ntudp_peer ? :)
	//   - YES with a router_ezpeer_t which does a ntudp_peer_t and a router_peer_t
	// - more on that noted in the file header
	// - this ntudp_pos_res_t MUST be cached in a file
	// - POSSIBLE SOLUTION: have it stored in the tmp_dir in the ntudp_ezpeer
	ntudp_npos_res_t npos_res	= ntudp_npos_res_t::from_ipport_aview(ipport_listen);
	DBG_ASSERT( npos_res.completed() );

	// Start the ntudp_peer_t
	ntudp_err_t	ntudp_err;
	ntudp_peer	= nipmem_new ntudp_peer_t();
	ntudp_peer->set_rdvpt_ftor(ntudp_rdvpt_ftor_t(this, NULL));
	// TODO should the ntudp_peerid_t be directly derived from the static router_peerid_t ?
	ntudp_err	= ntudp_peer->start(ipport_listen, npos_res, ntudp_peerid_t::build_random());	
	if( ntudp_err.failed() )	return router_err_from_ntudp(ntudp_err);

	// Start the router_resp_t
	socket_addr_t	resp_listen_addr	= "ntlay://" + ntudp_peer->local_peerid().to_string()
							+ ":" + ntudp_portid_t::build_random().to_string();
	router_resp	= nipmem_new router_resp_t(this);
	router_err	= router_resp->start(resp_listen_addr);	
	if( router_err.failed() )	return router_err;

	// publish the peer record on the kad_peer_t
	publish_peer_record();

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/router_peer_" + lident().peerid().to_string());

	// return no error
	return router_err_t::OK;
}

/** \brief Initialization for the catchall_netif
 */
router_err_t	router_peer_t::catchall_netif_ctor()	throw()
{
	// sanity check - the catchall_iaddr MUST BE set
	DBG_ASSERT( !catchall_iaddr.is_null() );
	// sanity check - the ip_netaddr_arr_t MUST be set
	DBG_ASSERT( !ip_netaddr_arr.empty() );
	// start the catchall_netif
	netif_err_t	netif_err;
	catchall_netif	= nipmem_new netif_vdev_t();
	netif_err	= catchall_netif->start(this, NULL, netif_vdev_t::TYPE_TUN);
	if( netif_err.failed() )	return router_err_from_netif(netif_err);

	// set the netif_vdev UP
	netif_err	= catchall_netif->set_updown(netif_util_t::IF_UP);
	if( netif_err.failed() )	return router_err_from_netif(netif_err);

	// setup the ip_netaddr
	netif_err	= catchall_netif->set_netaddr( ip_netaddr_t(catchall_iaddr, 32) );
	if( netif_err.failed() )	return router_err_from_netif(netif_err);

	// set all the ip_netaddr_t as route for the catchall netif_vdev
	for(size_t i = 0; i < ip_netaddr_arr.size(); i++){
		netif_err	= catchall_netif->add_route( ip_netaddr_arr[i] );
		if( netif_err.failed() )	return router_err_from_netif(netif_err);
	}
	// return no error
	return router_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the kad_peer_t on which the router records are stored/query
 * 
 * - it is taken directly from the ntudp_peer_t
 */
kad_peer_t *	router_peer_t::get_kad_peer()	const throw()
{
	return ntudp_peer->kad_peer();
}

/** \brief Retrun a interval_t of all the available inner ip_addr_t
 */
ip_addr_inval_t	router_peer_t::get_avail_iaddr()	const throw()
{
	ip_addr_inval_t	result;
	// derive the ip_addr_inval_t from the ip_netaddr_arr
	for(size_t i = 0; i < ip_netaddr_arr.size(); i++){
		const ip_netaddr_t &	ip_netaddr	= ip_netaddr_arr[i];
		result += ip_addr_inval_item_t( ip_netaddr.get_first_addr(), ip_netaddr.get_last_addr() );
	}	
	// remove all the already used ones
	result -= get_used_iaddr();
	// return the result
	return result;
}

/** \brief Retrun a interval_t of all the currently used inner ip_addr_t
 * 
 * - TODO what about the address in the router_acache_t ?
 *   - it should not be allocated
 *   - or maybe on last resort - aka if no other is available
 *   - i dunno how to handle it for now
 *     - maybe in the way the interval is handled
 *     - currently i dont remember it
 *     - e.g. the algo try to satisfy the interval one by one
 *     - put a interval array and put the most prefered interval first
 */
ip_addr_inval_t	router_peer_t::get_used_iaddr()	const throw()
{
	std::list<router_full_t *>::const_iterator	iter;
	ip_addr_inval_t					result;
	// go thru all the address used in local router_full_t
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		router_full_t *	router_full	= *iter;
		// add the local_iaddr of this router_full_t
		result	+= ip_addr_inval_item_t( router_full->local_iaddr() );
		// add the remote_iaddr of this router_full_t
		result	+= ip_addr_inval_item_t( router_full->remote_iaddr() );
	}
	
	// add all the ip address which are used externally to this router
	// - NOTE: important to allow multiple instance of router to run simultaneously
	netif_addr_arr_t	netif_addr_arr = netif_addr_t::get_all_netif();
	for(size_t i = 0; i < netif_addr_arr.size(); i++){
		netif_addr_t &	netif_addr	= netif_addr_arr[i];
		result	+= ip_addr_inval_item_t( netif_addr.get_ip_netaddr().get_base_addr() );
	}	
	// return the result
	return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ntudp_rdvpt_ftor_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the ntudp_rdvpt_t MUST be discarded, false otherwise
 * 
 * - filter out the ntudp_rdvpt_t use the inner addresses
 * - this is usefull to avoid nested issues i.e. the router using the 
 *   connection established by itself.
 */
bool router_peer_t::ntudp_rdvpt_ftor_cb(void *userptr, const ntudp_rdvpt_t &ntudp_rdvpt)	throw()
{
	// log to debug
	KLOG_DBG("ntudp_rdvpt=" << ntudp_rdvpt);
	// if the ntudp_rdvpt_t is NOT DIRECT, accept it
	if( ntudp_rdvpt.get_type() != ntudp_rdvpt_t::DIRECT )	return false;

	// set all the ip_netaddr_t as route for the catchall netif_vdev
	for(size_t i = 0; i < ip_netaddr_arr.size(); i++){
		const ip_netaddr_t &	ip_netaddr	= ip_netaddr_arr[i];
		// if the ntudp_rdvpt address, is contained in this ip_netaddr_t, reject it
		if( ip_netaddr.contain( ntudp_rdvpt.get_addr().ipaddr() ) )	return true;
	}
	
	// this is point is reached, accept it
	return false;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      handle the peer record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief publish the peer record 
 */
void	router_peer_t::publish_peer_record()						throw()
{
	// log to debug
	KLOG_DBG("enter local peerid=" << lident().get_peerid().to_canonical_string() );
	// if the peer record was already published, delete the kad_publish
	nipmem_zdelete	kad_publish;

	// build the kad_rec_t out of it
	bytearray_t	payload	= bytearray_t().serialize( router_resp->get_listen_addr() );
	kad_keyid_t	keyid	= lident().peerid().to_canonical_string();
	kad_recid_t	recid	= kad_recid_t::build_random();
	kad_rec_t	kad_rec = kad_rec_t(recid, keyid, profile.peer_record_ttl(), payload.to_datum());

	// publish the just-built kad_rec_t
	kad_err_t	kad_err;
	kad_publish	= nipmem_new kad_publish_t();
	kad_err		= kad_publish->start(get_kad_peer(), kad_rec, this, NULL);
	if( kad_err.failed() )
		KLOG_ERR("Peer record publication failed due to " << kad_err);
}

/** \brief callback notified when a kad_publish_t has an event to notify
 */
bool router_peer_t::neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
						, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the kad_even_t MUST be publish_ok()
	DBG_ASSERT( kad_event.is_publish_ok() );

	// NOTE: this function is only a place-holder for the kad_publish of the peer record

	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     netif_vdev_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when netif_vdev_t has an event to report
 * 
 * - TODO this function is ugly - clean it up
 */
bool router_peer_t::neoip_netif_vdev_cb(void *cb_userptr, netif_vdev_t &cb_netif_vdev
					, uint16_t ethertype, pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_ERR("recevied ethertype=0x" << std::hex << ethertype << std::dec << " pkt=" << pkt );
	
	// if the ethertype is NOT ip4, discard the packet
	if( ethertype != netif_vdev_t::TYPE_IP4 )	return true;

	// try to find the destination ip address in the address cache
	const router_acache_item_t *	acache_item;
	acache_item	= acache().find_by_remote_iaddr( ippkt_util_t::get_dst_addr(pkt) );
	// if the destination ip address is found in acache, launch a router_itor_t on this remote_peerid
	if( acache_item ){
		const router_name_t &	remote_dnsname	= acache_item->remote_dnsname();
		router_peerid_t		remote_peerid	= dnsname2peerid(remote_dnsname);
		router_itor_t *		router_itor;
		router_err_t		router_err;
		// sanity check - remote_peerid MUST have been found
		// - in fact i dunno what to do here, if this case is possible
		// - so i do DBG_ASSERT()
		DBG_ASSERT( !remote_peerid.is_null() );
		// if a router_itor_t is already running on this remote_peerid, queue the packet
		router_itor	= itor_by_remote_peerid(remote_peerid);
		if( router_itor ){
			// queue the tiggering packet
			router_itor->queue_inner_pkt(ethertype, pkt);
			// return tokeep
			return true;	
		}

		// if the remote_peerid is in the itor_negcache, discard the packet without replying icmp
		// - TODO this is for the mobility part 
		//   - dont report a itor faillure immdiatly 
		//   - leave it some time to let the new connection to be established
		// - the algo is 'take the last time an established connection with this destination
		//   existed. if it is more than Xsec. then reply icmp"
		// - i think it is possible by storing the last_seen_date in the router_acache 
		if( itor_negcache.contain(remote_peerid) ){
			// build the icmp packet to reply
			pkt_t	icmp_pkt= ippkt_util_t::build_icmp4_pkt(ippkt_util_t::get_dst_addr(pkt)
					, ippkt_util_t::get_src_addr(pkt)
					, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, 0, pkt.to_datum());
			// send the icmp packet
			catchall_netif->send_pkt(netif_vdev_t::TYPE_IP4, icmp_pkt);
			return true;
		}

		// launch a new router_itor_t based on this remote_peerid
		router_itor	= nipmem_new router_itor_t(this);
		router_err	= router_itor->start(remote_peerid, remote_dnsname);
		// if the router_itor_t start() succeed, return tokeep now
		if( router_err.succeed() ){
			// queue the tiggering packet
			router_itor->queue_inner_pkt(ethertype, pkt);
			// return tokeep
			return true;
		}
		// if the router_itor_t start() failed, delete it and fall thru to reply an icmp
		nipmem_zdelete router_itor;
	}

	// build the icmp packet to reply
	pkt_t	icmp_pkt= ippkt_util_t::build_icmp4_pkt(ippkt_util_t::get_dst_addr(pkt)
					, ippkt_util_t::get_src_addr(pkt)
					, ICMP_DEST_UNREACH, ICMP_NET_UNREACH, 0, pkt.to_datum());
	// send the icmp packet
	catchall_netif->send_pkt(netif_vdev_t::TYPE_IP4, icmp_pkt);

	// return 'tokeep'
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            ident database management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if fully-qualified router_name_t may be handled by dnsgrab, false otherwise
 * 
 * - to be dnsgrab_ok, a dnsname MUST be authorized to connect to this peer
 * - to be dnsgrab_ok, a dnsname MUST NOT be a nonesigned
 *   - nonesigned peer can only itor connection, never resp to it
 *   - and dnsgrab is done to itor connection *toward* this dnsname
 *   - aka the opposite/wrong direction  
 */
bool	router_peer_t::dnsname_is_dnsgrab_ok(const router_name_t &remote_name)	const throw()
{
	// sanity check - remote_name MUST be fully_qualified
	DBG_ASSERT( remote_name.is_fully_qualified() );
KLOG_ERR("sloti");
	// if this router_name is for lident peername, return true
	if( lident().dnsfqname(profile) == remote_name )	return true;

	// if this router_name_t is not allowed by the acl, return false
	if( acl().reject(remote_name.to_string()) )		return false;

KLOG_ERR("sloti");
	// if the remote_name matches a router_rootca_t, return true
	// - this means it may be a valid authsigned peer
	if( rootca_arr().find_by_dnsname(remote_name) )		return true;

KLOG_ERR("sloti remote_name=" << remote_name << " selfsigned_domain=" << profile.selfsigned_domain_str());
KLOG_ERR("slota rident_arr=" << rident_arr());
	// if remote_name has a selfsigned_domain and has a rident, return true
	if( remote_name.domain() == profile.selfsigned_domain_str() ){
KLOG_ERR("sloti");
		// if no router_rident_t matches this remote_name, return false
		if( !rident_arr().find_by_name(remote_name, profile))	return false; 
KLOG_ERR("sloti");
		// else return true
		return true;
	}
KLOG_ERR("sloti");

	// if all previous tests passed, the dnsname is NOT handled, so return false
	return false;
}

/** \brief Return the router_peerid_t of for this router_name_t - or null if an error occurs
 * 
 * - NOTE: it doesnt perform any check on the authorization of this certname
 *   this is a simple and direct conversion
 */
router_peerid_t router_peer_t::dnsname2peerid(const router_name_t &dnsname)	const throw()
{
	// sanity check - dnsname MUST be fully_qualified
	DBG_ASSERT( dnsname.is_fully_qualified() );

#if 0	// TODO to remove - obsolete stuff from the pre-nonesigned era

	// if the dnsname IS NOT in the selfsigned_domain, handle it as authsigned 	
	if( !profile.selfsigned_domain().match(dnsname) ){
		// sanity check - the router_name_t MUST be handled by the router_rootca_arr_t 
		DBG_ASSERT( rootca_arr().find_by_dnsname(dnsname) );
		// return a router_peerid_t directly derived from the router_name_t
		return router_peerid_t(dnsname.to_string());
	}
	
	// NOTE: at this point, the router_name_t is for a selfsigned identity
	DBG_ASSERT( profile.selfsigned_domain().match(dnsname) );
	// go thru the whole router_rident_arr_t to find one which matched the hostname
	for(size_t i = 0; i < rident_arr().size(); i++){
		const router_rident_t &	rident	= rident_arr()[i];
		// if the hostname matches this rident, return its router_peerid_t
		if( rident.dnsfqname(profile) == dnsname )	return rident.peerid();
	}
#else
	// if dnsname has a selfsigned_domain and has a rident, return the rident peerid
	if( dnsname.domain() == profile.selfsigned_domain_str() ){
		const router_rident_t *	router_rident	= rident_arr().find_by_name(dnsname, profile);
		// if no router_rident_t matches this remote_name, return a null router_peerid_T
		if( !router_rident )	return router_peerid_t();
		// else return the matching router_rident_t peerid
		return router_rident->peerid(); 
	}

	// test for authsigned_ok
	if( dnsname.is_authsigned_ok() ){
		// return a router_peerid_t directly derived from the router_name_t
		return router_peerid_t(dnsname.to_string());
	}
	// test for nonesigned_ok
	if( dnsname.is_nonesigned_ok() ){
		// return a router_peerid_t directly derived from the router_name_t host()
		return router_peerid_t::from_canonical_string(dnsname.host());
	}
#endif

	// if this point is reached, return a null router_peerid_t
	return router_peerid_t();
}


/** \brief Return router_peerid_t for the certificate subject name
 * 
 * - NOTE: it doesnt perform any check on the authorization of this certname
 *   this is a simple and direct conversion
 */
router_peerid_t	router_peer_t::certname2peerid(const std::string &certname)	throw()
{
	router_peerid_t		peerid;
#if 0	// TODO to remove - obsolete stuff from the pre-nonesigned era
	// determine if the x509_cert_t is from a selfsigned identity or a authsigned one
	bool	is_authsigned	= router_peerid_t::from_canonical_string( certname ).is_null();

	// compute the router_peerid_t depending on the type of certname authsigned or selfsigned
	if( is_authsigned )	peerid	= router_name_t(certname).to_string();
	else			peerid	= router_peerid_t::from_canonical_string( certname );
#else
	// compute the peerid depending on orward to the proper typesigned callback depending on the subject_name
	if( !router_peerid_t::from_canonical_string(certname).is_null() ){
		// a selfsigned has a canonical peerid as certname
		peerid	= router_peerid_t::from_canonical_string( certname );
	}else if( router_name_t(certname).is_authsigned_ok() ){
		// a authsigned has peerid derived from its router_name_t and
		// the certname is its fully-qualified router_name_t
		peerid	= router_name_t(certname).to_string();
	}else if( router_name_t(certname).is_nonesigned_ok() ){
		// a nonesigned has a certname of #{canonical_peerid}.nonesigned_domain
		peerid	= router_peerid_t::from_canonical_string(router_name_t(certname).host());
	}else {
		DBG_ASSERT( 0 );
	}
	// sanity check - the result MUST NOT be null
	DBG_ASSERT( !peerid.is_null() );
#endif
	// return the result
	return peerid;
}

/** \brief Return the router_name_t for this certificate subject name
 * 
 * - NOTE: it doesnt perform any check on the authorization of this certname
 *   this is a simple and direct conversion
 */
router_name_t	router_peer_t::certname2dnsname(const std::string &certname)		const throw()
{
#if 0	// TODO to remove - obsolete stuff from the pre-nonesigned era
	// determine if the x509_cert_t is from a selfsigned identity or a authsigned one
	bool	is_authsigned	= router_peerid_t::from_canonical_string( certname ).is_null();
	
	// if the certname is_authsigned, convert it to a router_name_t and return it
	if( is_authsigned )	return router_name_t(certname);		

	// NOTE: at this point, the certname is from a selfsigned identity

	// convert the cert.subject_name() into a router_peerid_t
	router_peerid_t	remote_peerid	= router_peerid_t::from_canonical_string( certname );
	// try to find a router_rident_t for this remote_peerid
	const router_rident_t * rident	= rident_arr().find_by_peerid(remote_peerid);
	// if router_rident_t have been found, return its fully_qualified router_name_t 
	if( rident )		return rident->dnsfqname(profile);
#else
	// compute the peerid depending on orward to the proper typesigned callback depending on the subject_name
	if( !router_peerid_t::from_canonical_string(certname).is_null() ){
		// convert the cert.subject_name() into a router_peerid_t
		router_peerid_t	remote_peerid	= router_peerid_t::from_canonical_string( certname );
		// try to find a router_rident_t for this remote_peerid
		const router_rident_t * rident	= rident_arr().find_by_peerid(remote_peerid);
		// if router_rident_t have NOT been found, return null router_name 
		if( !rident )		return router_name_t();
		// return its fully_qualified router_name_t 
		return rident->dnsfqname(profile);
	}else if( router_name_t(certname).is_authsigned_ok() ){
		// a authsigned identity as its fully-qualified router_name_t as certname
		return router_name_t(certname);
	}else if( router_name_t(certname).is_nonesigned_ok() ){
		// a nonesigned identity as its fully-qualified router_name_t as certname
		return router_name_t(certname);
	}else {
		DBG_ASSERT( 0 );
	}
#endif
	// else return a null router_name_t
	return router_name_t();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      socket certificate authentication
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called to authenticate a x509_cert_t received by the scnx layer
 * 
 * @return scnx_err_t::OK if the certificate is allowed, any other value mean denied
 */
scnx_err_t	router_peer_t::scnx_auth_ftor_cb(void *userptr, const x509_cert_t &cert) throw()
{
	const std::string &	subject_name	= cert.subject_name(); 
	scnx_err_t		scnx_err;
	// log to debug
	KLOG_ERR("enter remote_cert=" << cert);

	// forward to the proper typesigned callback depending on the subject_name
	if( !router_peerid_t::from_canonical_string(subject_name).is_null() ){
		scnx_err = scnx_auth_ftor_selfsigned_cb(userptr, cert);
	}else if( router_name_t(subject_name).is_authsigned_ok() ){
		scnx_err = scnx_auth_ftor_authsigned_cb(userptr, cert);
	}else if( router_name_t(subject_name).is_nonesigned_ok() ){
		scnx_err = scnx_auth_ftor_nonesigned_cb(userptr, cert);
	}else {
		scnx_err = scnx_err_t(scnx_err_t::ACL_REFUSED, "Invalid certificate subject_name");
	}

	// log to debug
	if( scnx_err.failed() )	KLOG_ERR("cert " << cert << " refused due to " << scnx_err);
	// return the result
	return scnx_err;
}

/** \brief called to authenticate a x509_cert_t received by the scnx layer
 * 
 * @return scnx_err_t::OK if the certificate is allowed, any other value mean denied
 */
scnx_err_t	router_peer_t::scnx_auth_ftor_selfsigned_cb(void *userptr, const x509_cert_t &cert) throw()
{
	const std::string &	subject_name	= cert.subject_name(); 
	// log to debug
	KLOG_DBG("enter remote_cert=" << cert);
	// sanity check - the x509_cert_t MUST be one of a selfsigned router_lident_t
	DBG_ASSERT( !router_peerid_t::from_canonical_string( subject_name ).is_null() );

	// convert the cert.subject_name() into a router_peerid_t
	router_peerid_t	remote_peerid	= router_peerid_t::from_canonical_string(subject_name);
	// try to find a router_rident_t for this remote_peerid
	const router_rident_t * rident	= rident_arr().find_by_peerid(remote_peerid);
	// if the router_rident_t doesnt exists, the certificated is refused 
	if( !rident )			return scnx_err_t::ACL_REFUSED;
	// if the router_rident_t certificate IS NOT equal, the certificate is refused
	if( rident->cert() != cert )	return scnx_err_t::ACL_REFUSED;
	
	// If this is a authentication for router_itor_t, ensure the cert is the expected one
	// - if the userptr is non-null, this mean this is an authentication for router_itor_t
	// - router_resp_t leave it NULL
	// - this check prevent the following attack
	//   1. alice and bob are both allowed to connect the local peer
	//   2. the local peer initiate a connection toward alice
	//   3. bob intercept the connection and replies its own certificate
	//   4. the local peer accepts it without noticing it is not alice one
	if( userptr ){
		router_itor_t *	router_itor	= itor_by_remote_peerid(remote_peerid);
		if( userptr != router_itor )	return scnx_err_t::ACL_REFUSED;	
	}
	
	// get the remote_dnsname from the router_rident_t
	router_name_t	remote_dnsname	= rident->dnsfqname(profile);
	// check if the router_acl_t allows this remote_dnsname
	if( acl().reject(remote_dnsname.to_string()) )	return scnx_err_t::ACL_REFUSED;
	
	// else the certificated is allowed
	return scnx_err_t::OK;
}

/** \brief called to authenticate a x509_cert_t received by the scnx layer
 * 
 * @return scnx_err_t::OK if the certificate is allowed, any other value mean denied
 */
scnx_err_t	router_peer_t::scnx_auth_ftor_authsigned_cb(void *userptr, const x509_cert_t &cert) throw()
{
	const std::string &	subject_name	= cert.subject_name(); 
	// log to debug
	KLOG_ERR("enter remote_cert=" << cert);

	// NOTE: at this point, it is a authsigned certificate
	DBG_ASSERT( router_name_t(subject_name).is_authsigned_ok() );

	// convert the x509_cert_t subject_name into a router_name_t
	router_name_t	remote_dnsname	= router_name_t(subject_name);
	// try to find a router_rootca_t for the dnsname
KLOG_ERR("SLOTA " << remote_dnsname);
KLOG_ERR("SLOTA " << rootca_arr());
	const router_rootca_t *	rootca	= rootca_arr().find_by_dnsname(remote_dnsname);
	// if the router_rootca_t doesnt exists, the certificated is refused 
	if( !rootca )					return scnx_err_t::ACL_REFUSED;
KLOG_ERR("SLOTA");
	// if the router_rootca_t certificate DOES NOT verify the user_cert, the certificate is refused
	if( rootca->cert().verify_cert(cert).failed() )	return scnx_err_t::ACL_REFUSED;
KLOG_ERR("SLOTA");

	// If this is a authentication for router_itor_t, ensure the cert is the expected one
	// - if the userptr is non-null, this mean this is an authentication for router_itor_t
	// - router_resp_t leave it NULL
	// - this check prevent the following attack
	//   1. alice and bob are both allowed to connect the local peer
	//   2. the local peer initiate a connection toward alice
	//   3. bob intercept the connection and replies its own certificate
	//   4. the local peer accepts it without noticing it is not alice one
	if( userptr ){
		router_peerid_t	remote_peerid	= remote_dnsname.to_string();
		router_itor_t *	router_itor	= itor_by_remote_peerid(remote_peerid);
		if( userptr != router_itor )	return scnx_err_t::ACL_REFUSED;	
	}
KLOG_ERR("SLOTA");

	// check if the router_acl_t allows this remote_dnsname
	if( acl().reject(remote_dnsname.to_string()) )	return scnx_err_t::ACL_REFUSED;

KLOG_ERR("SLOTA");
	// if this point is reached, the certificate is allowed
	return scnx_err_t::OK;
}

/** \brief called to authenticate a x509_cert_t received by the scnx layer
 * 
 * @return scnx_err_t::OK if the certificate is allowed, any other value mean denied
 */
scnx_err_t	router_peer_t::scnx_auth_ftor_nonesigned_cb(void *userptr, const x509_cert_t &cert) throw()
{
	// log to debug
	KLOG_ERR("enter remote_cert=" << cert);

	// NOTE: at this point, it is a authsigned certificate
	DBG_ASSERT( router_name_t(cert.subject_name()).is_nonesigned_ok() );

	// convert the x509_cert_t subject_name into a router_name_t
	router_name_t	remote_dnsname	= router_name_t(cert.subject_name());

	// NOTE: explicitly no check with the rootca_arr as it is a nonesigned

	// If this is a authentication for router_itor_t, ensure the cert is the expected one
	// - if the userptr is non-null, this mean this is an authentication for router_itor_t
	// - router_resp_t leave it NULL
	// - this check prevent the following attack
	//   1. alice and bob are both allowed to connect the local peer
	//   2. the local peer initiate a connection toward alice
	//   3. bob intercept the connection and replies its own certificate
	//   4. the local peer accepts it without noticing it is not alice one
	if( userptr ){
		router_peerid_t	remote_peerid	= router_peerid_t::from_canonical_string(remote_dnsname.host());		
		router_itor_t *	router_itor	= itor_by_remote_peerid(remote_peerid);
		if( userptr != router_itor )	return scnx_err_t::ACL_REFUSED;	
	}

	// check if the router_acl_t allows this remote_dnsname
	if( acl().reject(remote_dnsname.to_string()) )	return scnx_err_t::ACL_REFUSED;

	// if this point is reached, the certificate is allowed
	return scnx_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the router_itor_t which matches this remote_peerid, or NULL if none does
 */
router_itor_t *	router_peer_t::itor_by_remote_peerid(const router_peerid_t &remote_peerid)	throw()
{
	std::list<router_itor_t *>::iterator	iter;
	// go thru the whole database
	for(iter = itor_db.begin(); iter != itor_db.end(); iter++ ){
		router_itor_t *	router_itor	= *iter;
		// if this element matches, return it
		if(router_itor->remote_peerid() == remote_peerid)	return router_itor;
	}
	// if none have been found, return NULL
	return NULL;
}
/** \brief Return the router_full_t which matches this local_iaddr, or NULL if none does
 */
router_full_t *	router_peer_t::full_by_local_iaddr(const ip_addr_t &local_iaddr)	const throw()
{
	std::list<router_full_t *>::const_iterator	iter;
	// go thru the whole database
	for(iter = full_db.begin(); iter != full_db.end(); iter++ ){
		router_full_t *	router_full	= *iter;
		// if this element matches, return it
		if( router_full->local_iaddr() == local_iaddr )		return router_full;
	}
	// if none have been found, return NULL
	return NULL;
}

/** \brief Return the router_full_t which matches this remote_iaddr, or NULL if none does
 */
router_full_t *	router_peer_t::full_by_remote_iaddr(const ip_addr_t &remote_iaddr)	const throw()
{
	std::list<router_full_t *>::const_iterator	iter;
	// go thru the whole database
	for(iter = full_db.begin(); iter != full_db.end(); iter++ ){
		router_full_t *	router_full	= *iter;
		// if this element matches, return it
		if(router_full->remote_iaddr() == remote_iaddr)		return router_full;
	}
	// if none have been found, return NULL
	return NULL;
}

/** \brief Return the router_full_t which matches this remote_peerid, or NULL if none does
 */
router_full_t *	router_peer_t::full_by_remote_peerid(const router_peerid_t &remote_peerid)	const throw()
{
	std::list<router_full_t *>::const_iterator	iter;
	// go thru the whole database
	for(iter = full_db.begin(); iter != full_db.end(); iter++ ){
		router_full_t *	router_full	= *iter;
		// if this element matches, return it
		if(router_full->remote_peerid() == remote_peerid)	return router_full;
	}
	// if none have been found, return NULL
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     dsngrab_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief dnsgrab callback to received request	
 */
bool 	router_peer_t::neoip_dnsgrab_cb(void *cb_userptr, dnsgrab_t &cb_dnsgrab
						, dnsgrab_request_t &request)	throw()
{
	// log to debug
	KLOG_DBG("router peername " << lident() << " received a request of " << request.get_request_name() 
			<< " From " << request.get_addr_family()
			<< " by " << (request.is_request_by_name() ? "name" : "address"));

	// if the address family is not AF_INET, return notfound
	if( request.get_addr_family() != "AF_INET" ){
		set_dnsreq_reply_notfound(request);
		return true;	
	}

	// if it is a request by address, forward it to the proper sub callback
	if( request.is_request_by_addr() )	return dnsgrab_byaddr_cb(request);
	// else forward it to the subcallback dedicated to the request by name
	return dnsgrab_byname_cb(request);
}

/** \brief dnsgrab callback to received request	by address
 * 
 * @return a tokeep for the dnsgrab_t
 */
bool 	router_peer_t::dnsgrab_byaddr_cb(dnsgrab_request_t &request)	throw()
{
	ip_addr_t	request_addr	= request.get_request_name().c_str();
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the dnsgrab_request_t MUST be AF_INET
	DBG_ASSERT( request.get_addr_family() == "AF_INET" );
	// sanity check - the dnsgrab_request_t MUST be by address
	DBG_ASSERT( request.is_request_by_addr() );

	// if the request_addr matches the catchall_iaddr, reply lident
	if( request_addr == catchall_iaddr ){
		set_dnsreq_reply_lident(request);
		return true;
	}

	// if the request_addr matches any local_iaddr of the router_full_t, reply ident
	if( full_by_local_iaddr(request_addr) ){
		set_dnsreq_reply_lident(request);
		return true;
	}

	// if the request_addr matches any remote_iaddr of the router_full_t, reply rident
	router_full_t *	router_full	= full_by_remote_iaddr(request_addr);
	if( router_full ){
		set_dnsreq_reply_cnxfull(request, router_full);
		return true;
	}
	
	// TODO should i query the router_acache here ??
	// - i dont see why not
	// - it seems logical as it is the same at 
	// - think about it and see
	// - and if it is in the router_acache_t should i launch a itor ?
	//   - thus have a processing similar to dnsgrab_by_name
	
	// if this point is reached, reply notfound to the request by addr
	set_dnsreq_reply_notfound(request);
	return true;		
}

/** \brief dnsgrab callback to received request	by name
 * 
 * @return a tokeep for the dnsgrab_t
 */
bool 	router_peer_t::dnsgrab_byname_cb(dnsgrab_request_t &request)	throw()
{
	router_name_t	request_dnsname	= router_name_t(request.get_request_name());	
	// log to debug
	KLOG_ERR("enter request_name=" << request.get_request_name());

	// sanity check - here the dnsgrab_request MUST be AF_INET
	DBG_ASSERT( request.get_addr_family() == "AF_INET" );
	// sanity check - here the dnsgrab_request MUST be by_name()
	DBG_ASSERT( request.is_request_by_name() );
KLOG_ERR("slota");
	// if request_dnsname is not is_fully_qualified, notify a not_found
	// - NOTE: in theory, it should not happen but "be tolerant with what you receive" 
	if( !request_dnsname.is_fully_qualified() ){
		set_dnsreq_reply_notfound(request);
		return true;
	}
KLOG_ERR("slota");
	
	// if the dnsgrab_request_t is NOT one for this router_peer_t, notify a not_found
	if( !dnsname_is_dnsgrab_ok(request_dnsname) ){
		set_dnsreq_reply_notfound(request);
		return true;
	}
KLOG_ERR("slota lident().dnsfqname(profile)=" << lident().dnsfqname(profile) << " request_dnsname=" << request_dnsname);
	
	// if the dnsgrab_request_t is for the lident peername, reply the lident
	if( lident().dnsfqname(profile) == request_dnsname ){
		set_dnsreq_reply_lident(request);
		return true;
	}
KLOG_ERR("slota");
	
	// NOTE: here the request name MAY NOT be a router_lident_t one
	
	// if router_acl_t reject it, reply a notfound
	if( acl().reject(request_dnsname.to_string()) ){
		set_dnsreq_reply_notfound(request);
		return true;
	}
	
	// try to get a remote_peerid matching the request name
	router_peerid_t	remote_peerid	= dnsname2peerid(request_dnsname);
	// if no remote_peerid matches, reply notfound
	if( remote_peerid.is_null() ){
		set_dnsreq_reply_notfound(request);
		return true;
	}

	// if there are already a router_full_t for this remote_peerid, reply its matching rident
	router_full_t *	router_full	= full_by_remote_peerid(remote_peerid);
	if( router_full ){
		set_dnsreq_reply_cnxfull(request, router_full);
		return true;
	}

	// if an router_itor_t is already running for the requested hostname, add this request to it
	router_itor_t *	router_itor;
	router_itor	= itor_by_remote_peerid(remote_peerid);
	if( router_itor ){
		router_itor->queue_dnsgrab_request(request);
		return true;	
	}

	// if the remote_peerid is in the itor_negcache, reply notfound
	if( itor_negcache.contain(remote_peerid) ){
		set_dnsreq_reply_notfound(request);
		return true;
	}

	// launch a new router_itor_t based on this dnsgrab_request_t
	router_err_t	router_err;
	router_itor	= nipmem_new router_itor_t(this);
	router_err	= router_itor->queue_dnsgrab_request(request).start(remote_peerid, request_dnsname);
	// if the router_itor start failed, delete the router_itor_t and reply notfound
	if( router_err.failed() ){
		nipmem_delete router_itor;
		set_dnsreq_reply_notfound(request);
		return true;
	}
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reply building for dnsgrab_request_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief fill the dnsgrab_request_t with the router_lident_t
 */
void	router_peer_t::set_dnsreq_reply_lident(dnsgrab_request_t &request)		const throw()
{
	// mark the request as replied
	request.get_reply_present()	= true;

	// put the reply_name
	request.get_reply_name()	= lident().dnsfqname(profile).to_string();

	// put the catchall_iaddr in the reply address	
	request.get_reply_addresses().push_back(catchall_iaddr);	

	// put all the local_iaddr of the router_full_t in the reply address
	std::list<router_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const router_full_t *	router_full	= *iter;
		// put this router_full_t local address
		request.get_reply_addresses().push_back(router_full->local_iaddr());	
	}
}

/** \brief fill the dnsgrab_request_t with the router_full_t
 */
void	router_peer_t::set_dnsreq_reply_cnxfull(dnsgrab_request_t &request
					, const router_full_t *router_full)	const throw()
{
	// log to debug
	KLOG_DBG("enter");
	// mark the request as replied
	request.get_reply_present()	= true;

	// put the reply_name
	request.get_reply_name()	= router_full->remote_dnsname().to_string();

	// put remote_iaddr of the router_full_t in the reply address
	request.get_reply_addresses().push_back(router_full->remote_iaddr());
}

/** \brief fill the dnsgrab_request_t as notfound
 */
void	router_peer_t::set_dnsreq_reply_notfound(dnsgrab_request_t &request)		const throw()
{
	// mark the request as replied
	request.get_reply_present()	= true;
	// put the reply_name as empty string - magic to say notfound
	request.get_reply_name()	= std::string();
}



NEOIP_NAMESPACE_END

