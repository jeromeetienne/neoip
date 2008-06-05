/*! \file
    \brief Definition of the ntudp_pserver_tunnel_t

\par about multiple tunnel_t for the same ntudp_peerid_t
- When a REGISTER_REQUEST for a given ntudp_peerid_t is received, it is accepted
  IIF no other tunnel_t claims the same ntudp_peerid_t, else it is discarded
- thus a 'attacker' inpersonating a ntudp_peerid_t for another ntudp_peerid_t
  will accepted only if the legitimate one is not registered on this ntudp_pserver_t
  - note that this attacker can still register before the legitimate user and thus
    preventing it from registering
  - hopefully there will be enought ntudp_pserver_t to find one where the ntudp_peerid_t
    is not registered
- as a side effect, a legitimate user unregistering non gracefully and then coming 
  back would have to wait for the previous registration to timeout to register 
  on this ntudp_pserver_t or to find another one.
  - note that this problem occurs IIF the legitimate user has the same ntudp_peerid_t for 
    both attempts.
  
*/

/* system include */
/* local include */
#include "neoip_ntudp_pserver_tunnel.hpp"
#include "neoip_ntudp_pserver_tunnel_wikidbg.hpp"
#include "neoip_ntudp_pserver_extcnx.hpp"
#include "neoip_ntudp_tunl_pkttype.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_udp_layer.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_pserver_tunnel_t::ntudp_pserver_tunnel_t(ntudp_pserver_t *ntudp_pserver, udp_full_t *udp_full)	throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_ERR("enter");
	// copy parameter
	this->ntudp_pserver	= ntudp_pserver;
	this->udp_full		= udp_full;
	// set the callback for the udp_full
	inet_err = udp_full->set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
// TODO here there is an issue due to udp_vresp_t
// - udp_vresp_t do a start() on the udp_full to get the first packet
// - and here i do another start on it ...
// - hence the udp_full is started twice and the fails
//	inet_err = udp_full->start();
//	DBG_ASSERT( inet_err.succeed() );

	// start the idle_timeout
	idle_timeout.start(ntudp_pserver->profile.tunnel_idle_timeout(), this, NULL);

	// link the connection to the list
	ntudp_pserver->tunnel_link( this );
}

/** \brief destructor
 */
ntudp_pserver_tunnel_t::~ntudp_pserver_tunnel_t()				throw()
{
	// log to debug
	KLOG_ERR("enter");
	// if the connection has been established, notify the other peer that it is now closed
	if( is_established() ){
		// send NB_CLOSURE_PKT packet of REQUEST_REPLY with false accepted_f hoping the client
		// will receives one.
		for( size_t i = 0; i < ntudp_pserver->profile.tunnel_nb_closure_pkt(); i++ ){
			// build the REGISTER_REPLY with false accepted_f
			pkt_t	pkt = build_register_reply(client_nonce, false);
			udp_full->send(pkt);
		}
	}
	// close the udp_full_t connection
	DBG_ASSERT( udp_full );
	nipmem_delete	udp_full;
	// unlink the connection to the list
	ntudp_pserver->tunnel_unlink( this );	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     idle_timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 * 
 * - this is triggered when no REGISTER_REQUEST have been received for IDLE_TIMEOUT_DELAY
 */
bool ntudp_pserver_tunnel_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("idle_timeout expire");
	// autodelete
	nipmem_delete this;
	// return a 'dontkeep'
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    utility function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Send a PKT_SRV2CLI thru this tunnel
 * 
 * - the caller is ntudp_pserver_extcnx_t which received a packet
 */
void	ntudp_pserver_tunnel_t::send_pkt_srv2cli(const slot_id_t &extcnx_slotid
						, const datum_t &datum_tofwd)	throw()
{
	// build the outter pkt for the inner pkt
	pkt_t	outter_pkt	= build_pkt_srv2cli(extcnx_slotid, datum_tofwd);
	// send the reply thru the connection
	udp_full->send(outter_pkt);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	ntudp_pserver_tunnel_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:	// autodelete of the tunnel_t
					nipmem_delete	this;
					return false;
	case udp_event_t::RECVED_DATA:	// handle the received packet
					return handle_recved_data(*udp_event.get_recved_data());
	case udp_event_t::MTU_CHANGE:	// this event MUST NOT happend as the tunnel has no pmtudisc
					DBG_ASSERT( 0 );
	default:	DBG_ASSERT( 0 );
	}
	// return a 'tokeep'
	return true;
}

