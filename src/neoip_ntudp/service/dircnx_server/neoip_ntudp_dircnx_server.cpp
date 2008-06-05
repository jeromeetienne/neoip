/*! \file
    \brief Definition of the ntudp_dircnx_server_t
    
*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_ntudp_dircnx_server.hpp"
#include "neoip_ntudp_dircnx_server_cnx.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_udp.hpp"
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
ntudp_dircnx_server_t::ntudp_dircnx_server_t()				throw()
{
	// zero some field
	ntudp_peer	= NULL;
}

/** \brief destructor
 */
ntudp_dircnx_server_t::~ntudp_dircnx_server_t()				throw()
{
	// if the udp_vresp has been setup, unregister the callbacks
	if( ntudp_peer ){
		udp_vresp_t *	udp_vresp	= ntudp_peer->udp_vresp();
		udp_vresp->unregister_callback(ntudp_sock_pkttype_t::DIRECT_CNX_REQUEST);
	}
	// close all the cnx_t attached to this udp_vresp_t
	while( !cnx_list.empty() )	nipmem_delete	cnx_list.front();	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     start function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 * 
 * @return true on error, false otherwise
 */
ntudp_err_t	ntudp_dircnx_server_t::start(ntudp_peer_t *ntudp_peer)	throw()
{
	// copy the parameter
	this->ntudp_peer	= ntudp_peer;
	// register all the packet types in the udp_vresp_t
	udp_vresp_t *udp_vresp	= ntudp_peer->udp_vresp();
	udp_vresp->register_callback(ntudp_sock_pkttype_t::DIRECT_CNX_REQUEST, this, NULL);
	// return no error
	return ntudp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reponder callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_resp_t when a connection is established
 */
bool	ntudp_dircnx_server_t::neoip_inet_udp_vresp_event_cb(void *userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw()
{
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( udp_event.is_resp_ok() );	
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:{
			// if an established connection is notified by the udp_resp_t, create a cnx_t to handle it
			udp_full_t *	udp_full = udp_event.get_cnx_established();
			// create the connection - it links itself to 'this' udp_vresp_t
			nipmem_new	cnx_t(this, udp_full);
			break;}
	default:	DBG_ASSERT( 0 );	
	}
	// return a 'tokeep'
	return true;
}


NEOIP_NAMESPACE_END



