/*! \file
    \brief Definition of the ntudp_pserver_reverse_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_pserver_reverse.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_portid.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_itor_reverse.hpp"
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
ntudp_pserver_reverse_t::ntudp_pserver_reverse_t(ntudp_pserver_t *ntudp_pserver, udp_full_t *udp_full)
										throw()
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

	// link the connection to the list
	ntudp_pserver->reverse_link( this );
}

/** \brief destructor
 */
ntudp_pserver_reverse_t::~ntudp_pserver_reverse_t()				throw()
{
	// log to debug
	KLOG_ERR("enter");
	// unlink the connection to the list
	ntudp_pserver->reverse_unlink( this );
	// close the udp_full_t connection
	nipmem_zdelete udp_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	ntudp_pserver_reverse_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:	// autodelete of the reverse_t
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

/** \brief Handle received data on the reverse_t
 * 
 * - NOTE: this may delete the ntudp_pserver_t or the udp_full_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_pserver_reverse_t::handle_recved_data(pkt_t &pkt)		throw()
{
	ntudp_sock_pkttype_t	pkttype;
	// log to debug
	KLOG_ERR("enter");

	try {	// read the pkttype (without consuming)	
		pkt.unserial_peek( pkttype );
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_sock_pkttype_t::REVERSE_CNX_R2I_ACK:	return recv_reverse_cnx_r2i_ack(pkt);	
		default:	KLOG_ERR("Unexpected packet type " << pkttype);
				// return tokeep
				return true;
		}
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
		// return tokeep
		return true;
	}

	// autodelete the connection
	nipmem_delete this;
	// return 'dontkeep'
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle the reception of a REVERSE_CNX_R2I_ACK
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_pserver_reverse_t::recv_reverse_cnx_r2i_ack(pkt_t &orig_pkt)	throw(serial_except_t)
{
	ntudp_sock_pkttype_t	pkttype;
	ntudp_nonce_t		incoming_nonce;
	ntudp_peerid_t		itor_peerid, resp_peerid;
	ntudp_portid_t		itor_portid, resp_portid;
	ntudp_peer_t *		ntudp_peer	= ntudp_pserver->ntudp_peer;
	// copy the packet to forward it untouched to the tunnel 
	pkt_t		pkt	= orig_pkt;	
	// parse the packet
	pkt >> pkttype;
	pkt >> incoming_nonce;
	pkt >> resp_peerid >> resp_portid;
	pkt >> itor_peerid >> itor_portid;
	
	// try to find the ntudp_itor_t matching this connection address
	ntudp_addr_t	itor_addr	= ntudp_addr_t(itor_peerid, itor_portid);
	ntudp_addr_t	resp_addr	= ntudp_addr_t(resp_peerid, resp_portid);
	ntudp_itor_t * 	ntudp_itor	= ntudp_peer->itor_from_cnxaddr(itor_addr, resp_addr);

	// try to find the ntudp_itor_reverse_t matching this incoming nonce
	ntudp_itor_reverse_t *	itor_reverse = NULL;
	if( ntudp_itor ) itor_reverse	= ntudp_itor->get_reverse_from_nonce(incoming_nonce);

	// if no ntudp_itor_reverse_t matches, return a error
	if( !ntudp_itor || !itor_reverse ){
		// build the packet to reply
		pkt_t	pkt_err	= ntudp_sock_errcode_build_pkt(ntudp_sock_errcode_t::UNKNOWN_CNX, incoming_nonce);
		// send back the packet
		udp_full->send(pkt_err);
		// autodelete
		nipmem_delete	this;
		// return dontkeep
		return false;
	}

	// steal the udp_full_t to pass it to the itor_reverse
	udp_full_t *	udp_full_stolen	= udp_full;
	slot_id_t	udp_full_slotid	= udp_full_stolen->get_object_slotid();
	// mark the local udp_full unused
	udp_full 	= NULL;

	// autodelete this connection
	nipmem_delete this;

	// notify the ntudp_itor_reverse_t of the received ntudp_sock_pkttype_t::REVERSE_CNX_R2I_ACK
	// - it MUST be done last as it may cause the deletion of the whole ntudp_peer_t
	itor_reverse->notify_recved_reverse_cnx_r2i_ack(orig_pkt, udp_full_stolen);

	// return the tokeep value for the udp_full_t
	return object_slotid_tokeep(udp_full_slotid);
}

NEOIP_NAMESPACE_END



