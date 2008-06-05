/*! \file
    \brief Definition of the ntudp_pserver_extcnx_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_pserver_extcnx.hpp"
#include "neoip_ntudp_pserver_tunnel.hpp"
#include "neoip_ntudp_tunl_pkttype.hpp"
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_nonce.hpp"
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
ntudp_pserver_extcnx_t::ntudp_pserver_extcnx_t(ntudp_pserver_t *ntudp_pserver, udp_full_t *udp_full)	throw()
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
	timeout.start(ntudp_pserver->profile.extcnx_expire(), this, NULL);
	// associate a slot_id_t to this object
	slot_id		=  ntudp_pserver->extcnx_slotpool.allocate(this);
	// link the connection to the list
	ntudp_pserver->extcnx_link( this );
}

/** \brief destructor
 */
ntudp_pserver_extcnx_t::~ntudp_pserver_extcnx_t()				throw()
{
	// log to debug
	KLOG_ERR("enter");
	// unlink the connection to the list
	ntudp_pserver->extcnx_unlink( this );	
	// release the slot_id of this object
	ntudp_pserver->extcnx_slotpool.release(slot_id);
	// close the udp_full_t connection
	nipmem_zdelete	udp_full;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     idle_timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 * 
 * - this is triggered  when no packet are received back from the ntudp_pserver_tunnel_t
 */
bool ntudp_pserver_extcnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
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
//                     ?????????????????
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief For the caller to provide the recved reply packet.
 * 
 * - NOTE: this function delete the ntudp_pserver_extcnx_t
 */
void	ntudp_pserver_extcnx_t::notify_recved_reply(pkt_t &pkt_reply)	throw()
{
	// send the packet thru the connection
	udp_full->send(pkt_reply);
	// autodelete
	nipmem_delete this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	ntudp_pserver_extcnx_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:	// autodelete of the extcnx_t
					nipmem_delete	this;
					return false;
	case udp_event_t::RECVED_DATA:	// handle the received packet
					return handle_recved_data(*udp_event.get_recved_data());
	case udp_event_t::MTU_CHANGE:	// this event MUST NOT happend as this connection has no pmtudisc
					DBG_ASSERT( 0 );
	default:	DBG_ASSERT( 0 );
	}
	// return a 'tokeep'
	return true;
}

/** \brief Handle received data on the extcnx_t
 * 
 * - NOTE: this may delete the ntudp_pserver_t or the udp_full_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_pserver_extcnx_t::handle_recved_data(pkt_t &pkt)		throw()
{
	ntudp_tunl_pkttype_t	pkttype;
	// log to debug
	KLOG_ERR("enter");

	try {	// read the pkttype (without consuming)	
		pkt.unserial_peek( pkttype );
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_tunl_pkttype_t::PKT_EXT2SRV:	return recv_pkt_ext2srv(pkt);	
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

/** \brief handle the reception of a ntudp_tunl_pkttype_t::PKT_EXT2SRV
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_pserver_extcnx_t::recv_pkt_ext2srv(pkt_t &pkt)	throw(serial_except_t)
{
	ntudp_tunl_pkttype_t	pkttype;
	ntudp_nonce_t		incoming_nonce;
	ntudp_peerid_t		dest_peerid;
	datum_t			datum_tofwd;
	// parse the packet
	pkt >> pkttype;
	pkt >> incoming_nonce;
	pkt >> dest_peerid;
	pkt >> datum_tofwd;
	
	/* TODO this perform a full copy of the packet datum_tofwd
	 * - this is unnecessary as the data is already in the pkt_t
	 * - it could be posible to avoid this memcpy.
	 */

	// if the client_nonce is not already set, set it with the incoming_nonce
	if( client_nonce.is_null() )		client_nonce	= incoming_nonce;

	// if the incoming_nonce is different thand client_nonce, log the event and discard the packet
	if( incoming_nonce != client_nonce ){
		KLOG_INFO("Receive a client_nonce different from the expected one, discarding the packet");
		return true;
	}

	// try to find the tunnel matching the destination peerid
	ntudp_pserver_tunnel_t *tunnel = ntudp_pserver->get_tunnel_from_peerid(dest_peerid);
	
	// if no matching tunnel is found, reply an error
	if( !tunnel ){
		// build the packet to reply
		pkt_t	pkt_err = ntudp_sock_errcode_build_pkt(ntudp_sock_errcode_t::UNREACH_PEERID, client_nonce);		
		// send back the packet
		udp_full->send(pkt_err);
		// autodelete
		nipmem_delete	this;
		// return dontkeep
		return false;
	}
	
	// if a tunnel matches the destination peerid, pass it to it
	tunnel->send_pkt_srv2cli(slot_id, datum_tofwd);

	// return 'tokeep'
	return true;	
}

NEOIP_NAMESPACE_END



