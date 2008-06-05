/*! \file
    \brief Declaration of the kad_srvcnx_t::cnx_t
    
*/

/* system include */
/* local include */
#include "neoip_kad_srvcnx_cnx.hpp"
#include "neoip_udp.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_srvcnx_t::cnx_t::cnx_t(kad_srvcnx_t *kad_srvcnx, udp_full_t *udp_full)	throw()
{
	// copy parameter
	this->kad_srvcnx= kad_srvcnx;
	this->udp_full	= udp_full;
	// link the connection to the list
	kad_srvcnx->cnx_dolink( this );
}

/** \brief Desstructor
 */
kad_srvcnx_t::cnx_t::~cnx_t()			throw()
{
	// close the full connection
	nipmem_delete	udp_full;
	// unlink the connection to the list
	kad_srvcnx->cnx_unlink( this );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
kad_err_t kad_srvcnx_t::cnx_t::start()	throw()
{
	inet_err_t	inet_err;
	// set the callback
	inet_err	= udp_full->set_callback(this, NULL);
	if( inet_err.failed() )	goto error;
#if 0	// start the udp_full
	// - here the udp_full_t is not started as it comes from a udp_vresp_t
	//   and udp_vresp_t already start the udp_full_t
	// - it is kinda dirty, i dont like it
	inet_err	= udp_full->start();
	if( inet_err.failed() )	goto error;
#endif
	// log to debug
	KLOG_DBG("Responded to " << *udp_full );
	// return no error
	return kad_err_t::OK;
	
error:;	// autodelete
	nipmem_delete	this;
	// return the error
	return kad_err_from_inet(inet_err);

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    udp_full callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	kad_srvcnx_t::cnx_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::RECVED_DATA:{
			// get the received packet
			pkt_t *	pkt	= udp_event.get_recved_data();
			// log to debug
			KLOG_DBG("recved data=" << *pkt);
			// notify the packet
			bool tokeep	= kad_srvcnx->notify_callback(*pkt, udp_full->get_local_addr()
									, udp_full->get_remote_addr());
			// if the callback dont return tokeep, exit
			if( !tokeep )		return false;
			// if the callback dont return a reply packet, exit
			if( pkt->is_null() )	break;
			// send back the reply packet
			udp_full->send(pkt->get_data(), pkt->get_len());
			break;}
	default:	break;
	}
	// autodelete
	// note: in anycase kad_srvcnx_t handle only a single packet to perform a 
	//       request/reply protocol over single datagram
	nipmem_delete this;
	// return dont keep
	return false;
}



NEOIP_NAMESPACE_END


