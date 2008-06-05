/*! \file
    \brief Definition of the ntudp_npos_server_t
    
*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_server_cnx.hpp"
#include "neoip_ntudp_npos_pkttype.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_udp_client.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_udp_layer.hpp"
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
ntudp_npos_server_t::cnx_t::cnx_t(ntudp_npos_server_t *ntudp_npos_server, udp_full_t *udp_full)		throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");
	// zero some parameter
	this->probe_udp_client	= NULL;
	// copy parameter
	this->ntudp_npos_server	= ntudp_npos_server;
	this->udp_full		= udp_full;
	// set the callback for the udp_full
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
	ntudp_npos_server->cnx_link( this );
}

/** \brief destructor
 */
ntudp_npos_server_t::cnx_t::~cnx_t()				throw()
{
	// log to debug
	KLOG_DBG("enter");
	// close the udp_client for send INETREACH_PROBE
	if( probe_udp_client )	nipmem_delete probe_udp_client;
	// close the full connection
	if( udp_full )		nipmem_delete udp_full;
	// unlink the connection to the list
	ntudp_npos_server->cnx_unlink( this );	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	ntudp_npos_server_t::cnx_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
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
	case udp_event_t::MTU_CHANGE:	// this event MUST NOT happend as this connection has no pmtudisc
					DBG_ASSERT( 0 );
	default:	DBG_ASSERT( 0 );
	}
	// return a 'tokeep'
	return true;
}

/** \brief Handle received data on the cnx_t
 * 
 * - NOTE: this may delete the ntudp_npos_server_t or the udp_full_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_npos_server_t::cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	ntudp_npos_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype
		pkt >> pkttype;
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_npos_pkttype_t::SADDRECHO_REQUEST:	return recv_saddrecho_request(pkt);
		case ntudp_npos_pkttype_t::INETREACH_REQUEST:	return recv_inetreach_request(pkt);				
		case ntudp_npos_pkttype_t::INETREACH_PROBE:	return recv_inetreach_probe(pkt);
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

/** \brief handle the reception of a SADDRECHO_REQUEST
 */
bool	ntudp_npos_server_t::cnx_t::recv_saddrecho_request(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_nonce_t	request_nonce;
	// log to debug
	KLOG_DBG("enter");
	// read the incoming nounce
	pkt >> request_nonce;
	// build the SADDRECHO_REPLY
	pkt = build_saddrecho_reply(request_nonce);
	// send the reply thru the connection
	udp_full->send(pkt);
	// autodelete
	nipmem_delete	this;
	// return 'dontkeep'
	return false;	
}

/** \brief handle the reception of a INETREACH_REQUEST
 */
bool	ntudp_npos_server_t::cnx_t::recv_inetreach_request(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_nonce_t	request_nonce;
	ipport_addr_t	probe_daddr;
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");
	// read the incoming nounce
	pkt >> request_nonce;
	// read the probe_daddr
	pkt >> probe_daddr;

	// build and send the INETREACH_REPLY
	pkt = build_inetreach_reply(request_nonce);
	udp_full->send(pkt);

	// build the INETREACH_PROBE and store it in probe_pkt
	probe_pkt = build_inetreach_probe(request_nonce);
	// start the probe_udp_client to send the INETREACH_PROBE
	DBG_ASSERT( probe_udp_client == NULL );
	probe_udp_client= nipmem_new udp_client_t();
	inet_err	= probe_udp_client->start(probe_daddr, this, NULL);
	if( inet_err.failed() ){
		nipmem_delete this;
		return false;
	}

	// delete the udp_full_t and mark it unused - thus no packet will arrive on this connection
	nipmem_delete	udp_full;
	udp_full = NULL;
	// return 'dontkeep' 
	return false;
}

/** \brief handle the reception of a INETREACH_PROBE
 */
bool	ntudp_npos_server_t::cnx_t::recv_inetreach_probe(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_nonce_t	request_nonce;
	// log to debug
	KLOG_DBG("enter");
	// read the incoming nounce
	pkt >> request_nonce;

	// try to find a registered callback for this probe's nonce
	probe_cb_t	probe_cb	= ntudp_npos_server->inetreach_probe_find(request_nonce);
	// if one is found, notify it of the reception of the nonce
	if( !probe_cb.is_null() ){
		probe_cb.get_callback()->neoip_ntudp_npos_server_probe_event_cb(probe_cb.get_userptr()
										, *ntudp_npos_server);
	}else{	// if none is found, discard the packet and log the event
		// - NOTE: this may happen in normal case due to race between the sender/receiver
		KLOG_DBG("Received a INETREACH_PROBE with an unexpected nonce. discarding the packet");
	}

	// then autodelete this cnx_t
	nipmem_delete	this;
	// return 'dontkeep'
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build a SADDRECHO_REPLY packet
 */
pkt_t	ntudp_npos_server_t::cnx_t::build_saddrecho_reply(const ntudp_nonce_t &request_nonce)	throw()
{
	pkt_t	pkt;
	// add the packet type
	pkt << ntudp_npos_pkttype_t(ntudp_npos_pkttype_t::SADDRECHO_REPLY);
	// add the request_nonce
	pkt << request_nonce;
	// echo the remote address
	pkt << udp_full->get_remote_addr();
	// return the generated packet
	return pkt;
}

/** \brief build a INETREACH_REPLY packet
 */
pkt_t	ntudp_npos_server_t::cnx_t::build_inetreach_reply(const ntudp_nonce_t &request_nonce)	throw()
{
	pkt_t	pkt;
	// add the packet type
	pkt << ntudp_npos_pkttype_t(ntudp_npos_pkttype_t::INETREACH_REPLY);
	// add the request_nonce
	pkt << request_nonce;
	// return the generated packet
	return pkt;
}

/** \brief build a INETREACH_PROBE packet
 */
pkt_t	ntudp_npos_server_t::cnx_t::build_inetreach_probe(const ntudp_nonce_t &request_nonce)	throw()
{
	pkt_t	pkt;
	// add the packet type
	pkt << ntudp_npos_pkttype_t(ntudp_npos_pkttype_t::INETREACH_PROBE);
	// add the request_nonce
	pkt << request_nonce;
	// return the generated packet
	return pkt;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     udp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_client_t when a connection is established
 * 
 * - NOTE: used only to send the INETREACH_PROBE. it sends one, and then close the cnx_t
 */
bool	ntudp_npos_server_t::cnx_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( udp_event.is_client_ok() );

	// handle each possible events from its type
	// - special handling due to this special case which is very limited
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:
			// if the connection is established, send the INETREACH_PROBE
			// - the packet is prebuilt and stored in probe_pkt
			probe_udp_client->send( probe_pkt );
			break;
	default:	break;
	}	
	
	// autodelete the connection - in ALL cases
	nipmem_delete	this;
	// return 'dontkeep'
	return false;
}



NEOIP_NAMESPACE_END



