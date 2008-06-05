/*! \file
    \brief Definition of the ntudp_pserver_t

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_ntudp_pserver.hpp"
#include "neoip_ntudp_pserver_wikidbg.hpp"
#include "neoip_ntudp_pserver_tunnel.hpp"
#include "neoip_ntudp_pserver_extcnx.hpp"
#include "neoip_ntudp_pserver_reverse.hpp"
#include "neoip_ntudp_pserver_rec.hpp"
#include "neoip_ntudp_dircnx_server.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_tunl_pkttype.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_kad.hpp"
#include "neoip_kad_publish.hpp"
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
ntudp_pserver_t::ntudp_pserver_t()				throw()
{
	// zero some field
	ntudp_peer	= NULL;
	dircnx_server	= NULL;
	kad_publish	= NULL;
}

/** \brief destructor
 */
ntudp_pserver_t::~ntudp_pserver_t()				throw()
{
	// delete the kad_publish_t if needed
	nipmem_zdelete	kad_publish;
	// delete the dircnx_server if needed
	nipmem_zdelete	dircnx_server;

	// unregister the udp_vresp_t callbacks if needed
	if( ntudp_peer ){
		udp_vresp_t *	udp_vresp	= ntudp_peer->udp_vresp();
		udp_vresp->unregister_callback(ntudp_tunl_pkttype_t::REGISTER_REQUEST);
		udp_vresp->unregister_callback(ntudp_tunl_pkttype_t::PKT_EXT2SRV);
		udp_vresp->unregister_callback(ntudp_sock_pkttype_t::REVERSE_CNX_R2I_ACK);
	}

	// close all pending ntudp_pserver_tunnel_t
	while( !tunnel_db.empty() )	nipmem_delete	tunnel_db.front();
	// close all pending ntudp_pserver_extcnx_t
	while( !extcnx_db.empty() )	nipmem_delete	extcnx_db.front();
	// close all pending ntudp_pserver_reverse_t
	while( !reverse_db.empty() )	nipmem_delete	reverse_db.front();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    Setup Function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_pserver_t &	ntudp_pserver_t::set_profile(const ntudp_pserver_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check().succeed() );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
ntudp_err_t	ntudp_pserver_t::start(ntudp_peer_t *ntudp_peer)	throw()
{
	ntudp_err_t	ntudp_err;
	// copy the parameter
	this->ntudp_peer= ntudp_peer;
	
	// publish the ntudp_pserver_rec_t for other peers to find this ntudp_pserver_t
	ntudp_err	= pserver_rec_publish();
	if( ntudp_err.failed() )	return ntudp_err;

	// start the dircnx_server to receive direct connection
	dircnx_server	= nipmem_new ntudp_dircnx_server_t();
	ntudp_err	= dircnx_server->start(ntudp_peer);	
	if( ntudp_err.failed() )	return ntudp_err;
	
	// register the callback on the udp_vresp_t for the packet handled here
	// NOTE: use userptr as a lame way to distinguish between the pkttype
	ntudp_pkttype_t	pkttype;
	udp_vresp_t *	udp_vresp	= ntudp_peer->udp_vresp();
	pkttype	= ntudp_tunl_pkttype_t::REGISTER_REQUEST;
	udp_vresp->register_callback(pkttype, this, (void*)int(pkttype));
	pkttype	= ntudp_tunl_pkttype_t::PKT_EXT2SRV;
	udp_vresp->register_callback(pkttype, this, (void*)int(pkttype));
	pkttype	= ntudp_sock_pkttype_t::REVERSE_CNX_R2I_ACK;
	udp_vresp->register_callback(pkttype, this, (void*)int(pkttype));
	
	// return no error
	return ntudp_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Query Function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return a ntudp_pserver_tunnel_t pointer on the one matching this peerid, or NULL if none does
 */
ntudp_pserver_tunnel_t *ntudp_pserver_t::get_tunnel_from_peerid(const ntudp_peerid_t &peerid)	throw()
{
	std::list<ntudp_pserver_tunnel_t *>::iterator	iter;
	// scan every existing ntudp_pserver_tunnel_t
	for( iter = tunnel_db.begin(); iter != tunnel_db.end(); iter++ ){
		ntudp_pserver_tunnel_t *	tunnel = *iter;
		// if this tunnel has been register for this peerid, return this pointer;
		if( peerid == tunnel->registered_peerid() )	return tunnel;
	}
	// if it hasnt been found, return NULL
	return NULL;	
}

/** \brief Return a ntudp_pserver_extcnx_t pointer on the one matching this slot_id_t, or NULL if none does
 */
ntudp_pserver_extcnx_t *ntudp_pserver_t::get_extcnx_from_slotid(const slot_id_t &extcnx_slotid)	throw()
{
	std::list<ntudp_pserver_extcnx_t *>::iterator	iter;
	// scan every existing ntudp_pserver_extcnx_t
	for( iter = extcnx_db.begin(); iter != extcnx_db.end(); iter++ ){
		ntudp_pserver_extcnx_t *	extcnx = *iter;
		// if this extcnx_t has a matching slot_id, return this pointer
		if( extcnx_slotid == extcnx->get_slotid() )	return extcnx;
	}
	// if it hasnt been found, return NULL
	return NULL;	
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reponder callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_resp_t when a connection is established
 */
bool	ntudp_pserver_t::neoip_inet_udp_vresp_event_cb(void *userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw()
{
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( udp_event.is_resp_ok() );	
	// log to debug
	KLOG_ERR("enter event=" << udp_event);
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:{
			// if an established connection is notified by the udp_resp_t, create a tunnel to handle it
			udp_full_t *	udp_full = udp_event.get_cnx_established();
			// NOTE: use userptr as a lame way to distinguish between the pkttype at the
			//       reception of the udp_event_t::CNX_ESTABLISHED.
			uint8_t	pkttype	= static_cast<uint8_t>(reinterpret_cast<unsigned long>(userptr));
			switch(pkttype){
			case ntudp_tunl_pkttype_t::REGISTER_REQUEST:
						nipmem_new ntudp_pserver_tunnel_t(this, udp_full);
						break;
			case ntudp_tunl_pkttype_t::PKT_EXT2SRV:
						nipmem_new ntudp_pserver_extcnx_t(this, udp_full);
						break;
			case ntudp_sock_pkttype_t::REVERSE_CNX_R2I_ACK:
						nipmem_new ntudp_pserver_reverse_t(this, udp_full);
						break;
			default:	DBG_ASSERT(0);
			}
			break;}
	default:	DBG_ASSERT( 0 );	
	}
	// return a 'tokeep'
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      handle the ntudp_pserver_rec_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief publish the ntudp_pserver_rec_t
 */
ntudp_err_t	ntudp_pserver_t::pserver_rec_publish()	throw()
{
	kad_peer_t *	kad_peer	= ntudp_peer->kad_peer();
	kad_err_t	kad_err;

	// build the pserver record
	DBG_ASSERT(ntudp_peer->listen_addr_pview().is_null() == false);
	ntudp_pserver_rec_t	pserver_rec(ntudp_peer->listen_addr_pview()
					, ntudp_peer->local_peerid());

	// build the kad_rec_t out of it
	bytearray_t	payload	= bytearray_t().serialize(pserver_rec);
	kad_keyid_t	keyid	= ntudp_peer->get_profile().peer().pserver_key_str();
	kad_recid_t	recid	= kad_recid_t::build_random();	
	kad_rec_t	kad_rec = kad_rec_t(recid, keyid,profile.pserver_record_ttl(),payload.to_datum());

	// sanity check - kad_publish MUST NOT be already used
	DBG_ASSERT( kad_publish == NULL );
	
	// publish the just-built kad_rec_t
	kad_publish	= nipmem_new kad_publish_t();
	kad_err		= kad_publish->start(kad_peer, kad_rec, this, NULL);
	if( kad_err.failed() )	return ntudp_err_from_kad(kad_err);	

	// return no error
	return ntudp_err_t::OK;
}

/** \brief callback notified when a kad_publish_t has an event to notify
 */
bool	ntudp_pserver_t::neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
						, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << kad_event);	
	// sanity check - the kad_event MUST be publish_ok()
	DBG_ASSERT( kad_event.is_publish_ok() );

	// NOTE - dont do anything, just a place holder to provide a callback to kad_publish_t
	DBGNET_ASSERT( kad_event.is_completed() );

	// return tokeep
	return true;	
}
NEOIP_NAMESPACE_END



