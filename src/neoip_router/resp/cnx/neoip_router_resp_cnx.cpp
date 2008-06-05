/*! \file
    \brief Definition of the router_resp_t

*/

/* system include */
/* local include */
#include "neoip_router_resp_cnx.hpp"
#include "neoip_router_full.hpp"
#include "neoip_router_pkttype.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_resp_cnx_t::router_resp_cnx_t(router_resp_t *router_resp)	throw()
{
	// copy the parameters
	this->router_resp	= router_resp;
	// link this cnx_t to the router_resp_t
	router_resp->cnx_link(this);
}

/** \brief Destructor
 */
router_resp_cnx_t::~router_resp_cnx_t()	throw()
{
	// unlink this cnx_t from the router_resp_t
	router_resp->cnx_unlink(this);
	// delete the socket_full_t if needed
	nipmem_zdelete socket_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup the router
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
router_err_t router_resp_cnx_t::start(socket_full_t *socket_full)	throw()
{
	const router_profile_t &profile	= router_resp->router_peer->get_profile();
	// log to debug
	KLOG_ERR("enter");
	// copy the parameter
	this->socket_full	= socket_full;
	// start the socket_full
	socket_err_t	socket_err;
	socket_err	= socket_full->start(this, NULL);	
	if( socket_err.failed() )	return router_err_from_socket(socket_err);

	// start the expire_timeout
	expire_timeout.start(profile.resp_cnx_timeout(), this, NULL);

	// return no error
	return router_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool router_resp_cnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	if( &cb_timeout == &expire_timeout )	return expire_timeout_cb(userptr, cb_timeout);
	if( &cb_timeout == &rxmit_timeout )	return rxmit_timeout_cb(userptr, cb_timeout);
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );	
	return false;
}

/** \brief callback called when the neoip_timeout expire
 */
bool router_resp_cnx_t::expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// autodelete
	nipmem_delete this;
	// return 'dontkeep'
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool router_resp_cnx_t::rxmit_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// notify the expiration of the timeout to the delaygen
	rxmit_delaygen.notify_expiration();
	// TODO do i need this delaygen timeout as the whole router_resp_cnx_t already has a expire_timeout
	// - this duplication may be confusing and lead to error
	// if the delaygen is is_timedout, autodelete this cnx
	if( rxmit_delaygen.is_timedout() ){
		nipmem_delete this;
		return false;
	}

	// build the request
	pkt_t	pkt	= build_addr_nego_reply();
	// send the request
	socket_full->send( pkt );

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

/** \brief Build a ADDR_NEGO_REPLY with the chosen_addr
 */
pkt_t	router_resp_cnx_t::build_addr_nego_reply()	throw()
{
	pkt_t	pkt;
	// sanity check - the chosen_itor/resp_iaddr MUST be non null
	DBG_ASSERT( !itor_iaddr_reply.is_null() );
	DBG_ASSERT( !resp_iaddr_reply.is_null() );
	// put the packet type
	pkt << router_pkttype_t(router_pkttype_t::ADDR_NEGO_REPLY);
	// put the itor_iaddr_reply
	pkt << itor_iaddr_reply;
	// put the resp_iaddr_reply
	pkt << resp_iaddr_reply;
	// return the packet
	return pkt;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                socket_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when to notify an event
 */
bool	router_resp_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	KLOG_ERR("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );
	
	// if the socket_event_t is fatal, autodelete this router_resp_cnx_t
	if( socket_event.is_fatal() ){
		nipmem_delete	this;
		return false;
	}
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::NEW_MTU:
			// TODO FIXME to look at i am not sure about the mtu discovery state
			// - it depends on the socket_t semantique in fact
			DBG_ASSERT( 0 );		
	default:	DBG_ASSERT(0);
	}	
	// return 'tokeep'
	return true;	
}


/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	router_resp_cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	router_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype (without consuming)	
		pkt.unserial_peek( pkttype );
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case router_pkttype_t::ADDR_NEGO_REQUEST:	return recv_addr_nego_request(pkt);
		case router_pkttype_t::ADDR_NEGO_ACK:		return recv_addr_nego_ack(pkt);
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

/** \brief Pick a inner ip address from the available one (may return null if none is available)
 */
ip_addr_t	router_resp_cnx_t::pick_inner_addr(const ip_addr_inval_t &avail_iaddr
					, const ip_addr_t &prefered_iaddr)	throw()
{
	// if no address is available, return a null ip_addr_t
	if( avail_iaddr.size() == 0 )	return ip_addr_t();
	
	// if the prefered address is not null and is contained in avail_iaddr, pick this one
	if( !prefered_iaddr.is_null() && avail_iaddr.contain(prefered_iaddr) )
		return prefered_iaddr;

	// TODO find a better algorithm 
	// - this one is too deterministic and will cause negociation retry when 2 tunnel are established
	//   at the same time. 
	// - to pick one at random on the first item? seems better
	//   - do i have the ip_addr_t arithmetic operator to do it ?
	
	// return the lowest of the interval
	return avail_iaddr[0].min_value();
}

