/*! \file
    \brief Definition of the ntudp_resp_direct_t

\par Brief description
This modules handle the responder part for the direct connection establishment.

*/

/* system include */
/* local include */
#include "neoip_ntudp_resp_direct.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_udp_full.hpp"
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
ntudp_resp_direct_t::ntudp_resp_direct_t(ntudp_resp_t *ntudp_resp
				, const pkt_t &pkt_request, udp_full_t *udp_full
				, ntudp_resp_direct_cb_t *callback, void * userptr)	throw()
{
	// copy the parameter
	this->ntudp_resp	= ntudp_resp;
	this->pkt_request	= pkt_request;
	this->callback		= callback;
	this->userptr		= userptr;
	// set the callback for the udp_full
	this->udp_full		= udp_full;
	inet_err_t	inet_err= udp_full->set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
	// launch the zerotimer
	zerotimer.append(this, NULL);
	// link this object to the ntudp_resp_t
	ntudp_resp->direct_link(this);
}

/** \brief Desstructor
 */
ntudp_resp_direct_t::~ntudp_resp_direct_t()		throw()
{
	// unlink this object from the ntudp_resp_t
	ntudp_resp->direct_unlink(this);
	// delete the udp_full_t if needed
	if( udp_full )	nipmem_delete	udp_full;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       delivery_zerotimer callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	ntudp_resp_direct_t:: neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	pkt_t			orig_pkt	= pkt_request;
	ntudp_peer_t *		ntudp_peer	= ntudp_resp->ntudp_peer;
	ntudp_sock_pkttype_t	pkttype;
	ntudp_peerid_t		src_peerid, dst_peerid;
	ntudp_portid_t		src_portid, dst_portid;
	// parse the packet request
	try {
		pkt_request >> pkttype;
		pkt_request >> client_nonce;
		pkt_request >> dst_peerid >> dst_portid;
		pkt_request >> src_peerid >> src_portid;
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
		// notify the faillure to the caller
		return notify_callback(NULL, pkt_t(), pkt_t());
	}
	
	// sanity check - pkttype MUST be DIRECT_CNX_REQUEST as the packet triggered a ntudp_resp_direct_t
	DBG_ASSERT( pkttype == ntudp_sock_pkttype_t::DIRECT_CNX_REQUEST );

	// set the local/remote ntudp_addr_t from the peerid/portid of the packet
	m_local_addr	= ntudp_addr_t(dst_peerid, dst_portid);
	m_remote_addr	= ntudp_addr_t(src_peerid, src_portid);

	// if the cnxaddr is already bound, reply an error packet, and notify the faillure to the caller
	if( ntudp_peer->cnxaddr_is_bound(local_addr(), remote_addr()) ){
		// send back a error packet to the remote peer
		pkt_t pkt_err = ntudp_sock_errcode_build_pkt(ntudp_sock_errcode_t::ALREADY_BOUND, client_nonce);
		udp_full->send( pkt_err );
		// notify the faillure to the caller
		return notify_callback(NULL, pkt_t(), pkt_t());
	}

	 
	// build and send back a DIRECT_CNX_REPLY
	pkt_t pkt_reply	= build_direct_cnx_reply();
	udp_full->send( pkt_reply );
	
	// steal the udp_full_t from ntudp_resp_direct_t to pass it to the caller
	udp_full_t *	udp_full_stolen	= udp_full;
	udp_full	= NULL;
	
	// notify the caller
	return notify_callback(udp_full_stolen, orig_pkt, pkt_reply);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when a connection is established
 */
bool	ntudp_resp_direct_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( udp_event.is_client_ok() );
	
	// handle the fatal events
	if( udp_event.is_fatal() ){
		// log the event
		KLOG_INFO("received fatal event on " << *udp_full );
		// notify the faillure
		return notify_callback(NULL, pkt_t(), pkt_t());
	}

	// NOTE: other udp_event_t are ignored as this callback is only a place holder for the 
	//       udp_full_t during the zerotimer_t

	// return 'tokeep'
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the DIRECT_CNX_REPLY
 */
pkt_t ntudp_resp_direct_t::build_direct_cnx_reply()	const throw()
{
	pkt_t		pkt;
	// sanity check - the client_nonce MUST NOT be null
	DBG_ASSERT( !client_nonce.is_null() );
	// put the packet type
	pkt << ntudp_sock_pkttype_t(ntudp_sock_pkttype_t::DIRECT_CNX_REPLY);
	// put the nonce
	pkt << client_nonce;
	// return the packet
	return pkt;
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
bool 	ntudp_resp_direct_t::notify_callback(udp_full_t *udp_full, const pkt_t &estapkt_in
						, const pkt_t &estapkt_out)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_resp_direct_cb(userptr, *this, udp_full
							, estapkt_in, estapkt_out);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - specific to ntudp_resp_direct_t, the object MUST be deleted during notification
	DBG_ASSERT( tokeep == false );
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


