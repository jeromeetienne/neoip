/*! \file
    \brief Class to handle the bt_swarm_itor_t
    
- TODO why is this linked to bt_swarm_t and not bt_swarm_peersrc_t ?!?!
  - as bt_swarm_peersrc_t is the one launching them
  - this breaks the 'regularity rules'

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_itor.hpp"
#include "neoip_bt_swarm_peersrc.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bt_session_helper.hpp"
#include "neoip_bt_handshake.hpp"
#include "neoip_socket_client.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_pkt.hpp"
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
bt_swarm_itor_t::bt_swarm_itor_t()		throw()
{
	// zero some fields
	m_swarm_peersrc	= NULL;
	socket_client	= NULL;
}

/** \brief Destructor
 */
bt_swarm_itor_t::~bt_swarm_itor_t()		throw()
{
	// delete the socket_client if needed
	nipmem_zdelete	socket_client;
	// unlink this object from the bt_swarm_peersrc_t
	if( m_swarm_peersrc )	m_swarm_peersrc->itor_unlink(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t bt_swarm_itor_t::start(bt_swarm_peersrc_t *m_swarm_peersrc
					, const bt_peersrc_peer_t &m_peersrc_peer
					, bt_swarm_itor_cb_t *callback, void *userptr)	throw()
{
	bt_swarm_t *			bt_swarm	= m_swarm_peersrc->get_swarm();
	socket_err_t			socket_err;
	// copy the parameter
	this->m_swarm_peersrc	= m_swarm_peersrc;
	this->m_peersrc_peer	= m_peersrc_peer;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the bt_swarm_peersrc_t
	m_swarm_peersrc->itor_dolink(this);

	// get the socket_itor_arg_t for this peersrc_peer()
	socket_itor_arg_t 	itor_arg;
	itor_arg	= bt_session_helper_t::build_socket_itor_arg(peersrc_peer(), bt_swarm);
	// start the socket_client_t
	socket_client	= nipmem_new socket_client_t();	
	socket_err	= socket_client->start(itor_arg, this, NULL);
	if( socket_err.failed() )	return bt_err_from_socket(socket_err);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the remote socket_addr_t of this bt_swarm_itor_t
 */
const socket_addr_t &	bt_swarm_itor_t::remote_addr()	const throw()
{
	return socket_client->remote_addr();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_client_t to provide event
 */
bool	bt_swarm_itor_t::neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
						, const socket_event_t &socket_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( socket_event.is_client_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() )	return notify_failure(socket_event.to_string());

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:	return handle_cnx_established();
	case socket_event_t::RECVED_DATA:	return handle_recved_data(*socket_event.get_recved_data());
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
bool	bt_swarm_itor_t::handle_cnx_established()	throw()
{
	bt_swarm_t *	bt_swarm	= swarm_peersrc()->get_swarm();
	// log to debug
	KLOG_DBG("enter");
	
	// NOTE: no sendbuf management is made as the bt_handshake_t is very small
	
	// build the handshake
	pkt_t	pkt;
	pkt	<< bt_swarm->get_handshake();
	// send the handshake
	socket_client->send(pkt);

	// return 'tokeep'
	return true;
}

/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the socket_client_t
 */
bool	bt_swarm_itor_t::handle_recved_data(pkt_t &pkt)	throw()
{
	bt_swarm_t *	bt_swarm	= swarm_peersrc()->get_swarm();
	bt_handshake_t	remote_handshake;
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);
	// queue the received data to the one already received
	recved_data.append(pkt.to_datum(datum_t::NOCOPY));
	
	// try to parse the bt_handshake_t from the remote peer
	bytearray_t	copy_data	= recved_data;
	try {
		copy_data >> remote_handshake;
	}catch(serial_except_t &e){
		// if the unserialization of the bt_handshake_t failed, just return tokeep
		return true;
	}
	
	// log to debug
	KLOG_DBG("remote_handshake=" << remote_handshake);
	
	// if the remote_handshake infohash doesnt matches the bt_swarm_t one, notify_failure
	if( remote_handshake.infohash() != bt_swarm->get_mfile().infohash() )
		return notify_failure("invalid infohash");	
	// if there is already a bt_swarm_full_t for this remote peerid, notify_failure
	if( bt_swarm->full_by_remote_peerid(remote_handshake.peerid()) )
		return notify_failure("already got a bt_swarm_full_t with the same peerid");
	// if the remote peerid is the local peerid, notify_failure
	if( bt_swarm->local_peerid() == remote_handshake.peerid() )
		return notify_failure("the remote peer is the local peer.");
	// if it is no more allowed to create a bt_swarm_full, notify_failure
	if( bt_swarm->is_new_full_allowed() == false )
		return notify_failure("no new bt_swarm_full_t is allowed");
	
	// steal the socket_full_t from the socket_client_t - after this the socket_client_t MUST be deleted
	socket_full_t *	socket_full_stolen	= socket_client->steal_full();
	// delete the socket_client and mark it unused
	nipmem_zdelete	socket_client;
	
	// notify success
	return notify_success(socket_full_stolen, copy_data, remote_handshake);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief helper to notify a failure
 */
bool	bt_swarm_itor_t::notify_failure(const std::string &reason)		throw()
{
	return notify_failure(bt_err_t(bt_err_t::ERROR, reason));
}

/** \brief helper to notify a failure
 */
bool	bt_swarm_itor_t::notify_failure(const bt_err_t &bt_err)		throw()
{
	// notify the faillure to the caller
	return notify_callback(bt_err, NULL, bytearray_t(), bt_handshake_t());
}

bool	bt_swarm_itor_t::notify_success(socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw()
{
	return notify_callback(bt_err_t::OK, socket_full, recved_data, remote_handshake);
}

/** \brief notify the callback with the tcp_event
 */
bool bt_swarm_itor_t::notify_callback(const bt_err_t &bt_err, socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_swarm_itor_cb(userptr, *this, bt_err, socket_full
						, recved_data, remote_handshake);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





