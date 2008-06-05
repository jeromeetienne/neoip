/*! \file
    \brief Definition of the router_itor_t

\par Description of the address negociation
There are 3 packets:
-# ADDRNEGO_REQUEST which contains
  - a proposed itor_addr and resp_addr
  - a list of intervals for all the available address for the itor
  - this packet is sent only in the itor->resp direction
-# ADDRNEGO_REPLY which contains
  - a replied itor_addr and resp_addr
  - this packet is sent only in the resp->itor direction
-# ADDRNEGO_ACK which contains
  - a acked itor_addr and resp_addr
    - TODO are they mandatorily the same as the ones in ADDRNEGO_REPLY ? if yes, note it there
  - this packet is sent only in the itor->resp direction

\par Motivation for the ADDRNEGO_ACK packet
- between the time, the itor sent the request and receives the reply, the available
  address may have changed.
  - for example, an external code may have used one of the addresses which was
    available when the request got sent but which is no more when reply is received
- so there is a ack packet which is supposed to contains exactly the same addresses
  as the reply.

\par ADDRNEGO_REQUEST packet
- Building: by the itor
  - the itor check if the address cache contains the resp peerid.
    - if it is contained in the address cache and if the stored local_addr and
      remote addr are both currently available, copy them in the packet.
    - else set it to null.
   - the itor sends a list of interval describing all the available addresses for 
     the itor point of view
- Parsing: by the resp (and building ADDRNEGO_REPLY)
  - if the itor/resp addr in the packet are not null, and if they are available
    for the resp, build a reply packet with them
  - else build an interval list of the addresses which are available for the
    resp AND the itor, pick 2 of them and build a reply packet with them.

\par ADDRNEGO_REPLY
- Parsing: by the itor
  - if the replied itor/resp_addr are still free in the itor, accept them
    and put them in a ack packet
  - else send a new request

\par ADDRNEGO_ACK
- Parsing: by the resp
  - if the acked itor/resp_addr are the same sent in the reply, accept them
  - else close the connection

\par Packet retransmition
- all this address negociation looks a lot like a 3 packet connection establishment
  - the itor rxmit the request until it receives a reply
  - the resp rxmit the reply until it receives an ack or another request
  - the itor accept the connection on reply reception and set the estapkt packet appropriatly
  - the resp accept the connection on ack reception and set the estapkt packet appropriatly
*/

/* system include */
/* local include */
#include "neoip_router_itor_cnx.hpp"
#include "neoip_router_pkttype.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_client.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// include to directly access the socket_profile_t of the socket_domain_t::NTLAY
#include "neoip_socket_profile_ntlay.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_itor_cnx_t::router_itor_cnx_t(router_itor_t *router_itor)	throw()
{
	// copy the parameters
	this->router_itor	= router_itor;
	// zero some field
	this->socket_client	= NULL;
	// link this cnx_t to the router_itor_t
	router_itor->cnx_dolink(this);
}

/** \brief Destructor
 */
