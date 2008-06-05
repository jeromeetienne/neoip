/*! \file
    \brief Definition of the ntudp_dircnx_server_t
    
*/

/* system include */
/* local include */
#include "neoip_ntudp_dircnx_server_cnx.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_dircnx_server_t::cnx_t::cnx_t(ntudp_dircnx_server_t *dircnx_server, udp_full_t *udp_full)		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy parameter
	this->dircnx_server= dircnx_server;
	this->udp_full	= udp_full;
	// set the callback for the udp_full
	inet_err_t	inet_err;
	inet_err = udp_full->set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
	// start the udp_full
// TODO here there is an issue due to udp_vresp_t
// - udp_vresp_t do a start() on the udp_full to get the first packet
// - and here i do another start on it ...
// - hence the udp_full is started twice and the fails
//	inet_err = udp_full->start();
//	DBG_ASSERT( inet_err.succeed() );
	// link the connection to the list
	dircnx_server->cnx_link( this );
}

/** \brief destructor
 */
ntudp_dircnx_server_t::cnx_t::~cnx_t()				throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink the connection to the list
	dircnx_server->cnx_unlink( this );
	// close the full connection
	if( udp_full )		nipmem_delete udp_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	ntudp_dircnx_server_t::cnx_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:	// autodelete of the cnx_t
					nipmem_delete	this;
					return false;
	case udp_event_t::RECVED_DATA:	// handle the received packet
					return handle_recved_data(*udp_event.get_recved_data());
	case udp_event_t::MTU_CHANGE:	// this event MUST NOT happend as this udp_full_t has no pmtudisc
					DBG_ASSERT( 0 );
	default:	DBG_ASSERT( 0 );
	}
	// return a 'tokeep'
	return true;
}

/** \brief Handle received data on the cnx_t
 * 
 * - NOTE: this may delete the ntudp_dircnx_server_t or the udp_full_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_dircnx_server_t::cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	ntudp_sock_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype (without consuming)	
		pkt.unserial_peek( pkttype );
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_sock_pkttype_t::DIRECT_CNX_REQUEST:	return recv_direct_cnx_request(pkt);
		default:	KLOG_ERR("Unexpected packet type " << pkttype );
				goto delete_cnx;
		}
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
		// delete cnx
		goto delete_cnx;
	}

	// return 'tokeep'
	return true;

delete_cnx:	// autodelete
		nipmem_delete this;
		// return 'dontkeep'
		return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle the reception of a DIRECT_CNX_REQUEST
 */
bool	ntudp_dircnx_server_t::cnx_t::recv_direct_cnx_request(pkt_t &orig_pkt)	throw(serial_except_t)
{
	ntudp_sock_pkttype_t	pkttype;
	ntudp_nonce_t		client_nonce;
	ntudp_peerid_t		dst_peerid;
	ntudp_portid_t		dst_portid;
	ntudp_peer_t *	ntudp_peer	= dircnx_server->ntudp_peer;
	pkt_t		pkt		= orig_pkt;
	// log to debug
	KLOG_DBG("enter");
	// parse the packet
	pkt >> pkttype;
	pkt >> client_nonce;
	pkt >> dst_peerid;
	pkt >> dst_portid;
	
	// the dst_peerid MUST be the local one. If not, reply an error packet
	if( dst_peerid != ntudp_peer->local_peerid() ){
		// reply the error packet
		pkt_t	pkt_err	= ntudp_sock_errcode_build_pkt(ntudp_sock_errcode_t::UNREACH_PEERID
										, client_nonce);
		udp_full->send( pkt_err );
		// autodelete 
		nipmem_delete	this;
		return false;	
	}

	// try to find the ntudp_resp_t matching the dst_addr.portid()
	ntudp_resp_t *	ntudp_resp	= ntudp_peer->resp_from_portid(dst_portid);

	// if it is not found, reply a error packet
	if( !ntudp_resp ){
		// reply the error packet
		pkt_t	pkt_err	= ntudp_sock_errcode_build_pkt(ntudp_sock_errcode_t::UNREACH_PORTID
										, client_nonce);
		udp_full->send( pkt_err );
		// autodelete 
		nipmem_delete	this;
		return false;		
	}
	
	// steal the udp_full_t from the udp_client_t
	udp_full_t *	udp_full_stolen	= udp_full;
	slot_id_t	udp_full_slotid	= udp_full_stolen->get_object_slotid();
	// mark the local udp_full unused
	udp_full	= NULL;	
	
	// autodelete
	nipmem_delete	this;
	
	// notify the ntudp_resp_t of the reception of a DIRECT_CNX_REPLY for it
	// - NOTE: this may DELETE ntudp_peer_t - so MUST be done last
	ntudp_resp->notify_recved_direct_cnx_request(orig_pkt, udp_full_stolen);

	// return the tokeep value for the udp_full_t
	return object_slotid_tokeep(udp_full_slotid);
}

NEOIP_NAMESPACE_END



