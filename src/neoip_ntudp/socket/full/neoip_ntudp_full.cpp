/*! \file
    \brief Definition of the ntudp_full_t
    
\par Brief description
This module is basically a wrapper on top of udp_full_t and just additionnaly
the 'retransmission' of the packets from the connection establishment.


*/

/* system include */
/* local include */
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_full_t::ntudp_full_t(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &m_local_addr
				, const ntudp_addr_t &m_remote_addr, udp_full_t *udp_full)	throw()
{
	// sanity check - the local and remote address MUST be fully qualified
	DBG_ASSERT( m_local_addr.is_fully_qualified() );
	DBG_ASSERT( m_remote_addr.is_fully_qualified() );
	// sanity check - the local peerid MUST be the ntudp_peer_t one
	DBG_ASSERT( m_local_addr.peerid() == ntudp_peer->local_peerid() );
	
	// copy parameters
	this->ntudp_peer	= ntudp_peer;
	this->m_local_addr	= m_local_addr;
	this->m_remote_addr	= m_remote_addr;
	this->udp_full		= udp_full;
	this->callback		= NULL;
	// set the callback for the udp_full
	inet_err_t inet_err	= udp_full->set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
// TODO even MORE here the issue is the similar of the udp_vresp_t but differs...
// the shit about the start() stuff needs to be fixed...


// TODO here there is an issue due to udp_vresp_t
// - udp_vresp_t do a start() on the udp_full to get the first packet
// - and here i do another start on it ...
// - hence the udp_full is started twice and the fails
//	inet_err = udp_full->start();
//	DBG_ASSERT( inet_err.succeed() );

	// link this object to the ntudp_peer_t
	ntudp_peer->ntudp_full_link(this);
}

/** \brief destructor
 */
ntudp_full_t::~ntudp_full_t()		throw()
{
	// unlink this object from the ntudp_peer_t
	ntudp_peer->ntudp_full_unlink(this);
	// delete the udp_full
	nipmem_delete	udp_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      Setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the incoming/outgoing packet from the connection establishement
 */
ntudp_full_t & 	ntudp_full_t::set_estapkt(const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	// copy the parameter
	this->estapkt_in	= estapkt_in;
	this->estapkt_out	= estapkt_out;
	// return the object itself
	return *this;
}

/** \brief Set the callback
 */
ntudp_full_t &	ntudp_full_t::set_callback(ntudp_full_cb_t *callback, void *userptr)	throw()
{
	// copy some parameter
	this->callback		= callback;
	this->userptr		= userptr;
	// return the object itself
	return *this;
}

/** \brief Start the operation
 */
ntudp_err_t	ntudp_full_t::start()	throw()
{
	// sanity check - the ntudp_full_t MUST be already started
	DBG_ASSERT( this->callback );
	
	// return no error
	return ntudp_err_t::OK;
}


/** \brief Start the operation (just a helper - to improve the readability)
 */
ntudp_err_t	ntudp_full_t::start(ntudp_full_cb_t *callback, void *userptr)	throw()
{
	// set the callback
	set_callback(callback, userptr);
	// Start the operation
	return start();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	ntudp_full_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw()
{
	ntudp_event_t	ntudp_event;
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// convert the udp_event_t into a ntudp_event_t one and simply forward it
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:
			// simply convert and forward the event
			ntudp_event = ntudp_event_t::build_cnx_closed(udp_event.get_cnx_closed_reason());
			return notify_callback(ntudp_event);
	case udp_event_t::RECVED_DATA:
			// handle the RECVED_DATA - not just forwarding for this one to handle estapkt
			return handle_recved_data(udp_event.get_recved_data());
	case udp_event_t::MTU_CHANGE:
			// simply convert and forward the event
			ntudp_event = ntudp_event_t::build_mtu_change(udp_event.get_mtu_change());
			return notify_callback(ntudp_event);
	default:	DBG_ASSERT( 0 );
	}

	// return a 'tokeep'
	return true;
}

/** \brief Handle the udp_event_t::RECVED_DATA 
 * 
 * - if the incoming packet is match the estapkt_in, reply the estpkt_out and dont notify the caller
 * - else notify the caller
 * 
 * @return a tokeep for the udp_full_t object
 */
bool ntudp_full_t::handle_recved_data(pkt_t *pkt)	throw()
{
	// handle the estapkt_in if still present
	if( !estapkt_in.is_null() ){
		// if the incoming packet is equal to the estapkt_in, reply estpkt_out and exit
		if( *pkt == estapkt_in ){
			udp_full->send( estapkt_out );
			return true;
		}
		// if the received packet is not estapkt_in, it is assumed the remote peer received
		// the estapkt_out, so free both
		estapkt_in	= pkt_t();
		estapkt_out	= pkt_t();
	}
	// notify the caller of received data
	ntudp_event_t	ntudp_event = ntudp_event_t::build_recved_data(pkt);
	return notify_callback(ntudp_event);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      to_string() function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	ntudp_full_t::to_string()	const throw()
{
	return remote_addr().to_string();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	ntudp_full_t::notify_callback(const ntudp_event_t &ntudp_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_full_event_cb( userptr, *this, ntudp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



