/*! \file
    \brief Definition of the ntudp_npos_inetreach_t

\par Brief Description of the protocol
- the client sends INETREACH_REQUEST to the server
  - the request contains a nonce for security and a probe destination address
  - the probe destination address is the public listen address of the local ntudp_npos_server.
- the server replies a INETREACH_REPLY on the same connection
  - and send a INETREACH_PROBE to the probe destination address on another connection
- if the local ntudp_npos_server_t received the INETREACH_PROBE with the proper nonce, it is 
  considered as 'reachable from the public internet'
  - it notify a ntudp_npos_event_t::COMPLETED true
- if more than X (currently set to 3) INETREACH_REPLY is received without receiving any
  INETREACH_PROBE, it is considered as not reachable.
  - the value of X depends on the acceptable packets loss on the path client->server->client listen addr
    taking into account that there is no loss on the client->server->client on a
    single connection

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_inetreach.hpp"
#include "neoip_ntudp_npos_pkttype.hpp"
#include "neoip_ntudp_npos_event.hpp"
#include "neoip_ntudp_npos_server.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
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
ntudp_npos_inetreach_t::ntudp_npos_inetreach_t()		throw()
{
	// zero some firled
	npos_server	= NULL;
	nb_recved_reply	= 0;
	// allocate the udp_client
	udp_client	= nipmem_new udp_client_t();
	// setup the nonce
	request_nonce	= ntudp_nonce_t::build_random();
}

/** \brief Desstructor
 */
ntudp_npos_inetreach_t::~ntudp_npos_inetreach_t()		throw()
{
	// unregister the probe nonce in the ntudp_npos_server_t
	if( npos_server )	npos_server->inetreach_probe_unregister(request_nonce);

	// delete the udp_client if needed
	if(udp_client)	nipmem_delete	udp_client;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the remote address
 */
ipport_addr_t	ntudp_npos_inetreach_t::get_pserver_addr()	const throw()
{
	return udp_client->get_remote_addr();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Setup functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_npos_inetreach_t &ntudp_npos_inetreach_t::set_profile(const ntudp_npos_cli_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}
/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
ntudp_err_t ntudp_npos_inetreach_t::start(const ipport_addr_t &pserver_addr
					, const ipport_addr_t &listen_addr_pview
					, ntudp_npos_server_t *npos_server
					, ntudp_pserver_pool_t *pserver_pool
					, ntudp_npos_inetreach_cb_t *callback, void * userptr)		throw()
{
	ntudp_err_t	ntudp_err;
	// copy the field
	this->listen_addr_pview	= listen_addr_pview;
	this->npos_server	= npos_server;
	this->pserver_pool	= pserver_pool;
	this->callback		= callback;
	this->userptr		= userptr;

	// start the client toward pserver_Addr
	inet_err_t	inet_err;
	inet_err	= udp_client->start(pserver_addr, this, NULL);
	if( inet_err.failed() )	return ntudp_err_from_inet(inet_err);

	// register the probe nonce in the ntudp_npos_server_t
	npos_server->inetreach_probe_register(request_nonce, this, NULL);
	
	// return no error
	return ntudp_err_t::OK;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool ntudp_npos_inetreach_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("send a INETREACH_REQUEST");
	// notify the expiration of the timeout to the delaygen
	rxmit_delaygen.notify_expiration();
	// if the delaygen is is_timedout, notify an TIMEDOUT 
	if( rxmit_delaygen.is_timedout() ){
		// notify that the pserver_addr is timedout
		pserver_pool->notify_timedout(get_pserver_addr(), rxmit_delaygen.elapsed_delay()
							- rxmit_delaygen.get_arg().first_delay());
		// notify the event
		return notify_callback(ntudp_npos_event_t::build_timedout());
	}
	
	// build the request packet
	pkt_t	pkt	= build_pkt_request();
	// send the request packet
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

/** \brief build the advertisement packet
 */
pkt_t ntudp_npos_inetreach_t::build_pkt_request()	const throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << ntudp_npos_pkttype_t(ntudp_npos_pkttype_t::INETREACH_REQUEST);
	// put the nonce
	pkt << request_nonce;
	// put the listen_addr_pview
	pkt << listen_addr_pview;
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
bool	ntudp_npos_inetreach_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( udp_event.is_client_ok() );
	
	// handle the fatal events
	if( udp_event.is_fatal() ){
		// log the event
		KLOG_ERR("received fatal event on " << *udp_client );
		// notify that the pserver_addr is unreachable
		pserver_pool->notify_unreach( get_pserver_addr() );
		// build and notify the event
		ntudp_npos_event_t ntudp_npos_event = ntudp_npos_event_t::build_network_error(udp_event.to_string());
		return notify_callback(ntudp_npos_event);
	}

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:
			// init delaygen for the itor phase
			rxmit_delaygen = delaygen_t(profile.cli_itor_pkt_rxmit());
			// start the retransmit timer immediatly
			rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
			break;
	case udp_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*udp_event.get_recved_data());
	default:	break;
	}
	return true;
}

/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_npos_inetreach_t::handle_recved_data(pkt_t &pkt)	throw()
{
	ntudp_npos_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype
		pkt >> pkttype;
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_npos_pkttype_t::INETREACH_REPLY:
				return recv_inetreach_reply(pkt);
		default:	KLOG_ERR("Unexpected packet type " << pkttype );
		}
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
	}

	// return 'tokeep'
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle the reception of a INETREACH_REPLY
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_npos_inetreach_t::recv_inetreach_reply(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_nonce_t	echoed_nonce;
	// log to debug
	KLOG_DBG("enter");

	// read the echoed nounce
	pkt >> echoed_nonce;
	// check if the echoed_nonce is equal to the request_nonce
	if( echoed_nonce != request_nonce ){
		// log the event
		KLOG_ERR("Received a echoed_nonce different from the expected one! ignoring the packet");
		// return a 'tokeep' - to accept other reply
		return true;
	}
	
	// update the nb_recved_reply counter
	nb_recved_reply++;
	// if the nb_recved_reply reaches the maximum, notify as inetreach false
	if( nb_recved_reply > profile.max_recved_inetreach_reply() ){
		ntudp_npos_event_t ntudp_npos_event = ntudp_npos_event_t::build_completed(true);
		return notify_callback(ntudp_npos_event);
	}
	
	// notify that the pserver_addr is reachable
	pserver_pool->notify_reach( get_pserver_addr() );

	// return a 'tokeep'
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_npos_server_t when to notify an probe
 * 
 * - if a probe is received with the proper request_probe, notify a completed true
 */
void	ntudp_npos_inetreach_t::neoip_ntudp_npos_server_probe_event_cb(void *cb_userptr
					, ntudp_npos_server_t &cb_ntudp_npos_server)	throw()
{	
	// build and notify the event
	ntudp_npos_event_t ntudp_npos_event = ntudp_npos_event_t::build_completed(true);
	notify_callback(ntudp_npos_event);
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
bool 	ntudp_npos_inetreach_t::notify_callback(const ntudp_npos_event_t &ntudp_npos_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_npos_inetreach_event_cb(userptr, *this, ntudp_npos_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}
NEOIP_NAMESPACE_END


