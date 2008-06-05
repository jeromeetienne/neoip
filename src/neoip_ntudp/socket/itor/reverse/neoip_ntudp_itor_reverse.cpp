/*! \file
    \brief Definition of the ntudp_itor_reverse_t

\par Brief description
This modules handle the itor part for the reverse connection.

*/

/* system include */
/* local include */
#include "neoip_ntudp_itor_reverse.hpp"
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_itor_profile.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_ntudp_tunl_pkttype.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_udp_client.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_itor_reverse_t::ntudp_itor_reverse_t(ntudp_itor_t *ntudp_itor, const ipport_addr_t &pserver_ipport
				, ntudp_itor_reverse_cb_t *callback, void * userptr)		throw()
{
	inet_err_t	inet_err;
	// copy the parameter
	this->ntudp_itor	= ntudp_itor;
	this->pserver_ipport	= pserver_ipport;
	this->callback		= callback;
	this->userptr		= userptr;
	// setup the nonce
	client_nonce		= ntudp_nonce_t::build_random();
	// sanity check - here local peer MUST be inetreach OK
	DBG_ASSERT( ntudp_itor->ntudp_peer->get_npos_res().inetreach() );
	// set the listen_addr_pview now
	// - thus if the local peer becomes non inetreach during this itor, no issue will happen
	listen_addr_pview	= ntudp_itor->ntudp_peer->listen_addr_pview();
	// create and start the udp_client
	udp_client		= nipmem_new udp_client_t();
	inet_err		= udp_client->start(pserver_ipport, this, NULL);
	DBG_ASSERT( inet_err.succeed() );
	// link this object to the ntudp_itor_t
	ntudp_itor->reverse_link(this);
}

/** \brief Desstructor
 */
ntudp_itor_reverse_t::~ntudp_itor_reverse_t()		throw()
{
	// unlink this object from the ntudp_itor_t
	ntudp_itor->reverse_unlink(this);
	// delete the udp_client if needed
	if(udp_client)	nipmem_delete	udp_client;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool ntudp_itor_reverse_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// sanity check - ensure the rxmit_timeout is not configured to timeout
	DBG_ASSERT( rxmit_delaygen.get_arg().timeout_delay().is_special() );

	// build the request
	pkt_t	pkt_request = build_reverse_cnx_request();
	pkt_t	pkt_tosend  = ntudp_tunl_pkttype_build_pkt_ext2srv(client_nonce
							, ntudp_itor->remote_addr().peerid()
							, pkt_request.to_datum());
	// send the request
	udp_client->send( pkt_tosend );

	// set the next timer
	rxmit_timeout.change_period(rxmit_delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the RELAYED_CNX_REQUEST
 */
pkt_t ntudp_itor_reverse_t::build_reverse_cnx_request()	const throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << ntudp_sock_pkttype_t(ntudp_sock_pkttype_t::REVERSE_CNX_REQUEST);
	// put the nonce
	pkt << client_nonce;
	// add the destination peerid
	pkt << ntudp_itor->remote_addr().peerid();
	// add the destination portid
	pkt << ntudp_itor->remote_addr().portid();
	// add the source peerid
	pkt << ntudp_itor->local_addr().peerid();
	// add the source portid
	pkt << ntudp_itor->local_addr().portid();
	// add the listen_addr_pview
	pkt << listen_addr_pview;
	// return the packet
	return pkt;
}

/** \brief build the REVERSE_CNX_I2R_ACK
 */
pkt_t ntudp_itor_reverse_t::build_reverse_cnx_i2r_ack()	const throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << ntudp_sock_pkttype_t(ntudp_sock_pkttype_t::REVERSE_CNX_I2R_ACK);
	// put the nonce
	pkt << client_nonce;
	// return the packet
	return pkt;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     udp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_client_t when a connection is established
 */
bool	ntudp_itor_reverse_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( udp_event.is_client_ok() );
	
	// handle the fatal events
	if( udp_event.is_fatal() ){
		// log the event
		KLOG_INFO("received fatal event on " << *udp_client );
		// notify the faillure
		return notify_callback(NULL, pkt_t(), pkt_t());
	}

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:
			// init delaygen for the packet rxmit
			rxmit_delaygen = delaygen_t(ntudp_itor->profile->itor_pkt_rxmit());
			// start the timer immediatly
			rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
			break;
	case udp_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*udp_event.get_recved_data());
	default:	break;
	}
	// return 'tokeep'
	return true;
}

