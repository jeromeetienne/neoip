/*! \file
    \brief Definition of the ntudp_itor_estarelay_2peer_t

\par Brief description
This modules handle the itor part for the estarelay connection but only the part between
the itor and the relay. the part between itor and peer is done by \ref ntudp_itor_estarelay_2peer_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_itor_estarelay_2peer.hpp"
#include "neoip_ntudp_itor_estarelay.hpp"
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_itor_profile.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_udp_client.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_itor_estarelay_2peer_t::ntudp_itor_estarelay_2peer_t(ntudp_itor_estarelay_t *itor_estarelay
				, const ipport_addr_t &local_ipport, const ipport_addr_t &remote_ipport
				, ntudp_itor_estarelay_2peer_cb_t *callback, void * userptr)	throw()
{
	// copy the parameter
	this->itor_estarelay	= itor_estarelay;
	this->callback		= callback;
	this->userptr		= userptr;

	// create and start the udp_client
	inet_err_t	inet_err;
	udp_client	= nipmem_new udp_client_t();
	inet_err	= udp_client->set_local_addr(local_ipport);
	DBG_ASSERT( inet_err.succeed() );	
	inet_err	= udp_client->start(remote_ipport, this, NULL);
	DBG_ASSERT( inet_err.succeed() );
}

/** \brief Desstructor
 */
ntudp_itor_estarelay_2peer_t::~ntudp_itor_estarelay_2peer_t()		throw()
{
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
bool ntudp_itor_estarelay_2peer_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	
	// sanity check - ensure the rxmit_timeout is not configured to timeout
	DBG_ASSERT( rxmit_delaygen.get_arg().timeout_delay().is_special() );
		
	// build the request
	pkt_t	pkt	= build_estarelay_cnx_i2r_ack();
	// send the request
	udp_client->send( pkt );

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

/** \brief build the ESTARELAY_CNX_I2R_ACK
 */
pkt_t ntudp_itor_estarelay_2peer_t::build_estarelay_cnx_i2r_ack()	const throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << ntudp_sock_pkttype_t(ntudp_sock_pkttype_t::ESTARELAY_CNX_I2R_ACK);
	// put the nonce
	pkt << itor_estarelay->client_nonce;
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
bool	ntudp_itor_estarelay_2peer_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw()
{
	ntudp_itor_t *	ntudp_itor	= itor_estarelay->ntudp_itor;	
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
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_itor_estarelay_2peer_t::handle_recved_data(pkt_t &pkt)	throw()
{
	ntudp_sock_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype (without consuming)	
		pkt.unserial_peek( pkttype );
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_sock_pkttype_t::ESTARELAY_CNX_R2I_ACK:return recv_estarelay_cnx_r2i_ack(pkt);
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


/** \brief handle the reception of a ESTARELAY_CNX_R2I_ACK
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_itor_estarelay_2peer_t::recv_estarelay_cnx_r2i_ack(pkt_t &pkt_r2i_ack)	throw(serial_except_t)
{
	ntudp_sock_pkttype_t	pkttype;	
	ntudp_nonce_t		incoming_nonce;
	pkt_t			pkt = pkt_r2i_ack;
	// log to debug
	KLOG_ERR("enter");
	// read the pkttype
	pkt >> pkttype;
	// read the incoming nounce
	pkt >> incoming_nonce;

	// if the incoming_nonce is different thand client_nonce, discard the packet
	if( incoming_nonce != itor_estarelay->client_nonce )	return true;

	// steal the udp_full_t from the udp_client_t
	udp_full_t *	udp_full_stolen	= udp_client->steal_full();
	// delete the udp_client and mark it unused
	nipmem_delete	udp_client;
	udp_client	= NULL;

	// notify the sucess to the caller
	pkt_t	pkt_i2r_ack	= build_estarelay_cnx_i2r_ack();
	notify_callback(udp_full_stolen, pkt_r2i_ack, pkt_i2r_ack);
	
	// return dontkeep as the udp_client has been deleted
	return false;
}

/** \brief handle the reception of a ERROR_PKT
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_itor_estarelay_2peer_t::recv_error_pkt(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_sock_pkttype_t	pkttype;	
	ntudp_nonce_t		incoming_nonce;
	ntudp_sock_errcode_t	sock_errcode;
	// log to debug
	KLOG_ERR("enter");
	// read the pkttype
	pkt >> pkttype;
	// read the incoming nounce
	pkt >> incoming_nonce;
	// if the incoming_nonce is different thand client_nonce, discard the packet
	if( incoming_nonce != itor_estarelay->client_nonce )	return true;
	
	// read the ntudp_sock_errcode_t
	pkt >> sock_errcode;
	
	// log to debug
	KLOG_ERR("sock_errcode=" << sock_errcode);
	
	// notify the faillure to the caller
	return notify_callback(NULL, pkt_t(), pkt_t());
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
bool 	ntudp_itor_estarelay_2peer_t::notify_callback(udp_full_t *udp_full, const pkt_t &estapkt_in
							, const pkt_t &estapkt_out)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_itor_estarelay_2peer_cb(userptr, *this
						, udp_full, estapkt_in, estapkt_out);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - specific to ntudp_itor_estarelay_2peer_t, the object MUST be deleted during notification
	DBG_ASSERT( tokeep == false );	
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


