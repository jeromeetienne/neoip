/*! \file
    \brief Definition of the ntudp_npos_saddrecho_t

\par Brief description
This modules does a 'source address echo' aka it sends packet to a server 
which replies the source address as it saw it.
- this is used in the ntudp_npos_natted() and other places

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_ntudp_npos_saddrecho_wikidbg.hpp"
#include "neoip_ntudp_npos_pkttype.hpp"
#include "neoip_ntudp_npos_event.hpp"
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
ntudp_npos_saddrecho_t::ntudp_npos_saddrecho_t()		throw()
{
	// allocate the udp_client
	udp_client	= nipmem_new udp_client_t();
	// setup the nonce
	request_nonce	= ntudp_nonce_t::build_random();
}

/** \brief Desstructor
 */
ntudp_npos_saddrecho_t::~ntudp_npos_saddrecho_t()		throw()
{
	// delete the udp_client if needed
	if(udp_client)	nipmem_delete	udp_client;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      set parameter
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_npos_saddrecho_t &ntudp_npos_saddrecho_t::set_profile(const ntudp_npos_cli_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Set the local address
 */
ntudp_err_t	ntudp_npos_saddrecho_t::set_local_addr(const ipport_addr_t &local_addr)	throw()
{
	inet_err_t	inet_err;
	inet_err	= udp_client->set_local_addr(local_addr);
	if( inet_err.failed() )	return ntudp_err_from_inet(inet_err);
	return ntudp_err_t::OK;
}


/** \brief Set a new callback to ntudp_npos_saddrecho_t
 * 
 * - this function may be used even once the object is started. this is used
 *   when the ntudp_npos_saddrecho_t changes owner. 
 *   - e.g. in ntudp_aview_pool_t providing ntudp_npos_saddrecho_t
 */
void	ntudp_npos_saddrecho_t::set_callback(ntudp_npos_saddrecho_cb_t *callback, void * userptr)
										throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         start functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the actions
 */
ntudp_err_t ntudp_npos_saddrecho_t::start(const ipport_addr_t &pserver_addr
					, ntudp_pserver_pool_t *pserver_pool
					, ntudp_npos_saddrecho_cb_t *callback, void * userptr)	throw()
{
	// copy the parameter
	this->pserver_pool	= pserver_pool;
	this->callback		= callback;
	this->userptr		= userptr;
	// start the udp_client_t
	inet_err_t	inet_err;
	inet_err	= udp_client->start(pserver_addr, this, NULL);
	if( inet_err.failed() )	return ntudp_err_from_inet(inet_err);
	// return no error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the local address local/public view
 */
ipport_aview_t	ntudp_npos_saddrecho_t::local_addr_aview()	const throw()
{
	// build the result
	ipport_aview_t	ipport_aview(udp_client->get_local_addr(), local_addr_pview);
	// sanity check - the ipport_aview_t MUST be fully_qualified
	DBG_ASSERT( ipport_aview.is_fully_qualified() );
	// return the result
	return ipport_aview;
}

/** \brief Return the remote address
 */
ipport_addr_t	ntudp_npos_saddrecho_t::get_pserver_addr()	const throw()
{
	return udp_client->get_remote_addr();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool ntudp_npos_saddrecho_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("Send a SADDRECHO request");

	// notify the expiration of the timeout to the delaygen
	rxmit_delaygen.notify_expiration();
	// if the delaygen is is_timedout, notify an TIMEDOUT 
	if( rxmit_delaygen.is_timedout() ){
		// notify that pserver_addr as timedout
		pserver_pool->notify_timedout(get_pserver_addr(), rxmit_delaygen.get_elapsed_delay() 
								- rxmit_delaygen.get_arg().first_delay());
		// notify a TIMEDOUT event
		return notify_callback(ntudp_npos_event_t::build_timedout());
	}

	// build the request
	pkt_t	pkt	= build_pkt_request();
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

/** \brief build the advertisement packet
 */
pkt_t ntudp_npos_saddrecho_t::build_pkt_request()	const throw()
{
	pkt_t	pkt;
	// put the packet type
	pkt << ntudp_npos_pkttype_t(ntudp_npos_pkttype_t::SADDRECHO_REQUEST);
	// put the nonce
	pkt << request_nonce;
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
bool	ntudp_npos_saddrecho_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
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
		// notify that the remote_addr is unreachable
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
bool	ntudp_npos_saddrecho_t::handle_recved_data(pkt_t &pkt)	throw()
{
	ntudp_npos_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the pkttype
		pkt >> pkttype;
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case ntudp_npos_pkttype_t::SADDRECHO_REPLY:	return recv_saddrecho_reply(pkt);
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

/** \brief Return true if the client is in itor phase, false otherwise (aka when it 
 *         is in the full phase)
 */
bool	ntudp_npos_saddrecho_t::phase_is_itor()	const throw()
{
	// if the phase is full, return false
	if( !local_addr_pview.is_null() )	return false;
	// return true
	return true;
}

/** \brief handle the reception of a SADDRECHO_REPLY
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_npos_saddrecho_t::recv_saddrecho_reply(pkt_t &pkt)		throw(serial_except_t)
{
	ntudp_nonce_t	echoed_nonce;
	ipport_addr_t	echoed_saddr;
	// log to debug
	KLOG_DBG("enter");
	// read the echoed nounce
	pkt >> echoed_nonce;
	// read the echoed saddr from the packet
	pkt >> echoed_saddr;

	// check if the echoed_nonce is equal to the request_nonce
	if( echoed_nonce != request_nonce ){
		// log the event
		KLOG_ERR("Received a echoed_nonce different from the expected one! ignoring the packet");
		// return a 'tokeep' - to accept other reply
		return true;
	}
	// NOTE: if this point is reached, the 'connection' is established
	
	// notify the remote_addr as reachable
	pserver_pool->notify_reach( get_pserver_addr() );
	
	// if the echoed_saddr address has changed during the full phase, report a NETWORK_ERROR
	// - this is likely due to a address reassignement on the NAT box
	if( !phase_is_itor() && local_addr_pview != echoed_saddr ){
		std::string reason	= "local_addr_pview changed. likely a NAT box address reassignment";
		return notify_callback( ntudp_npos_event_t::build_network_error(reason) );
	}
	
	// switch to the full phase if not there already
	if( phase_is_itor() ){
		// copy the echoed_saddr
		local_addr_pview= echoed_saddr;
		// init delaygen for the FULL phase
		rxmit_delaygen 	= delaygen_t(profile.cli_full_pkt_rxmit());
		// restart the timer immediatly with it
		rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
	}
	// sanity check - at this point the phase is no more itor
	DBG_ASSERT( !phase_is_itor() );

	// build and notify the event
	return notify_callback( ntudp_npos_event_t::build_completed(true) );
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
bool 	ntudp_npos_saddrecho_t::notify_callback(const ntudp_npos_event_t &ntudp_npos_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_npos_saddrecho_event_cb(userptr, *this, ntudp_npos_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


