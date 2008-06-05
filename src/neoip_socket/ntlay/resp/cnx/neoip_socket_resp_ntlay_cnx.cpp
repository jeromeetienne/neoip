/*! \file
    \brief Declaration of the neoip_socket_udp_resp
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_resp_ntlay_cnx.hpp"
#include "neoip_socket_full_ntlay.hpp"
#include "neoip_socket_layer_ntlay.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_nlay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN 


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_resp_ntlay_t::cnx_t::cnx_t(socket_resp_ntlay_t *socket_resp_ntlay, ntudp_full_t *ntudp_full) throw()
{
	// copy parameter
	this->socket_resp_ntlay	= socket_resp_ntlay;
	this->ntudp_full	= ntudp_full;
	// link the connection to the list
	socket_resp_ntlay->cnx_dolink( this );
}

/** \brief Destructor
 */
socket_resp_ntlay_t::cnx_t::~cnx_t()			throw()
{
	// close the full connection
	nipmem_zdelete	ntudp_full;
	// unlink the connection to the list
	socket_resp_ntlay->cnx_unlink( this );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_resp_ntlay_t::cnx_t::start()	throw()
{
	ntudp_err_t	ntudp_err;
	// start the udp_full
	ntudp_err	= ntudp_full->start(this, NULL);
	if( ntudp_err.failed() )	return socket_err_from_ntudp(ntudp_err);
	// return no error
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    ntudp_full_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_full_t when to notify an event
 */
bool	socket_resp_ntlay_t::cnx_t::neoip_ntudp_full_event_cb(void *userptr, ntudp_full_t &cb_ntudp_full
					, const ntudp_event_t &ntudp_event)	throw()
{
	KLOG_DBG("enter event=" << ntudp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( ntudp_event.is_full_ok() );

	// if the udp_event_t is fatal, delete this cnx_t
	if( ntudp_event.is_fatal() ){
		nipmem_delete this;
		return false;
	}
	
	// handle each possible events from its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::RECVED_DATA:{
			pkt_t *	pkt	= ntudp_event.get_recved_data();
			return handle_recved_data(*pkt);}
	default:	DBG_ASSERT( 0 );
	}
	// return 'tokeep'
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    packet reception
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Handle the reception of a udp packet
 * 
 * @return a tokeep for the udp_full_t callback
 */
bool	socket_resp_ntlay_t::cnx_t::handle_recved_data(pkt_t &pkt)			throw()
{
	nlay_err_t	nlay_err;
	nlay_event_t	nlay_event;
	bytearray_t	pathid;

	// build the pathid from the udp_full local/remote address - later used to cookie computation
	pathid << ntudp_full->local_addr();
	pathid << ntudp_full->remote_addr();

	// process the incoming packet by nlay_resp
	nlay_err = socket_resp_ntlay->nlay_resp->pkt_from_lower(pkt, pathid.to_datum(), nlay_event);
	if( !nlay_err.succeed() ){
		KLOG_DBG("processing incoming udp packet thru nlay produced "<< nlay_err);
		return true;
	}

	// if nlay_resp_t provided a packet to reply, send it back thru udp_full
	// - here the udp_full->send error is purposely ignored as udp is an unrealiable outter transport
	//   and anyway nothing can be done as the packet is only a reply from an incoming packet
	if( !pkt.is_null() )	ntudp_full->send(pkt);

	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( nlay_event.is_sync_resp_ok() );

	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::CNX_ESTABLISHED:	// if the connection is now established
			// handle the newly established connection and forward the event
			return notify_cnx_established(nlay_event.get_cnx_established());
	case nlay_event_t::NONE:		// if this packet didnt trigger any connection
			// return tokeep
			return true;
	default:	DBG_ASSERT( 0 );
	}

	// return dont keep
	return false;
}

/** \brief Notify a CNX_ESTABLISHED event 
 * 
 * @param nlay_full	The nlay_full_t of this connection. the pointer is now owned
 *                      by this function.
 * @return a tokeep for the udp_full_t callback.
 */
bool	socket_resp_ntlay_t::cnx_t::notify_cnx_established(nlay_full_t *nlay_full)	throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");

	// backup the object_slotid of the ntudp_full_t - to be able to return its tokeep value
	slot_id_t	ntudp_full_slotid	= ntudp_full->get_object_slotid();

	// create the socket_full_ntlay_t
	socket_full_ntlay_t *	socket_full_ntlay;
	socket_full_ntlay	= nipmem_new socket_full_ntlay_t();
	// set ctor_param in socket_full_ntlay_t
	socket_err	= socket_full_ntlay->set_ctor_param(ntudp_full, nlay_full
						, socket_resp_ntlay->profile());
	if( socket_err.failed() ){
		// delete the just created variables (
		nipmem_delete	nlay_full;
		nipmem_delete	socket_full_ntlay;
		// delete the cnx_t
		nipmem_delete	this;
		return false;
	}

	// now ntudp_full is handled by the user, mark it unused locally
	// - MUST be done before the event notification in case the callback delete the object
	ntudp_full = NULL;

	// create the socket_full_t
	socket_full_t *	socket_full	= nipmem_new socket_full_t(socket_full_ntlay);
	// build the event to notify
	socket_event_t	socket_event	= socket_event_t::build_cnx_established(socket_full);
	// notify the object
	bool	tokeep	= socket_resp_ntlay->notify_callback(socket_event);
	// if the socket_resp_ntlay_t has not been delete during the callback, auto delete this cnx_t
	// - NOTE: be carefull not to use the object after this line
	if( tokeep )	nipmem_delete	this;

	// if the ntudp_full_t has no been deleted, so 'tokeep' else return 'dontkeep'
	return object_slotid_tokeep(ntudp_full_slotid);
}


NEOIP_NAMESPACE_END




