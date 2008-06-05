/*! \file
    \brief Declaration of the udp_resp_t
    
*/

/* system include */
/* local include */
#include "nc2_server.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 * 
 * - TODO poor error handling if the init failed
 */
nc2_server_t::nc2_server_t(const std::string &addr_str)		throw()
{
	inet_err_t	err;
	nlay_err_t	nlay_err;
	// set the listen address
	err = udp_resp.set_listen_addr(addr_str.c_str());
	DBG_ASSERT( err.succeed() );
	// set the callback
	err = udp_resp.set_callback(this, NULL);
	DBG_ASSERT( err.succeed() );
	// start the udp_resp_t
	err = udp_resp.start();
	if( err.failed() )	KLOG_ERR("Cant start udp_resp due to " << err );
	DBG_ASSERT( err.succeed() );
	// start the nlay resp
	nlay_resp = nlay_resp_t(&nlay_profile, nlay_type_t::DGRAM, nlay_type_t::DGRAM);
	nlay_err  = nlay_resp.start();
	DBG_ASSERT( nlay_err.succeed() );	
}

/** \brief Desstructor
 */
nc2_server_t::~nc2_server_t()		throw()
{
	// delete all the pending udp connection
	while( !cnx_udp_list.empty() )	nipmem_delete	cnx_udp_list.front();	
	// delete all the pending nlay connection
	while( !cnx_nlay_list.empty() )	nipmem_delete	cnx_nlay_list.front();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reponder callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_resp_t when a connection is established
 */
bool	nc2_server_t::neoip_inet_udp_resp_event_cb(void *userptr, udp_resp_t &cb_udp_resp
							, const udp_event_t &udp_event)	throw()
{
	KLOG_ERR("enter event=" << udp_event);
	if( udp_event.is_cnx_established() ){
		udp_full_t *	udp_full = udp_event.get_cnx_established();
		// create the connection
		nipmem_new	cnx_udp_t(this, udp_full);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    nc2_server_t::cnx_udp_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 * 
 * - TODO poor error handle when the constructor fails
 */
nc2_server_t::cnx_udp_t::cnx_udp_t(nc2_server_t *nc2_server, udp_full_t *udp_full)	throw()
{
	inet_err_t	inet_err;
	// copy parameter
	this->nc2_server	= nc2_server;
	this->udp_full		= udp_full;
	// set the callback
	inet_err = udp_full->set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
	// start the udp_full
	inet_err = udp_full->start();
	DBG_ASSERT( inet_err.succeed() );
	KLOG_ERR("create a udp full socket local=" << udp_full->get_local_addr()
						<< " remote=" << udp_full->get_remote_addr() );	

	// link the connection to the list
	nc2_server->cnx_udp_link( this );
}

/** \brief Desstructor
 */
nc2_server_t::cnx_udp_t::~cnx_udp_t()			throw()
{
	KLOG_ERR("destroy a cnx_udp_t socket");
	// close the full connection
	if( udp_full )	nipmem_delete	udp_full;
	// unlink the connection to the list
	nc2_server->cnx_udp_unlink( this );
}


/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	nc2_server_t::cnx_udp_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
					, const udp_event_t &udp_event)		throw()
{
	KLOG_ERR("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:
			nipmem_delete this;
			return false;
	case udp_event_t::RECVED_DATA:{
			pkt_t *	pkt	= udp_event.get_recved_data();
			bool	to_keep = recv_udp_pkt(*pkt);
			if( !to_keep )	return false;
			break;}
	default:	DBG_ASSERT( 0 );
	}

	return true;
}


/** \brief Handle the reception of a udp packet
 */
bool	nc2_server_t::cnx_udp_t::recv_udp_pkt(pkt_t &pkt)			throw()
{
	nlay_err_t	nlay_err;
	nlay_event_t	nlay_event;
	serial_t	pathid;

	// build the pathid from the udp_full
	pathid << udp_full->get_local_addr();
	pathid << udp_full->get_remote_addr();

	// forward the incoming packet to nlay_resp
	nlay_err = nc2_server->nlay_resp.pkt_from_lower(pkt, datum_t(pathid.get_data(), pathid.get_len())
									, nlay_event);
	if( !nlay_err.succeed() ){
		KLOG_ERR("processing incoming udp packet thru nlay produced "<< nlay_err);
		return true;
	}
	// if nlay_resp provided a packet, send it back thru udp_full
	if( !pkt.is_null() )	udp_full->send(pkt.get_data(), pkt.get_len());

	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::CNX_ESTABLISHED:{	// if the connection is now established
			nlay_full_t *	nlay_full = nlay_event.get_cnx_established();
			// create the cnx_nlay_t connection
			nipmem_new	cnx_nlay_t(nc2_server, udp_full, nlay_full);
			// now udp_full is handler by cnx_nlay_t, mark it unused locally
			udp_full = NULL;
			// autodelete
			nipmem_delete	this;
			return true;}
	case nlay_event_t::NONE:		// if this packet didnt trigger any connection
			break;
	default:	DBG_ASSERT( 0 );
	}

	// autodelete to delete the udp connection
	// - the point is not to use local rescources until the remote peer is authorized
	// - so the udp connection MUST be deleted if the nlay_resp_t didnt produced a new connection
	//   as a it is a local resource.
	nipmem_delete	this;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    nc2_server_t::cnx_nlay_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nc2_server_t::cnx_nlay_t::cnx_nlay_t(nc2_server_t *nc2_server, udp_full_t *udp_full
						, nlay_full_t *nlay_full)	throw()
{
	inet_err_t	inet_err;
	nlay_err_t	nlay_err;
	// copy parameter
	this->nc2_server	= nc2_server;
	this->udp_full		= udp_full;
	this->nlay_full		= nlay_full;

	// change the callback for udp_full
	inet_err = udp_full->set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );

	// set the callback for nlay_full
	nlay_err = nlay_full->set_callback(this, NULL);
	DBG_ASSERT( nlay_err.succeed() );

	// start the nlay_full
	nlay_err = nlay_full->start();
	DBG_ASSERT( nlay_err.succeed() );
	KLOG_ERR("create a nlay full socket" );	

	// link the connection to the list
	nc2_server->cnx_nlay_link( this );
}

/** \brief Desstructor
 */
nc2_server_t::cnx_nlay_t::~cnx_nlay_t()			throw()
{
	KLOG_ERR("destroy a nlay full socket");
	// close the udp_full connection
	nipmem_delete	udp_full;
	// close the nlay_full connection
	nipmem_delete	nlay_full;
	// unlink the connection to the list
	nc2_server->cnx_nlay_unlink( this );
}


/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	nc2_server_t::cnx_nlay_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw()
{
	nlay_err_t	nlay_err;
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:
			KLOG_ERR("enter event=" << udp_event);
//			nipmem_delete this;
			return false;
	case udp_event_t::RECVED_DATA:{
			pkt_t *	pkt = udp_event.get_recved_data();
			nlay_err = nlay_full->pkt_from_lower( *pkt );
			if( !nlay_err.succeed() )
				KLOG_ERR("processing incoming udp packet thru nlay produced "<< nlay_err);
			if( !pkt->is_null() )	KLOG_ERR("pkt=" << *pkt );
			break;}
	default:	DBG_ASSERT( 0 );
	}

	return true;
}

/** \brief callback notified by \ref nlay_full_t when to notify an event
 */
bool	nc2_server_t::cnx_nlay_t::neoip_nlay_full_event_cb(void *userptr, nlay_full_t &cb_nlay_full
							, const nlay_event_t &nlay_event)	throw()
{
	nlay_err_t	nlay_err;
	KLOG_ERR("enter event=" << nlay_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( nlay_event.is_full_ok() );
	// sanity check - the nlay_itor MUST NOT be null
	DBG_ASSERT( nlay_full );
	// sanity check - the udp_full MUST NOT be null
	DBG_ASSERT( udp_full );
	
	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::CNX_CLOSING:
	case nlay_event_t::CNX_DESTROYED:
			KLOG_ERR("enter event=" << nlay_event);
			nipmem_delete this;
			return false;
	case nlay_event_t::PKT_TO_LOWER:{
			pkt_t *	pkt	= nlay_event.get_pkt_to_lower();
			udp_full->send(pkt->get_data(), pkt->get_len());
			break;}
	default:	DBG_ASSERT( 0 );
	}
	return true;
}

NEOIP_NAMESPACE_END


