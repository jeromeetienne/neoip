/*! \file
    \brief Definition of the kad_clicnx_t

*/

/* system include */
/* local include */
#include "neoip_kad_clicnx.hpp"
#include "neoip_kad_stat.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_udp_client.hpp"
#include "neoip_inet_err.hpp"
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
kad_clicnx_t::kad_clicnx_t()		throw()
{
	// zero some field 
	udp_client	= NULL;
	kad_stat	= NULL;
}

/** \brief Desstructor
 */
kad_clicnx_t::~kad_clicnx_t()		throw()
{
	// delete the udp_client if needed
	nipmem_zdelete udp_client;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
kad_clicnx_t &kad_clicnx_t::set_profile(const kad_clicnx_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == kad_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Constructor
 */
kad_err_t	kad_clicnx_t::start(kad_peer_t *kad_peer, const pkt_t &pkt, const delay_t &expire_delay
					, const ipport_addr_t &remote_oaddr, kad_stat_t *kad_stat
					, kad_clicnx_cb_t *callback, void *userptr)		throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("remote_oaddr=" << remote_oaddr);
	// copy the parameter
	this->kad_peer		= kad_peer;
	this->pkt_to_send	= pkt;
	this->kad_stat		= kad_stat;
	this->callback		= callback;
	this->userptr		= userptr;

	// create and start the udp_client
	udp_client	= nipmem_new udp_client_t();
	inet_err	= udp_client->start(remote_oaddr, this, NULL);
	if( inet_err.failed() )	return kad_err_from_inet(inet_err);
	
	// start the expire_timeout
	expire_timeout.start(expire_delay, this, NULL);
	
	// return no error
	return kad_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool kad_clicnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	if( &cb_timeout == &rxmit_timeout )	return rxmit_timeout_cb(userptr, cb_timeout);
	if( &cb_timeout == &expire_timeout )	return expire_timeout_cb(userptr, cb_timeout);
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief callback called when the neoip_timeout expire
 */
bool kad_clicnx_t::rxmit_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// notify the expiration of the timeout to the delaygen
	rxmit_delaygen.notify_expiration();
	// if the delaygen is is_timedout, notify the faillure
	if( rxmit_delaygen.is_timedout() )
		return notify_callback( kad_event_t::build_timedout("RPC clicnx timedout") );

	// update the statistic
	if( kad_stat )	kad_stat->nb_byte_sent += pkt_to_send.get_len();
	// send the request
	udp_client->send( pkt_to_send );

	// set the next timer
	rxmit_timeout.change_period(rxmit_delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}


/** \brief callback called when the neoip_timeout expire
 */
bool kad_clicnx_t::expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// notify the caller
	return notify_callback( kad_event_t::build_timedout("RPC clicnx timedout") );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     udp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_client_t when a connection is established
 */
bool	kad_clicnx_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( udp_event.is_client_ok() );
	
	// if a udp_event_t is fatal, notify a kad_event_t::CNX_CLOSED
	if( udp_event.is_fatal() )
		return notify_callback( kad_event_t::build_cnx_closed(udp_event.to_string()) );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:
			// init delaygen for the packet rxmit
			rxmit_delaygen	= delaygen_t(profile.rxmit_delaygen_arg());
			// start the timer immediatly
			rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
			break;
	case udp_event_t::RECVED_DATA:
			return handle_recved_data( *udp_event.get_recved_data() );
	default:	break;
	}
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			process udp_event_t from udp_client_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Process udp_event_t::RECVED_DATA
 * 
 * @return a tokeep for the udp_client_t
 */
bool	kad_clicnx_t::handle_recved_data(pkt_t &pkt)				throw()
{
	// update the statistic if needed
	if( kad_stat )	kad_stat->nb_byte_recv += pkt.get_len();

	// handle the special case of kad_pkttype_t::RESET_REPLY
	bool	tokeep	= handle_reset_reply(pkt);
	if( !tokeep )		return false;
	// if the pkt_t is null, it is handle_reset_reply() notifying to discard it, return true
	if( pkt.is_null() )	return true;

	// notify the recved_data to the caller
	return notify_callback( kad_event_t::build_recved_data(&pkt) );
}

/** \brief handle the special case of kad_pkttype_t::RESET_REPLY
 * 
 * - if a kad_pkttype_t::RESET_REPLY is received
 *   - check that its inner packet is the same as the packet sent (as a DOS protection)
 *   - if not equal, discard the packet
 *   - if equal, act if the udp_client_t received a error aka notify a kad_event_t::CNX_CLOSED
 * 
 * @return a tokeep for the udp_client_t - additionnatly pkt_t may be nullified if it is 
 *         to be discarded
 */
bool	kad_clicnx_t::handle_reset_reply(pkt_t &pkt)				throw()
{
	kad_pkttype_t	pkttype(kad_peer->get_profile().pkttype());
	// peek the kad_pkttype_t at the head of the packet
	try {
		pkt.unserial_peek( pkttype );
	}catch(serial_except_t &e){
		return notify_callback( kad_event_t::build_cnx_closed("Cant parse kad_pkttype_t::RESET_REPLY() packet due to " + e.what()) );
	}

	// if this pkt IS NOT a kad_pkttype_t::RESET_REPLY, return true now
	if( pkttype != pkttype.RESET_REPLY() )	return true;
	// NOTE: here the pkttype is a RESET_REPLY
	
	// compute the pkttype_len
	size_t	pkttype_len	= 1;
	DBG_ASSERT( kad_peer->get_profile().pkttype().serial_type() == pkttype_profile_t::UINT8);

	// log to debug
	KLOG_DBG("pkt_to_send=" << pkt_to_send.to_datum(datum_t::NOCOPY));
	KLOG_DBG("inner_pkt=" << pkt.tail_peek(pkt.length()-pkttype_len, datum_t::NOCOPY));

	// check if the inner packet of the RESET_REPLY is equal to the pkt_to_send
	if( pkt_to_send.to_datum(datum_t::NOCOPY) != pkt.tail_peek(pkt.length()-pkttype_len, datum_t::NOCOPY) ){
		// log the event
		KLOG_INFO("received a RESET REPLY but the inner packet IS NOT equal to the sent one");
		// mark the pkt_t as 'todiscard'
		pkt	= pkt_t();
		return true;
	}
	
	// If the received RESET_REPLY is valid, as if a error have been received on udp_client_t
	return notify_callback( kad_event_t::build_cnx_closed("Receive a kad_pkttype_t::RESET_REPLY()") );
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
bool 	kad_clicnx_t::notify_callback(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_clicnx_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


