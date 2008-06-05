/*! \file
    \brief Definition of the kad_store_rpc_t

\par Brief Description
\ref kad_store_rpc_t implements a STORE rpc.

*/

/* system include */
/* local include */
#include "neoip_kad_store_rpc.hpp"
#include "neoip_kad_client_rpc.hpp"
#include "neoip_kad_peer.hpp"
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
kad_store_rpc_t::kad_store_rpc_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero the field
	kad_peer	= NULL;
	client_rpc	= NULL;
}

/** \brief Desstructor
 */
kad_store_rpc_t::~kad_store_rpc_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the kad_client_rpc_t if needed
	if( client_rpc )	nipmem_delete client_rpc;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                    Setup function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
kad_err_t	kad_store_rpc_t::start(kad_peer_t *kad_peer
				, const kad_recdups_t &kad_recdups, const cookie_id_t &cookie_id
				, const kad_addr_t &remote_addr, const delay_t &expire_delay
				, kad_store_rpc_cb_t *callback, void *userptr) 		throw()
{
	kad_pkttype_t	pkttype(kad_peer->get_profile().pkttype());	
	kad_err_t	kad_err;
	// copy some parameters
	this->kad_peer	= kad_peer;
	this->callback	= callback;
	this->userptr	= userptr;

	// build the packet to send
	pkt_t		request_payload;
	request_payload	= build_request_payload(kad_recdups, cookie_id);

	// create the kad_client_t and start it
	client_rpc	= nipmem_new kad_client_rpc_t();
	kad_err 	= client_rpc->start(kad_peer, request_payload
					, pkttype.STORE_REQUEST(), pkttype.STORE_REPLY()
					, remote_addr, expire_delay, this);
	if( kad_err.failed() )	return kad_err;

	// update the statistic
	kad_peer->get_stat()->nb_store_sent++;
	
	// return no error
	return kad_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Packet Building
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Build the request payload (aka without the packet header)
 */
pkt_t	kad_store_rpc_t::build_request_payload(const kad_recdups_t &kad_recdups
						, const cookie_id_t &cookie_id)	const throw()
{
	const kad_rpc_profile_t &	profile = kad_peer->get_profile().rpc();	
	pkt_t				pkt;
	// put the records to publish
	pkt << kad_recdups;
	// put the cookie_id - if cookie_store_ok()
	if( profile.cookie_store_ok() )
		pkt << cookie_id;
	// return the just built payload
	return pkt;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Packet Reception
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Called by kad_client_rpc_t to notify a reply payload
 */
kad_event_t	kad_store_rpc_t::reply_payload_cb(pkt_t &pkt)	throw()
{
	// notify the success of store
	return kad_event_t::build_completed();
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
bool 	kad_store_rpc_t::notify_event(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_store_rpc_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END



