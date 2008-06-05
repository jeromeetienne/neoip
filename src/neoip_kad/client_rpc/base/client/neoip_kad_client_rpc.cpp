/*! \file
    \brief Declaration of the kad_client_rpc_t

\par Brief Description
\ref kad_client_rpc_t implement the common part to all the specific client RPC
- handling the request_nonce
- the packet header building/parsing
- the connection and packet retransmittion are handled by kad_clicnx_t
- the kbucket update at each received message and to notify the timeout faillure

*/

/* system include */
/* local include */
#include "neoip_kad_client_rpc.hpp"
#include "neoip_kad_clicnx.hpp"
#include "neoip_kad_rpc_common.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_client_rpc_t::kad_client_rpc_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero the field
	kad_peer	= NULL;
	kad_clicnx	= NULL;
}

/** \brief Desstructor
 */
kad_client_rpc_t::~kad_client_rpc_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete kad_clicnx_t if needed
	if( kad_clicnx )	nipmem_delete kad_clicnx;

}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                    Setup function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
kad_err_t	kad_client_rpc_t::start(kad_peer_t *kad_peer, const pkt_t &request_payload
					, const kad_pkttype_t &request_pkttype
					, const kad_pkttype_t &reply_pkttype
					, const kad_addr_t &remote_addr, const delay_t &expire_delay
					, kad_client_rpc_vapi_t *callback) 		throw()
{
	kad_err_t	kad_err;
	// sanity check - the remote_addr.oaddr() MUST be fully_qualified
	// - but the kad_peerid_t may be null
	DBG_ASSERT( remote_addr.oaddr().is_fully_qualified() );
	// copy some parameters
	this->kad_peer		= kad_peer;
	this->remote_addr	= remote_addr;
	this->reply_pkttype	= reply_pkttype;
	this->callback		= callback;
	// draw the random nonce
	request_nonce	= kad_nonceid_t::build_random();
	// set the creation date
	creation_date	= date_t::present();

	// build the request_header
	pkt_t		request_header;
	request_header	= kad_rpc_common_t::build_req_header(kad_peer, remote_addr, request_pkttype
										, request_nonce);
	// build the request packet from the header and the payload
	pkt_t		request_pkt;
	request_pkt.append(request_header.to_datum());				
	request_pkt.append(request_payload.to_datum());				

	// create the kad_clicnx and start it
	const kad_clicnx_profile_t &	clicnx_profile = kad_peer->get_profile().clicnx();
	kad_clicnx	= nipmem_new kad_clicnx_t();
	kad_err 	= kad_clicnx->set_profile(clicnx_profile).start(kad_peer,request_pkt, expire_delay
					, remote_addr.get_oaddr(), kad_peer->get_stat(), this, NULL );
	if( kad_err.failed() )	return kad_err;

	// return no error
	return kad_err_t::OK;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                  kad_clicnx_t callback
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_clicnx_t receive a packet
 */
bool	kad_client_rpc_t::neoip_kad_clicnx_cb(void *cb_userptr, kad_clicnx_t &cb_kad_clicnx
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_DBG("received event=" << kad_event );
	// sanity check - the event MUST be cnx_ok()
	DBG_ASSERT( kad_event.is_cnx_ok() );

	// if the event is fatal, simply forward it
	if( kad_event.is_fatal() ){
		// notify the kbucket thus it may keep its state fresh
		kad_kbucket_t *	kad_kbucket	= kad_peer->get_kbucket();
		delay_t		elapsed_delay	= date_t::present() - creation_date;
		kad_kbucket->notify_failed_rpc(remote_addr.get_peerid(), kad_event, elapsed_delay);
		// notify the caller
		return notify_callback(kad_event);
	}
	
	// sanity check - here the event MUST be is_recved_data();
	DBG_ASSERT( kad_event.is_recved_data() );
	// parse the reply
	return handle_reply( *kad_event.get_recved_data() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          packet reception
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle reply
 */
bool	kad_client_rpc_t::handle_reply(pkt_t &pkt)		throw()
{
	const kad_rpc_profile_t &	profile = kad_peer->get_profile().rpc();
	kad_pkttype_t	pkttype;
	kad_nonceid_t	incoming_nonce;
	kad_addr_t	source_addr;
	kad_err_t	kad_err;
	
	// parse the reply header
	kad_err	= kad_rpc_common_t::parse_rep_header(kad_peer, pkt, pkttype
						, source_addr, incoming_nonce);
	if( kad_err.failed() ){
		KLOG_ERR("Cant parse incoming packet due to " << kad_err );
		return true;
	}

	// check that both nonces match - if req_nonce_in_pkt_ok()
	if( profile.req_nonce_in_pkt_ok() && incoming_nonce != request_nonce ){
		KLOG_INFO("Received a reply with a invalid nonce");
		return true;
	}
	
	// if the reply type doesnt match the request type, exit
	if( pkttype != reply_pkttype ){
		KLOG_ERR("Received an invalue packet type");
		return true;
	}
	
	// update the source_node in the kbucket
	// - in paper section 2.1, "When a Kademlia node receives any message (re-
	//   quest or reply) from another node, it updates the appropriate k-bucket 
	//   for the sender’s node ID."
	kad_peer->srckaddr_parse(source_addr, kad_clicnx->get_remote_oaddr());

	// notify the caller of this reply payload
	kad_event_t	kad_event;
	kad_event	= callback->reply_payload_cb(pkt);
	
	// if the returned event is null, do nothing
	if( kad_event.is_null() )	return true;
	
	// if the returned kad_event_t is non null, notify it to the caller
	return notify_callback(kad_event);
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
bool 	kad_client_rpc_t::notify_callback(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->notify_event(kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END



