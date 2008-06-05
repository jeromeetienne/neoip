/*! \file
    \brief Definition of the router_full_t

- TODO check if the pkttype FWDED_PKT is usefull
  - it seems it isnt, so it is wasting network rescource

\par Possible Improvement - idle timeout 
- issue it is already somewhere in the nlay
- should i rely on nlay stuff or more doing local one here?
- a custom local one would be duplication... so would require an actual reason
  - why am i afraid of the nlay stuff ?
  - i think it is because i dont trust it
- to use it will allow me to test it and so either to fix it or to trust it more
  - in anycase this seems way better then current state
- YES use nlay one
  - no good reason to do duplication, in fact it is bad to run away from untrusted code

\par Possible Improvement - inner packet header compression
- there is a lot of work on the packet header compression (see ROhC ietf group)
- it gains traffic,
  - some estimation gives 50% for specific case like VoIP with many small packet
  - i think it is mainly due to the tcp ack in unidirectionnal transfert
  - those tcp ack a 40byte minimum, and linux add options which makes them 52-byte 
- rfc1444 rfc2502
  - apparently i havent found any free library implementing it
  - so to include it would require me to read the rfc and implement it myself
- they could be reduced to 1-byte or so 
- the usual tcp/ip packet going out of linux is 52-byte
  - 20-byte of ip header
  - 32-byte of tcp header (20-byte for the core header + 12-byte for header option)
- currently the router_full_t total overhead is 48-byte
  - 28-byte for the udp/ip
  - XX-byte for ntudp_full_t (is there any ?)
  - XX-byte for the nlay_full_t (some payload + security (4-byte seqnb + 12-byte mac)
    - the seqnb maybe smaller - 3-byte is no trouble - maybe even 2 ...
    - depends on the amount of packet in the air
    - may be negociated during connection establishement
  - 3-byte for the router_full_t (1-byte payload + 2-byte ethertype)
    - as the majority of packet are FWDPKT + IPV4 it could be optimized easily to 1-byte

\par Possible Improvement - packet regrouping/degrouping
- principle: if the users sends many packet in a row thru the netif_vdev_t, and
  those packets are small enought to fit in a single outter packets, regroup
  them in a single outter packet.
- PRO: this reduces the overhead for those packets as it uses a single overhead
  chunk for several inner packets
- PRO: it provides a better packet compression as the compressed chunk are larger
- PRO: it provices a faster cpu processing as processed chunk are larger so
  less cpu overhead.
- implementation:
  - have a eloop_t tool which ask to be called before switching to the next iteration
    - this is required not to delay the packet processing.
  - grouping the packets: implemented in nlay
    1. stack the received packets 
    2. flush the received packets 
       - if the new packet may the regrouped packet larger than the maximum outter packet
       - if the eloop_t prenextiter signal is received
- issue: how to read the data in bunch in the netdev_if_t
  - especially how much data may be read before stopping ?
  - risk: spending its times reading a single netif_vdev_t without reading anything else
    - this can happen in case of high traffic as a netif_vdev_t may have packets
      to read continuously
    - aka a starvation issue for all other socket or netif_vdev_t
  - ultimatly it should read exactly the good number of packet until it triggers
    a transmission of a outter packet.
  - maybe a feedback mechanism, aka router_full_t being notified when a outter
    packet is sent and thus router_full_t not going on to read netif_vdev_t. 
- issue: to trigger the transmission of a outter packet, this means it receives
  the packet which make the outter packet too big to be sent
  - thus it is not possible to send this 'too much' packet in this group
  - what happen to this 'too much' packet ?
    - should it be sent immediatly ? producing a outter packet smaller than required
    - should it be delayed for the next event loop iteration ? breaking the rules
      'dont delay packets' ?
    - why not both ? and choosing on option
*/

/* system include */
/* local include */
#include "neoip_router_full.hpp"
#include "neoip_router_pkttype.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_netif_vdev.hpp"
#include "neoip_socket.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// include to have direct access to the scnx_full_t in the socket
#include "neoip_socket_full_ntlay.hpp"
#include "neoip_nlay_full_upapi.hpp"
#include "neoip_nlay_scnx_full_api.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_full_t::router_full_t(router_peer_t *router_peer)	throw()
{
	// copy the parameter
	this->router_peer	= router_peer;
	// zero some parameter
	socket_full	= NULL;
	netif_vdev	= NULL;
	// link this router_full_t to the router
	router_peer->full_dolink(this);
}

/** \brief Destructor
 */
