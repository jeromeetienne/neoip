/*! \file
    \brief Definition of the ntudp_npos_natlback_t
    
*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_natlback_cnx.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_udp_layer.hpp"
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
ntudp_npos_natlback_t::cnx_t::cnx_t(ntudp_npos_natlback_t *ntudp_npos_natlback, udp_full_t *udp_full)		throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");
	// copy parameter
	this->ntudp_npos_natlback	= ntudp_npos_natlback;
	this->udp_full			= udp_full;
	// set the callback for the udp_full
	inet_err = udp_full->set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
	// start the udp_full
	inet_err = udp_full->start();
	DBG_ASSERT( inet_err.succeed() );
	// link the connection to the list
	ntudp_npos_natlback->cnx_link( this );
}

/** \brief destructor
 */
ntudp_npos_natlback_t::cnx_t::~cnx_t()				throw()
{
	// log to debug
	KLOG_DBG("enter");
	// close the full connection
	if( udp_full )		nipmem_delete udp_full;
	// unlink the connection to the list
	ntudp_npos_natlback->cnx_unlink( this );	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	ntudp_npos_natlback_t::cnx_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:	// autodelete of the cnx_t
					nipmem_delete	this;
					return false;
	case udp_event_t::RECVED_DATA:	// handle the received packet
					return handle_recved_data(*udp_event.get_recved_data());
	case udp_event_t::MTU_CHANGE:	// this event MUST NOT happend as this connection has no pmtudisc
					DBG_ASSERT( 0 );
	default:	DBG_ASSERT( 0 );
	}
	// return a 'tokeep'
	return true;
}

/** \brief Handle received data on the cnx_t
 * 
 * - NOTE: this may delete the ntudp_npos_natlback_t or the udp_full_t
 * 
 * @return a 'tokeep/dontkeep' for the udp_full_t
 */
bool	ntudp_npos_natlback_t::cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	ntudp_nonce_t	incoming_nonce;
	// log to debug
	KLOG_DBG("enter");

	try {	// read the incoming_nonce
		pkt >> incoming_nonce;
		// if the incoming nonce matches the ntudp_npos_natlback one, notify sucess
		if( incoming_nonce == ntudp_npos_natlback->request_nonce ){
			return ntudp_npos_natlback->notify_recved_nonce();
		}
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
	}

	// autodelete
	nipmem_delete this;
	// return 'dontkeep'
	return false;
}




NEOIP_NAMESPACE_END