/** \brief handle the reception of a ADDR_NEGO_REQUEST
 * 
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	router_resp_cnx_t::recv_addr_nego_request(pkt_t &pkt)		throw(serial_except_t)
{
	router_peer_t *		router_peer	= router_resp->router_peer;	
	const router_profile_t &profile		= router_peer->get_profile();
	router_pkttype_t	pkttype;
	ip_addr_inval_t		avail_iaddr;
	ip_addr_t		itor_iaddr_request;
	ip_addr_t		resp_iaddr_request;
	// log to debug
	KLOG_DBG("enter");
	// read the packet type
	pkt >> pkttype;
	// read the itor_iaddr_request
	pkt >> itor_iaddr_request;
	// read the resp_iaddr_request
	pkt >> resp_iaddr_request;	
	// read the available inner address for the remote peer
	pkt >> avail_iaddr;

	// log to debug
	KLOG_DBG("itor_iaddr_request="	<< itor_iaddr_request);
	KLOG_DBG("resp_iaddr_request="	<< resp_iaddr_request);
	KLOG_DBG("avail_iaddr=" 	<< avail_iaddr );
	
	// remove the locally used ip_addr_t from the available from the remote peer
	avail_iaddr -= router_peer->get_used_iaddr();
	// log to debug
	KLOG_DBG("avail_iaddr=" << avail_iaddr );

	// pick the itor_iaddr_reply
	itor_iaddr_reply	= pick_inner_addr(avail_iaddr, itor_iaddr_request);
	// remove the itor_iaddr_reply from the available inner ip address
	if( !itor_iaddr_reply.is_null() )	avail_iaddr -= ip_addr_inval_item_t(itor_iaddr_reply);
	// pick the resp_iaddr_reply
	resp_iaddr_reply	= pick_inner_addr(avail_iaddr, resp_iaddr_request);

	// log to debug
	KLOG_DBG("itor_iaddr_reply=" << itor_iaddr_reply);
	KLOG_DBG("resp_iaddr_reply=" << resp_iaddr_reply);

	// if the inner address have not been found, autodelete the connection
	if( itor_iaddr_reply.is_null() || resp_iaddr_reply.is_null() ){
		nipmem_delete this;
		return false;
	}

	// init delaygen for the ADDR_NEGO_ACK rxmit
	rxmit_delaygen = delaygen_t(profile.addrnego_delaygen());
	// start the timer immediatly
	rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);

	// return tokeep
	return true;	
}

/** \brief handle the reception of a ADDR_NEGO_ACK
 * 
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	router_resp_cnx_t::recv_addr_nego_ack(pkt_t &pkt)		throw(serial_except_t)
{
	router_peer_t *		router_peer	= router_resp->router_peer;	
	router_pkttype_t	pkttype;
	ip_addr_t		itor_iaddr_ack;
	ip_addr_t		resp_iaddr_ack;
	// log to debug
	KLOG_DBG("enter");
	// read the packet type
	pkt >> pkttype;
	// read the itor_iaddr_reply
	pkt >> itor_iaddr_ack;
	// read the resp_iaddr_reply
	pkt >> resp_iaddr_ack;	
	
	// if the acked_itor/resp_iaddr are different from the chosen ones, autodelete this cnx
	// NOTE: due to packet reordering, it is possible to received acked_addr != from chosen_addr
	if( itor_iaddr_ack != itor_iaddr_reply || resp_iaddr_ack != resp_iaddr_reply ){
		// log the event_t
		KLOG_ERR("The chosen itor/resp addresses have changed *during* the negociation. closing the connection.");
		// autodelete
		nipmem_delete this;
		return false;
	}

	// backup the object_slotid of the socket_full_t - to be able to return its tokeep value
	slot_id_t	socket_full_slotid	= socket_full->get_object_slotid();
		
	// steal the socket_full
	socket_full_t *	socket_full_stolen	= socket_full;
	socket_full	= NULL;

	// spawn a router_full_t from the socket_full_t and the chosen_itor/resp_iaddr
	router_full_t *	router_full;
	router_err_t	router_err;
	router_full	= nipmem_new router_full_t(router_peer);
	router_err	= router_full->start(socket_full_stolen, resp_iaddr_reply, itor_iaddr_reply
						, pkt_t(), pkt_t() );
	if( router_err.failed() ){
		KLOG_ERR("Cant create a router_full_t due to " << router_err);
		nipmem_delete router_full;
	}

	// autodelete this connection
	nipmem_delete	this;

	// compute the socket_full_t tokeep from its slotid as it may and may not be delete here
	return object_slotid_tokeep(socket_full_slotid);
}

NEOIP_NAMESPACE_END