router_full_t::~router_full_t()	throw()
{
	const router_profile_t &profile	= router_peer->get_profile();
	// log to debug
	KLOG_ERR("enter");
	// populate the router_acache_t - done in the dtor thus the acache_ttl is the time of unused
	router_peer->m_acache.update(remote_dnsname(), local_iaddr(), remote_iaddr(), profile.acache_ttl());	
	// unlink this router_full_t from the router
	router_peer->full_unlink(this);	
	// delete socket_full_t if needed
	nipmem_zdelete	socket_full;
	// delete netif_vdev_t if needed - linux remove the routes by itself
	nipmem_zdelete	netif_vdev;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    Setup Fcuntion
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
router_err_t	router_full_t::start(socket_full_t *socket_full
				, const ip_addr_t &m_local_iaddr, const ip_addr_t &m_remote_iaddr
				, const pkt_t &m_estapkt_in, const pkt_t &m_estapkt_out)	throw()
{
	const router_profile_t &profile	= router_peer->get_profile();	
	// copy some parameter
	this->socket_full	= socket_full;
	this->m_local_iaddr	= m_local_iaddr;
	this->m_remote_iaddr	= m_remote_iaddr;
	this->estapkt_in	= estapkt_in;
	this->estapkt_out	= estapkt_out;

	// get the cerficate subject name from the scnx in the socket_full
	socket_full_ntlay_t &	full_ntlay	= socket_full_ntlay_t::from_socket(*socket_full);
	std::string certname	= full_ntlay.nlay().scnx().get_remote_idname();
	m_remote_peerid		= router_peer->certname2peerid(certname);
	m_remote_dnsname	= router_peer->certname2dnsname(certname);
	// sanity check - the remote_idname MUST be a valid router_peerid_t as it got authorized
	//                thru the router_peer_t::scnx_auth_ftor_cb() function.
	DBG_ASSERT( !remote_peerid().is_null() );
	DBG_ASSERT( !remote_dnsname().is_null() );

	// set the socket_full_t callback
	socket_full->set_callback(this, NULL);
	// enable mtu_pathdisc in socket_full_t
	socket_full->mtu_pathdisc(true);	

	// start the netif_vdev_t - IIF profile.tunnel_stub() IS NOT set
	if( !profile.tunnel_stub() ){
		netif_err_t	netif_err;
		netif_vdev	= nipmem_new netif_vdev_t();
		netif_err	= netif_vdev->start(this, NULL, netif_vdev_t::TYPE_TUN);
		if( netif_err.failed() )	return router_err_from_netif(netif_err);
	
		// set the netif_vdev UP
		netif_err	= netif_vdev->set_updown(netif_util_t::IF_UP);
		if( netif_err.failed() )	return router_err_from_netif(netif_err);

	 	// set the netif_vdev ip addr
		netif_err	= netif_vdev->set_netaddr( ip_netaddr_t(local_iaddr(),32) );
		if( netif_err.failed() )	return router_err_from_netif(netif_err);
	
		// set the route for the remote_iaddr via this netif
		netif_err	= netif_vdev->add_route( ip_netaddr_t(remote_iaddr(), 32) );
		if( netif_err.failed() )	return router_err_from_netif(netif_err);

		// set the netif_mtu to match the socket_full->mtu_inner() and the local mtu_overhead()
		// - thus it is up to the kernel to handle the icmp packet to fragment 
		netif_err	= netif_vdev->set_mtu( socket_full->mtu_inner() - mtu_overhead() );
		if( netif_err.failed() )	return router_err_from_netif(netif_err);		
	}

	// return no error
	return router_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        MTU management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the mtu overhead for router_full_t
 */
size_t	router_full_t::mtu_overhead()	throw()
{
	serial_t	serial;
	// put the packet type
	serial << router_pkttype_t(router_pkttype_t::FWDED_PKT);
	// put the ethertype
	serial << uint16_t(0x4242);
	// return the length
	return serial.get_len();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     netif_vdev_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when netif_vdev_t has an event to report
 */
bool router_full_t::neoip_netif_vdev_cb(void *cb_userptr, netif_vdev_t &cb_netif_vdev
						, uint16_t ethertype, pkt_t &pkt)	throw()
{
	const router_profile_t &profile	= router_peer->get_profile();	
	// log to debug
	KLOG_DBG("recevied ethertype=0x" << std::hex << ethertype << std::dec << " pkt=" << pkt );
	
	// sanity check - the profile.tunnel_stub() MUST NOT be set
	DBG_ASSERT( !profile.tunnel_stub() );	
	
	// forward the packet thru the socket
	forward_pkt(ethertype, pkt);
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       forward packet
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Forward a packet thru the socket_full_t
 */
void	router_full_t::forward_pkt(uint16_t ethertype, pkt_t &pkt)	throw()
{
	pkt_t	pkt_hd;
	// NOTE: no sanity check on the MTU as it is directly set in the netif_vdev_t
	// - so it is either directly the kernel to handle it
	// - or up to the socket_full_t to update the netif_vdev_t MTU if nedded
	
	// put the packet type
	pkt_hd << router_pkttype_t(router_pkttype_t::FWDED_PKT);
	// put the ethertype
	pkt_hd << ethertype;
	// prepend the pkt_hd to the packet
	pkt.prepend( pkt_hd.to_datum(datum_t::NOCOPY) );

	// send it thru the socket_full_t
	socket_full->send( pkt );
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t to provide event
 */
bool	router_full_t::neoip_socket_full_event_cb(void *userptr
			, socket_full_t &cb_socket_full, const socket_event_t &socket_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// if the socket_event_t is fatal, autodelete this router_resp_cnx_t
	if( socket_event.is_fatal() ){
		nipmem_delete	this;
		return false;
	}
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:	handle_socket_recved_data(*socket_event.get_recved_data());
						break;
	case socket_event_t::NEW_MTU:		handle_socket_new_mtu( socket_event.get_new_mtu() );
						break;		
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::RECVED_DATA from the socket_full_t
 */
void	router_full_t::handle_socket_recved_data(pkt_t &pkt)			throw()
{
	const router_profile_t &profile	= router_peer->get_profile();	
	router_pkttype_t	pkttype;
	uint16_t		ethertype;

	// handle the estapkt_in if still present
	if( !estapkt_in.is_null() ){
		// if the incoming packet is equal to the estapkt_in, reply estpkt_out and exit
		if( pkt == estapkt_in ){
			socket_full->send( estapkt_out );
			return;
		}
		// if the received packet is not estapkt_in, it is assumed the remote peer received
		// the estapkt_out, so free both
		estapkt_in	= pkt_t();
		estapkt_out	= pkt_t();
	}
	
	// parse the incoming packet
	try {
		pkt >> pkttype;
		pkt >> ethertype;
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
		return;
	}
	
	// if the pkttype is NOT router_pkttype_t::FWDED_PKT, discard it
	// - TODO sure but what may be received if NOT a FWDED_PKT ?
	//   - if nothing, put at least DBGNET_ASSERT
	DBGNET_ASSERT( pkttype == router_pkttype_t::FWDED_PKT );
	if( pkttype != router_pkttype_t::FWDED_PKT )	return;

	// if profile.tunnel_stub() IS set, exit now and discard the received packet
	if( profile.tunnel_stub() ){
		KLOG_DBG("acting as tunnel stub so dicarding ethertype=0x" << std::hex << ethertype
								<< std::dec << " pkt=" << pkt );
		return;
	}

	// log to debug
	KLOG_DBG("sending to " << netif_vdev->get_name() << " ethertype=0x" << std::hex << ethertype
								<< std::dec << " pkt=" << pkt );
	// send the received packet thru the netif_vdev
	netif_err_t	netif_err;
	netif_err	= netif_vdev->send_pkt(ethertype, pkt);
	// if the netif_vdev send fails, log the event
	if(netif_err.failed())	KLOG_ERR("Could not sent packet thru netif_vdev due to " << netif_err);
}

/** \brief Handle the socket_event_t::NEW_MTU from the socket_full_t
 */
void	router_full_t::handle_socket_new_mtu(size_t new_mtu)			throw()
{
	const router_profile_t &profile	= router_peer->get_profile();	
	// if profile.tunnel_stub() is true, do nothing
	if( profile.tunnel_stub() )		return;
	// if the local_mtu_overhead is larger than the new_mtu, do nothing
	// - this should never happen in practice so log the event 
	if( mtu_overhead() > new_mtu ){
		KLOG_ERR("ignoring new mtu=" << new_mtu << " as local mtu_overhead=" << mtu_overhead());
		return;
	}
	// set the netif_mtu to the netif_vdev
	netif_err_t	netif_err;
	netif_err	= netif_vdev->set_mtu(new_mtu - mtu_overhead());
	if( netif_err.failed() )	KLOG_ERR("Cant set the netif_vdev mtu to " << new_mtu << " due to " << netif_err );
}


NEOIP_NAMESPACE_END