/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_client_t
 */
bool	ntudp_itor_reverse_t::handle_recved_data(pkt_t &pkt)	throw()
{
	ntudp_sock_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype
		pkt >> pkttype;
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_sock_pkttype_t::REVERSE_CNX_REPLY:	return recv_reverse_cnx_reply(pkt);
		case ntudp_sock_pkttype_t::ERROR_PKT:		return recv_error_pkt(pkt);
		default:	KLOG_ERR("Unexpected packet type " << pkttype );
				return true;
		}
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
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


/** \brief handle the reception of a REVERSE_CNX_REPLY
 * 
 * - to receive this packet implies the responder will now try to send REVERSE_CNX_R2I_ACK
 *   on the public view of the listen address. aka the ntudp_dircnx_server_t will receive
 *   it and pass it to this function.
 *   - so stop sending REVERSE_CNX_REQUEST
 *   - and wait for REVERSE_CNX_R2I_ACK to be received
 * 
 * @return a 'tokeep/dontkeep' for the udp_client_t
 */
bool	ntudp_itor_reverse_t::recv_reverse_cnx_reply(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_nonce_t	incoming_nonce;
	// log to debug
	KLOG_ERR("enter");
	// read the incoming nounce
	pkt >> incoming_nonce;
	// if the incoming_nonce is different thand client_nonce, discard the packet
	if( incoming_nonce != client_nonce )	return true;

	// stop the timer
	rxmit_timeout.stop();
	// delete the udp_client and mark it unused
	nipmem_delete	udp_client;
	udp_client	= NULL;

	// return dontkeep
	return false;
}

/** \brief handle the reception of a ERROR_PKT
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_itor_reverse_t::recv_error_pkt(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_nonce_t		incoming_nonce;
	ntudp_sock_errcode_t	sock_errcode;
	// log to debug
	KLOG_ERR("enter");
	// read the incoming nounce
	pkt >> incoming_nonce;
	// if the incoming_nonce is different thand client_nonce, discard the packet
	if( incoming_nonce != client_nonce )	return true;
	
	// read the ntudp_sock_errcode_t
	pkt >> sock_errcode;
	
	// log to debug
	KLOG_ERR("sock_errcode=" << sock_errcode);
	
	// notify the faillure to the caller
	return notify_callback(NULL, pkt_t(), pkt_t());
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function called with a REVERSE_CNX_R2I_ACK has been received 
 * 
 * - WARNING: this function does a notification to the ntudp_itor_t caller
 *   - he may delete the whole ntudp_peer_t, so take this into account using this function
 *   - e.g. assume the ntudp_peer_t have been deleted during this function
 */
void	ntudp_itor_reverse_t::notify_recved_reverse_cnx_r2i_ack(const pkt_t &request_pkt
							, udp_full_t *reverse_udp_full)	throw()
{
	// copy the packet to pass it untouched to ntudp_full_t
	pkt_t			pkt	= request_pkt;
	ntudp_sock_pkttype_t	pkttype;
	ntudp_nonce_t		incoming_nonce;

	// parse the packet
	pkt >> pkttype;
	// sanity check - only pkttype equal to REVERSE_CNX_R2I_ACK should arrive on this part of the code
	DBG_ASSERT( pkttype == ntudp_sock_pkttype_t::REVERSE_CNX_R2I_ACK );

	// return the nonce
	pkt >> incoming_nonce;
	// if the incoming_nonce is different thand client_nonce, discard the packet
	if( incoming_nonce != client_nonce )	return;

	// send a REVERSE_CNX_REPLY thru the notified udp_full_t 
	pkt_t	reply_pkt	= build_reverse_cnx_i2r_ack();
	reverse_udp_full->send(reply_pkt);

	// notify the caller of the sucess
	// - it MUST be done last as it may cause the deletion of the whole ntudp_peer_t
	notify_callback(reverse_udp_full, request_pkt, reply_pkt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	ntudp_itor_reverse_t::notify_callback(udp_full_t *udp_full, const pkt_t &estapkt_in
						, const pkt_t &estapkt_out)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_itor_reverse_cb(userptr, *this, udp_full
							, estapkt_in, estapkt_out);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - specific to ntudp_itor_direct_t, the object MUST be deleted during notification
	DBG_ASSERT( tokeep == false );		
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