router_itor_cnx_t::~router_itor_cnx_t()	throw()
{
	// unlink this cnx_t from the router_itor_t
	router_itor->cnx_unlink(this);
	// delete socket_client_t if needed
	nipmem_zdelete	socket_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup the router
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
router_err_t router_itor_cnx_t::start(const socket_addr_t &remote_oaddr, router_itor_cnx_cb_t *callback
							, void *userptr)	throw()
{
	router_peer_t *	router_peer	= router_itor->router_peer;
	// log to debug
	KLOG_DBG("enter with remote_addr=" << remote_oaddr);
	
	// copy some parameter
	this->callback	= callback;
	this->userptr	= userptr;
	
	/******* setup the socket_profile_t	*******************************/
	socket_profile_t	socket_profile(socket_domain_t::NTLAY);
	socket_profile_ntlay_t&	profile_dom	= socket_profile_ntlay_t::from_socket(socket_profile);
	profile_dom.scnx().ident_privkey	( router_peer->lident().privkey()		);
	profile_dom.scnx().ident_cert		( router_peer->lident().cert() 			);
	profile_dom.scnx().scnx_auth_ftor	( scnx_auth_ftor_t(router_peer, router_itor)	);
	profile_dom.ntudp_peer			( router_peer->ntudp_peer 			);

	/******* Start the socket_client_t	*******************************/
	socket_err_t	socket_err;
	socket_client	= nipmem_new socket_client_t();
	socket_err	= socket_client->setup(socket_domain_t::NTLAY, socket_type_t::DGRAM
							, socket_addr_t(), remote_oaddr, this, NULL);
	if( socket_err.failed() )	return router_err_from_socket(socket_err);
	socket_err	= socket_client->set_profile(socket_profile).start();
	if( socket_err.failed() )	return router_err_from_socket(socket_err);

	// return no error
	return router_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the remote_addr_t for this cnx_t
 */
const socket_addr_t	router_itor_cnx_t::get_remote_oaddr()	const throw()
{
	return socket_client->get_remote_addr();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool router_itor_cnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");	
	// notify the expiration of the timeout to the delaygen
	rxmit_delaygen.notify_expiration();
	// if the delaygen is is_timedout, notify the caller of the faillure
	if( rxmit_delaygen.is_timedout() )	return notify_callback_failed();

	// build the request
	pkt_t	pkt	= build_addr_nego_request();
	// send the request
	socket_client->send( pkt );

	// set the next timer
	rxmit_timeout.change_period(rxmit_delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Packet Building
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a ADDR_NEGO_REQUEST packet
 */
pkt_t	router_itor_cnx_t::build_addr_nego_request()				throw()
{
	const router_acache_item_t *	acache_item;
	router_peer_t *		router_peer	= router_itor->router_peer;
	pkt_t			pkt;

	// put the packet type
	pkt << router_pkttype_t(router_pkttype_t::ADDR_NEGO_REQUEST);
	// try to find a acache item for this 
	acache_item	= router_peer->acache().find_by_remote_dnsname(router_itor->remote_dnsname());
	// if there is a acache_item, use it to set the itor_addr_request/resp_addr_request
	if( acache_item ){
		pkt << acache_item->local_iaddr();
		pkt << acache_item->remote_iaddr();
	}else{	// if there are no acache_item, set the itor_addr_request/resp_addr_request to null
		pkt << ip_addr_t();
		pkt << ip_addr_t();
	}

	// put the available ip_addr_t
	pkt << router_peer->get_avail_iaddr();
	// return the packet
	return pkt;	
}

/** \brief Build a ADDR_NEGO_ACK with the itor_addr_ack/resp_addr_ack
 */
pkt_t	router_itor_cnx_t::build_addr_nego_ack(const ip_addr_t &itor_addr_ack
					, const ip_addr_t &resp_addr_ack)	throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << router_pkttype_t(router_pkttype_t::ADDR_NEGO_ACK);
	// put the acked ip_addr_t for itor
	pkt << itor_addr_ack;
	// put the acked ip_addr_t for resp
	pkt << resp_addr_ack;
	// return the packet
	return pkt;	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_client_t to provide event
 */
bool	router_itor_cnx_t::neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
						, const socket_event_t &socket_event) throw()
{
	const router_profile_t &profile	= router_itor->router_peer->get_profile();
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( socket_event.is_client_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() )	return notify_callback_failed();

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:
			// init delaygen for the packet rxmit
			rxmit_delaygen = delaygen_t(profile.addrnego_delaygen());
			// start the timer immediatly
			rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
			break;
	case socket_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::NEW_MTU:
			// TODO FIXME to look at i am not sure about the mtu discovery state
			// - it depends on the socket_t semantique in fact
			DBG_ASSERT( 0 );
	default:	DBG_ASSERT(0);
	}

	// return tokeep
	return true;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the socket_client_t
 */
bool	router_itor_cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	router_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype (without consuming)	
		pkt.unserial_peek( pkttype );
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case router_pkttype_t::ADDR_NEGO_REPLY:	return recv_addr_nego_reply(pkt);
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

/** \brief handle the reception of a ADDR_NEGO_REPLY
 * 
 * @return a 'tokeep/dontkeep' for the socket_client_t
 */
bool	router_itor_cnx_t::recv_addr_nego_reply(pkt_t &pkt)		throw(serial_except_t)
{
	router_peer_t *		router_peer	= router_itor->router_peer;
	pkt_t			estapkt_in	= pkt;
	ip_addr_inval_t		avail_addr	= router_peer->get_avail_iaddr();
	router_pkttype_t	pkttype;
	ip_addr_t		itor_addr_reply;
	ip_addr_t		resp_addr_reply;
	// log to debug
	KLOG_DBG("enter");
	// read the packet type
	pkt >> pkttype;
	// read the itor_addr_reply
	pkt >> itor_addr_reply;
	// read the resp_addr_reply
	pkt >> resp_addr_reply;

	// if the inner addresses are no more available, restart the negociation from the begining
	if( !avail_addr.contain(itor_addr_reply) || !avail_addr.contain(resp_addr_reply) ){
		// reset the rxmit_delaygen
		rxmit_delaygen.reset();
		// start the timer immediatly
		rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
		// return tokeep
		return true;
	}

	// build the ADDR_NEGO_ACK
	pkt_t	estapkt_out	= build_addr_nego_ack(itor_addr_reply, resp_addr_reply);
	// build and send the ADDR_NEGO_ACK
	socket_client->send( estapkt_out );

	// steal the socket_full_t from the socket_client_t - after this the socket_client_t MUST be deleted
	socket_full_t *	socket_full_stolen	= socket_client->steal_full();
	// delete the socket_client and mark it unused
	nipmem_zdelete	socket_client;
	// notify the sucess to the caller
	notify_callback(socket_full_stolen, itor_addr_reply, resp_addr_reply, estapkt_in, estapkt_out);
	// return dontkeep as the socket_client_t has been deleted 
	return false;
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
bool 	router_itor_cnx_t::notify_callback(socket_full_t *socket_full, const ip_addr_t &local_addr
					, const ip_addr_t &remote_addr
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_router_itor_cnx_cb(userptr, *this, socket_full
						, local_addr, remote_addr
						, estapkt_in, estapkt_out);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - specific to router_itor_cnx_t, the object MUST be deleted during notification
	DBG_ASSERT( tokeep == false );	
	// return the tokeep
	return tokeep;
}

/** \brief notify the caller callback in case of faillure
 */
bool 	router_itor_cnx_t::notify_callback_failed()	throw()
{
	return notify_callback(NULL, ip_addr_t(), ip_addr_t(), pkt_t(), pkt_t() );
}

/** \brief notify the caller callback in case of succeed
 */
bool 	router_itor_cnx_t::notify_callback_succeed(socket_full_t *socket_full
					, const ip_addr_t &local_addr, const ip_addr_t &remote_addr
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	return notify_callback(socket_full, local_addr, remote_addr, estapkt_in, estapkt_out);
}

NEOIP_NAMESPACE_END