/** \brief Handle received data on the tunnel_t
 * 
 * - NOTE: this may delete the ntudp_pserver_t or the udp_full_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_pserver_tunnel_t::handle_recved_data(pkt_t &pkt)		throw()
{
	ntudp_tunl_pkttype_t	pkttype;
	// log to debug
	KLOG_ERR("enter");

	try {	// read the pkttype
		pkt >> pkttype;
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_tunl_pkttype_t::REGISTER_REQUEST:	return recv_register_request(pkt);
		case ntudp_tunl_pkttype_t::PKT_CLI2SRV:		return recv_pkt_cli2srv(pkt);
		default:	KLOG_ERR("Unexpected packet type " << pkttype );
				// simply discard the packet
				return true;
		}
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
		// simply discard the packet
		return true;
	}

	// return 'tokeep'
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle the reception of a REGISTER_REQUEST
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_pserver_tunnel_t::recv_register_request(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_nonce_t	incoming_nonce;
	ntudp_peerid_t	incoming_peerid;
	// log to debug
	KLOG_ERR("enter");
	// read the incoming nounce
	pkt >> incoming_nonce;
	// read the peerid to register
	pkt >> incoming_peerid;
	
	// if this REGISTER_REQUEST is the first one, copy the incoming nonce to the client_nonce
	if( client_peerid.is_null() ){
		// if the peerid is already registered on this ntudp_pserver_t, reply a not-accepting
		// REGISTER_REPLY and autodelete this connection
		if( ntudp_pserver->get_tunnel_from_peerid(incoming_peerid) ){
			// build the not-accepting REGISTER_REPLY 
			pkt = build_register_reply(incoming_nonce, false);
			// send the reply thru the connection
			udp_full->send(pkt);
			// autodelete
			nipmem_delete	this;
			// return 'dontkeep'
			return false;
		}
		// log to debug
		KLOG_ERR("register new client with peerid=" << incoming_peerid);
		// init client_peerid with the incoming_peerid
		client_peerid	= incoming_peerid;
		// init the client_nonce - to use it in case of tunnel_t deletion (see dtor)
		client_nonce	= incoming_nonce;
	}
	
	// if the incoming_nonce is different thand client_nonce, discard the packet
	if( incoming_nonce != client_nonce )	return true;	

	// restart the idle_timeout as a valid REGISTER_REQUEST has been received
	idle_timeout.start(ntudp_pserver->profile.tunnel_idle_timeout(), this, NULL);	
	// build the accepting REGISTER_REPLY
	pkt = build_register_reply(incoming_nonce, true);
	// send the reply thru the connection
	udp_full->send(pkt);
	// return 'tokeep'
	return true;	
}


/** \brief handle the reception of a PKT_CLI2SRV
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_pserver_tunnel_t::recv_pkt_cli2srv(pkt_t &pkt)			throw(serial_except_t)
{
	ntudp_nonce_t	incoming_nonce;
	slot_id_t	extcnx_slotid;
	datum_t		datum_to_fwd;
	// log to debug
	KLOG_ERR("enter");
	// read the incoming_nonce as a 'security measure'
	pkt >> incoming_nonce;
	// read the extcnx_slotid to determine on which ntudp_pserver_extcnx_t forward this packet
	pkt >> extcnx_slotid;
	// read the packet to forward
	pkt >> datum_to_fwd;

	// if the incoming_nonce is different thand client_nonce, discard the packet
	if( incoming_nonce != client_nonce )	return true;

	// try to find the ntudp_pserver_extcnx_t for this slotid
	ntudp_pserver_extcnx_t *pserver_extcnx	= ntudp_pserver->get_extcnx_from_slotid(extcnx_slotid);

	// if none is found, log the event and discard the packet
	if( !pserver_extcnx ){
		KLOG_ERR("Received a "<< ntudp_tunl_pkttype_t::PKT_CLI2SRV <<" with a unexisting slotid");
		return true;
	}

	// pass the packet to the found extcnx - for it to send it
	pkt_t	pkt_to_fwd(datum_to_fwd);
	pserver_extcnx->notify_recved_reply( pkt_to_fwd );
	
	// return 'tokeep'
	return true;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build a CNX_REPLY packet
 */
pkt_t	ntudp_pserver_tunnel_t::build_register_reply(const ntudp_nonce_t &echoed_nonce
							, bool accepted_f)	const throw()
{
	pkt_t	pkt;
	// add the packet type
	pkt << ntudp_tunl_pkttype_t(ntudp_tunl_pkttype_t::REGISTER_REPLY);
	// add the echoed_nonce
	pkt << echoed_nonce;
	// add the accepted_f
	pkt << accepted_f;
	// return the generated packet
	return pkt;
}


/** \brief build the PKT_SRV2CLI
 */
pkt_t	ntudp_pserver_tunnel_t::build_pkt_srv2cli(const slot_id_t &extcnx_slotid
						, const datum_t &datum_tofwd)	const throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << ntudp_tunl_pkttype_t(ntudp_tunl_pkttype_t::PKT_SRV2CLI);
	// add the client_nonce
	pkt << client_nonce;
	// add the external connection slot_id_t
	pkt << extcnx_slotid;
	// add the inner_pkt
	pkt << datum_tofwd;
	// return the packet
	return pkt;
}
NEOIP_NAMESPACE_END



