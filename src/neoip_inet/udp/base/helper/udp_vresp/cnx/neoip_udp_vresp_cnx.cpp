/*! \file
    \brief Definition of the udp_vresp_t
    
*/

/* system include */
/* local include */
#include "neoip_udp_vresp_cnx.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
udp_vresp_cnx_t::udp_vresp_cnx_t(udp_vresp_t *udp_vresp, udp_full_t *udp_full)		throw()
{
	inet_err_t	inet_err;
	// copy parameter
	this->udp_vresp	= udp_vresp;
	this->udp_full	= udp_full;
	// set the callback for the udp_full
	inet_err = udp_full->set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
	// start the udp_full
	inet_err = udp_full->start();
	DBG_ASSERT( inet_err.succeed() );
	// link the connection to the list
	udp_vresp->cnx_link( this );
}

/** \brief destructor
 */
udp_vresp_cnx_t::~udp_vresp_cnx_t()				throw()
{
	// close the full connection
	if( udp_full )	nipmem_delete	udp_full;
	// unlink the connection to the list
	udp_vresp->cnx_unlink( this );	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	udp_vresp_cnx_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:
			// autodelete of the cnx_t
			nipmem_delete	this;
			return false;
	case udp_event_t::RECVED_DATA:{
			// backup the object_slotid of the udp_full_t
			slot_id_t	udp_full_slotid	= udp_full->get_object_slotid();
			// handle the received packet
			// - NOTE: it MAY destroy the udp_vresp_cnx_t so dont use the object after it
			handle_recved_data(*udp_event.get_recved_data());
			// return the computed 'tokeep' for the udp_full_t as its state is unknown
			return object_slotid_tokeep(udp_full_slotid);}
	case udp_event_t::MTU_CHANGE:	// ignored event
			break;
	default:	DBG_ASSERT( 0 );
	}
	// return a 'tokeep'
	return true;
}

/** \brief Handle received data on the cnx_t
 * 
 * - NOTE: this may delete the udp_vresp_t or the udp_full_t
 */
void	udp_vresp_cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	// backup the object_slot_id of the udp_full_t
	// - it is used later to determine if 'tokeep' or 'dontkeep' should be returned
	slot_id_t	udp_full_slotid	= udp_full->get_object_slotid();
	
	// reset the udp_full_t callback
	// - it is ok as cnx_t is only for the first packet received
	udp_full->set_callback(NULL, NULL);

	// parse the incoming packet
	try {
		uint8_t			pkttype;
		udp_vresp_t::reg_cb_t	reg_cb;
		// read the pkttype (without consuming)	
		pkt.unserial_peek( pkttype );
		// log to debug
		KLOG_DBG("pkttype=" << (int)pkttype);
		// try to get the registered cb matching the packet type
		reg_cb = udp_vresp->reg_cb_get(pkttype);
		// if the reg_cb doesnt exist, handle it as an error
		if( reg_cb.is_null() ){
			// log the event
			KLOG_INFO("Received a pkttype " << (int)pkttype << " but no callback is registered for it");
			// autodelete
			nipmem_delete this;
			return;
		}
		// notify the udp_full_t to the caller
		// - NOTE: this may delete the udp_vresp_t or the udp_full_t
		udp_event_t	udp_event = udp_event_t::build_cnx_established(udp_full);
		reg_cb.get_callback()->neoip_inet_udp_vresp_event_cb(reg_cb.get_userptr(), *udp_vresp, udp_event);
		
		// if udp_full_t still exist so send the packet to it
		if( object_slotid_exist(udp_full_slotid) )
			udp_full->push_back_resp_dgram(pkt.get_data(), pkt.get_len());

		// mark the udp_full_t as unused WITHOUT deleting it, as it is now owned by the callback
		udp_full = NULL;
		// autodelete this object
		nipmem_delete this;
	}catch(serial_except_t &e){
		KLOG_ERR("Cant parse incoming packet due to " << e.what() );
		// autodelete
		nipmem_delete this;
	}
}

NEOIP_NAMESPACE_END



