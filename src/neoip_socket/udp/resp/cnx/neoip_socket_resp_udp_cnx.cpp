/*! \file
    \brief Declaration of the neoip_socket_udp_resp
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_resp_udp_cnx.hpp"
#include "neoip_socket_full_udp.hpp"
#include "neoip_socket_layer_udp.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_nlay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_cpp_demangle.hpp"

NEOIP_NAMESPACE_BEGIN 


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_resp_udp_t::cnx_t::cnx_t(socket_resp_udp_t *socket_resp_udp, udp_full_t *udp_full)	throw()
{
	// copy parameter
	this->socket_resp_udp	= socket_resp_udp;
	this->udp_full		= udp_full;
	// link the connection to the list
	socket_resp_udp->cnx_dolink( this );
}

/** \brief Destructor
 */
socket_resp_udp_t::cnx_t::~cnx_t()			throw()
{
	// close the full connection
	nipmem_zdelete	udp_full;
	// unlink the connection to the list
	socket_resp_udp->cnx_unlink( this );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_resp_udp_t::cnx_t::start()	throw()
{
	inet_err_t	inet_err;
	// start the udp_full
	inet_err = udp_full->start(this, NULL);
	if( inet_err.failed() )	return socket_err_from_inet(inet_err);
	// some logging
	KLOG_DBG("create a udp full socket local=" << udp_full->get_local_addr()
						<< " remote=" << udp_full->get_remote_addr() );	
	// return no error
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    udp_full_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	socket_resp_udp_t::cnx_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
					, const udp_event_t &udp_event)		throw()
{
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// if the udp_event_t is fatal, delete this cnx_t
	if( udp_event.is_fatal() ){
		nipmem_delete this;
		return false;
	}
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::RECVED_DATA:{
			pkt_t *	pkt	= udp_event.get_recved_data();
			return recv_udp_pkt(*pkt);}
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
bool	socket_resp_udp_t::cnx_t::recv_udp_pkt(pkt_t &pkt)			throw()
{
	nlay_err_t	nlay_err;
	nlay_event_t	nlay_event;
	bytearray_t	pathid;

	// build the pathid from the udp_full local/remote address - later used to cookie computation
	pathid << udp_full->get_local_addr();
	pathid << udp_full->get_remote_addr();

	// process the incoming packet by nlay_resp
	nlay_err = socket_resp_udp->nlay_resp->pkt_from_lower(pkt, pathid.to_datum(), nlay_event);
	if( !nlay_err.succeed() ){
		KLOG_DBG("processing incoming udp packet thru nlay produced "<< nlay_err);
		return true;
	}

	// if nlay_resp_t provided a packet to reply, send it back thru udp_full
	// - here the udp_full->send error is purposely ignored as udp is an unrealiable outter transport
	//   and anyway nothing can be done as the packet is only a reply from an incoming packet
	if( !pkt.is_null() )	udp_full->send(pkt.get_data(), pkt.get_len());

	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( nlay_event.is_sync_resp_ok() );

	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::CNX_ESTABLISHED:	// if the connection is now established
			// handle the newly established connection and forward the event
			return notify_cnx_established(nlay_event.get_cnx_established());
	case nlay_event_t::NONE:		// if this packet didnt trigger any connection
			break;
	default:	DBG_ASSERT( 0 );
	}

	// autodelete to delete the udp connection (specific to udp)
	// - the point is not to use local rescources until the remote peer is authorized
	// - so the udp connection MUST be deleted if the nlay_resp_t didnt produced a new connection
	//   as a it is a local resource.
	nipmem_delete	this;
	// return dont keep
	return false;
}

/** \brief Notify a CNX_ESTABLISHED event 
 * 
 * @param nlay_full	The nlay_full_t of this connection. the pointer is now owned
 *                      by this function.
 * @return a tokeep for the udp_full_t callback.
 */
bool	socket_resp_udp_t::cnx_t::notify_cnx_established(nlay_full_t *nlay_full)	throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");

	// backup the object_slotid of the udp_full_t - to be able to return its tokeep value
	slot_id_t	udp_full_slotid	= udp_full->get_object_slotid();

	// create the socket_full_udp_t
	socket_full_udp_t *	socket_full_udp;
	socket_full_udp		= nipmem_new socket_full_udp_t();
	// set ctor_param in socket_full_udp_t
	socket_err = socket_full_udp->set_ctor_param(udp_full, nlay_full, socket_resp_udp->profile());
	if( socket_err.failed() ){
		// delete the just created variables
		nipmem_delete	nlay_full;
		nipmem_delete	socket_full_udp;
		// delete the cnx_t
		nipmem_delete	this;
		return false;
	}

	// now udp_full is handled by the user, mark it unused locally
	// - MUST be done before the event notification in case the callback delete the object
	udp_full = NULL;

	// create the socket_full_t
	socket_full_t *	socket_full	= nipmem_new socket_full_t(socket_full_udp);
	// build the event to notify
	socket_event_t	socket_event	= socket_event_t::build_cnx_established(socket_full);
	// notify the object
	bool	tokeep	= socket_resp_udp->notify_callback(socket_event);
	// if the socket_resp_udp_t has not been delete during the callback, auto delete this cnx_t
	// - NOTE: be carefull not to use the object after this line
	if( tokeep )	nipmem_delete	this;

	// if the udp_full_t has not been deleted, so 'tokeep' else return 'dontkeep'
	return object_slotid_tokeep(udp_full_slotid);
}


NEOIP_NAMESPACE_END




