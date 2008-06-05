/*! \file
    \brief Definition of the ntudp_relpeer_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_relpeer_tunnel.hpp"
#include "neoip_ntudp_relpeer_tunnel_wikidbg.hpp"
#include "neoip_ntudp_tunl_pkttype.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_portid.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_udp_layer.hpp"
#include "neoip_udp_client.hpp"
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
ntudp_relpeer_tunnel_t::ntudp_relpeer_tunnel_t()	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// setup some variables
	ntudp_peer	= NULL;
	udp_client	= NULL;
	itor_inprogress	= true;
	// init the client_nonce
	// - this is a anti-DoS measure forcing an attacker to be onpath if it wants packet to be accepted
	// - this remains the same value for the whole life of this tunnel_t
	client_nonce	= ntudp_nonce_t::build_random();
}

/** \brief destructor
 */
ntudp_relpeer_tunnel_t::~ntudp_relpeer_tunnel_t()				throw()
{
	// log to debug
	KLOG_ERR("enter");		
	// delete the udp_client_t if needed
	if( udp_client )	nipmem_delete udp_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        start() option
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_relpeer_tunnel_t &ntudp_relpeer_tunnel_t::set_profile(const ntudp_relpeer_tunnel_profile_t &profile)
										throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
ntudp_err_t	ntudp_relpeer_tunnel_t::start(const ipport_addr_t &pserver_addr, ntudp_peer_t *ntudp_peer
				, ntudp_relpeer_tunnel_cb_t *callback, void * userptr)		throw()
{
	inet_err_t	inet_err;
	// copy the parameters
	this->ntudp_peer	= ntudp_peer;
	this->callback		= callback;
	this->userptr		= userptr;	
	// start the udp_client_t
	udp_client		= nipmem_new udp_client_t();
	inet_err		= udp_client->start(pserver_addr, this, NULL);
	if( inet_err.failed() )	return ntudp_err_from_inet(inet_err);

	// return the error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        utility function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Return the pserver_addr which is used for this tunnel_t
 */
const ipport_addr_t &	ntudp_relpeer_tunnel_t::get_pserver_addr() const throw()
{
	return udp_client->get_remote_addr();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool	ntudp_relpeer_tunnel_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// notify the expiration
	rxmit_delaygen.notify_expiration();
	// if the delaygen is is_timedout, this means the probing period timedout so autodelete this tunnel_t
	if( rxmit_delaygen.is_timedout() ){
		// notify that the pserver_addr is timedout
		ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();
		pserver_pool->notify_timedout(get_pserver_addr(), rxmit_delaygen.elapsed_delay() 
								- rxmit_delaygen.get_arg().first_delay());
		// notify the caller of the faillure
		return notify_callback(false);
	}

	// build and send the request
	pkt_t	pkt	= build_pkt_register_request();
	udp_client->send( pkt );

	// set the next period
	rxmit_timeout.change_period(rxmit_delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the REGISTER_REQUEST packet
 */
pkt_t	ntudp_relpeer_tunnel_t::build_pkt_register_request()	const throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << ntudp_tunl_pkttype_t(ntudp_tunl_pkttype_t::REGISTER_REQUEST);
	// add the client_nonce
	pkt << client_nonce;
	// add the client peerid
	pkt << ntudp_peer->local_peerid();
	// return the packet
	return pkt;
}

/** \brief build the PKT_CLI2SRV
 */
pkt_t	ntudp_relpeer_tunnel_t::build_pkt_cli2srv(const slot_id_t &extcnx_slotid
						, const datum_t &datum_tofwd)	const throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << ntudp_tunl_pkttype_t(ntudp_tunl_pkttype_t::PKT_CLI2SRV);
	// add the client_nonce
	pkt << client_nonce;
	// add the external connection slot_id_t
	pkt << extcnx_slotid;
	// add the datum_tofwd
	pkt << datum_tofwd;
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
bool	ntudp_relpeer_tunnel_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( udp_event.is_client_ok() );

	// handle the fatal events
	if( udp_event.is_fatal() ){
		ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();
		// notify the pserver_addr as unreachable
		pserver_pool->notify_unreach(get_pserver_addr());
		// notify the caller of the failure
		return notify_callback(false);
	}

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:
			// init the rxmit_delaygen for the itor phase
			rxmit_delaygen	= delaygen_t(profile.itor_pkt_rxmit());
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

/** \brief Handle received data on the tunnel_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_relpeer_tunnel_t::handle_recved_data(pkt_t &pkt)	throw()
{
	ntudp_tunl_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype
		pkt >> pkttype;
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_tunl_pkttype_t::REGISTER_REPLY:	return recv_register_reply(pkt);
		case ntudp_tunl_pkttype_t::PKT_SRV2CLI:		return recv_pkt_srv2cli(pkt);
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

/** \brief handle the reception of a REGISTER_REPLY
 * 
 * @return a 'tokeep' for the udp_client_t
 */
bool	ntudp_relpeer_tunnel_t::recv_register_reply(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();
	ntudp_nonce_t		incoming_nonce;
	bool			accepted_f;
	// read the incoming nounce
	pkt >> incoming_nonce;
	// read the accepted_f
	pkt >> accepted_f;
	// if the incoming_nonce is not equal to the client_nonce, discard this packet
	// - the client_nonce is used as a DoS protection. forcing an attacker to be onpath
	if( incoming_nonce != client_nonce  )	return true;
	
	// if the ntudp_pserver_t doesnt/nomore accepts the tunnel, 
	if( !accepted_f ){
		// notify the pserver_addr as unreachable
		pserver_pool->notify_unreach(get_pserver_addr());
		// notify the ntudp_relpeer_t that this tunnel is over
		return notify_callback(false);
	}

	// notify the pserver_addr as reachable - as a packet from a public server has just been received
	pserver_pool->notify_reach(get_pserver_addr());
	
	// if this tunnel_t is still in itor phase, pass it to full
	if( itor_inprogress ){
		// update itor_inprogress
		itor_inprogress	= false;
		// init the rxmit_delaygen for the full phase
		rxmit_delaygen	= delaygen_t(profile.full_pkt_rxmit());		
		// notify the ntudp_relpeer_t that this tunnel is established
		bool tokeep = notify_callback(true);
		if( !tokeep )	return false;
	}
	// reset the rxmit_delaygen
	rxmit_delaygen.reset();
	// restart the rxmit_timeout
	rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
	
	// return 'tokeep'
	return true;
}

/** \brief handle the reception of a PKT_SRV2CLI
 * 
 * @return a 'tokeep' for the udp_client_t
 */
bool	ntudp_relpeer_tunnel_t::recv_pkt_srv2cli(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();
	ntudp_nonce_t		incoming_nonce;
	slot_id_t		extcnx_slotid;
	datum_t			fwded_datum;
	// log to debug
	KLOG_ERR("enter");
	// read the incoming_nonce as a 'security measure'
	pkt >> incoming_nonce;
	// read the extcnx_slotid to determine on which ntudp_pserver_extcnx_t forward this packet
	pkt >> extcnx_slotid;
	// read the forwarded packet to forward
	pkt >> fwded_datum;

	// if the incoming_nonce is different thand client_nonce, discard the packet
	if( incoming_nonce != client_nonce )	return true;

	// notify the pserver_addr as reachable - as a packet from a public server has just been received
	pserver_pool->notify_reach(get_pserver_addr());
	
	// pass the fwded_datum to ntudp_resp_t to handle
	datum_t	datum_toreply	= ntudp_resp_t::notify_recved_pkt_tunl_srv2cli(ntudp_peer, fwded_datum);
	// if there are no datum_toreply, return now
	if( datum_toreply.is_null() )	return true;

	// build the pkt_cli2srv with the datum_toreply
	pkt_t	pkt_cli2srv	= build_pkt_cli2srv(extcnx_slotid, datum_toreply);
	// send the reply thru the connection
	udp_client->send(pkt_cli2srv);
	
	// return 'tokeep'
	return true;
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
bool 	ntudp_relpeer_tunnel_t::notify_callback(const bool succeed)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_relpeer_tunnel_cb(userptr, *this, succeed);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



