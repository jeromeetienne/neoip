/*! \file
    \brief Definition of the kad_ping_rpc_t

\par Brief Description
\ref kad_ping_rpc_t implements a PING rpc.

*/

/* system include */
/* local include */
#include "neoip_kad_ping_rpc.hpp"
#include "neoip_kad_client_rpc.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_listener.hpp"	// to notify the echoed_local_ipaddr()
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
kad_ping_rpc_t::kad_ping_rpc_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero the field
	kad_peer	= NULL;
	m_client_rpc	= NULL;
}

/** \brief Desstructor
 */
kad_ping_rpc_t::~kad_ping_rpc_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the kad_client_rpc_t if needed
	nipmem_zdelete	m_client_rpc;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                    Setup function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
kad_err_t	kad_ping_rpc_t::start(kad_peer_t *kad_peer, const kad_addr_t &remote_addr
				, const delay_t &expire_delay
				, kad_ping_rpc_cb_t *callback, void *userptr) 		throw()
{
	kad_pkttype_t	pkttype(kad_peer->get_profile().pkttype());
	kad_err_t	kad_err;
	// copy some parameters
	this->kad_peer	= kad_peer;
	this->callback	= callback;
	this->userptr	= userptr;

	// build the packet to send
	pkt_t		request_payload;
	request_payload	= build_request_payload();

	// create the kad_client_t and start it
	m_client_rpc	= nipmem_new kad_client_rpc_t();
	kad_err 	= m_client_rpc->start(kad_peer, request_payload
						, pkttype.PING_REQUEST(), pkttype.PING_REPLY()
						, remote_addr, expire_delay, this);
	if( kad_err.failed() )	return kad_err;

	// update the statistic
	kad_peer->get_stat()->nb_ping_sent++;

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
pkt_t	kad_ping_rpc_t::build_request_payload()	const throw()
{
	// return an empty packet as PING_REQUEST has no payload
	return pkt_t();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Packet Reception
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Called by kad_client_rpc_t to notify a reply payload
 */
kad_event_t	kad_ping_rpc_t::reply_payload_cb(pkt_t &pkt)	throw()
{
	// parse the incoming reply payload
	try {
		// read the echoed m_echoed_local_ipaddr
		// - NOTE: it serves as echoed_local_ipaddr discovery	
		pkt >> m_echoed_local_ipaddr;
	}catch(serial_except_t &e){
		KLOG_ERR("Cant parse incoming packet due to " << e.what() );
		// return a null kad_event_t
		return kad_event_t();
	}
	// log to debug
	KLOG_DBG("echoed_local_ipaddr=" << echoed_local_ipaddr() );
	
	// notify the kad_listener_t of the echoed_local_ipaddr()
	// - this is ok to notify this function here as it is not doing any
	//   callback notification.
	kad_listener_t*	kad_listener	= kad_peer->get_listener();
	kad_listener->notify_echoed_local_ipaddr(echoed_local_ipaddr(), client_rpc()->get_remote_addr());
	
	// return a kad_event_t::COMPLETED
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
bool 	kad_ping_rpc_t::notify_event(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_ping_rpc_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END



