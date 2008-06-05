/*! \file
    \brief Declaration of the kad_listener_t

*/

/* system include */
/* local include */
#include "neoip_kad_listener.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_srvcnx.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_nslan.hpp"
#include "neoip_ndiag_watch.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

// only to debug
#include "neoip_lib_session.hpp"
#include "neoip_property.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_listener_t::kad_listener_t()		throw()
{
	// zero some field
	udp_vresp	= NULL;
	kad_srvcnx	= NULL;
	m_nslan_listener= NULL;
	m_nslan_peer	= NULL;
}

/** \brief Desstructor
 */
kad_listener_t::~kad_listener_t()		throw()
{
	// NOTE: dont delete the udp_vresp_t as it is own by the caller
	// destroy the kad_srvcnx
	nipmem_zdelete kad_srvcnx;
	// destroy all the remaining kad_peer_t
	while( !peer_db.empty() )	nipmem_delete peer_db.front();
	// delete nslan_peer_t if needed
	nipmem_zdelete m_nslan_peer;
	// delete nslan_listener_t if needed
	nipmem_zdelete m_nslan_listener;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Setup and start the operation
 */
kad_err_t	kad_listener_t::start(const ipport_addr_t &nslan_addr, udp_vresp_t *udp_vresp
				, const ipport_aview_t &m_listen_aview)	throw()
{
	kad_err_t	kad_err;
	inet_err_t	inet_err;

	// copy the listen_oaddr_pview
	this->m_listen_aview	= m_listen_aview;
	// sanity check - listen_aview.lview MUST be equal to udp_vresp.get_listen_addr()
	DBG_ASSERT( listen_aview().lview() == udp_vresp->get_listen_addr() );

	// copy the udp_vresp_t to later initialized the kad_srvcnx_t
	// - NOTE: kad_srvcnx_t is initialized in the peer_dolink() as it needs a pkttype_profile_t
	this->udp_vresp	= udp_vresp;
	
	// create a nslan_listener_t
	m_nslan_listener= nipmem_new nslan_listener_t();
	inet_err	= m_nslan_listener->start(nslan_addr);
	if( inet_err.failed() )	return kad_err_from_inet(inet_err);
	// create a nslan_peer_t when all kad_peer_t will publish
	m_nslan_peer	= nipmem_new nslan_peer_t(nslan_listener(), "nslan realm for kad_bstrap_t");

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/kad_listener_" + listen_lview().to_string());
	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Used by kad_*_t internal to notify an echoed local ip_addr_t
 * 
 * - in practice this is used only by kad_ping_rpc_t as PING_REPLY does
 *   include the echoed local ip_addr_t
 * - this function is used to keep the local_ipaddr_pview() uptodate
 * - WARNING: this function MUST NOT notify any callback
 *   - this function is called INSYNC inside the kad_ping_rpc_t callback
 *   - so very low level
 *   - if there are notification to be made, use a zerotimer_t
 */
void	kad_listener_t::notify_echoed_local_ipaddr(const ip_addr_t &echoed_ipaddr
				, const kad_addr_t &remote_addr)	throw()
{
	// log to debug
	KLOG_ERR("enter echoed_ipaddr=" << echoed_ipaddr << " remote_kaddr=" << remote_addr);
	// if remote_addr.oaddr().ipaddr().is_public() is not true, do nothing
	// - as it mean the echoed_ipaddr is not a public view
	if( !remote_addr.oaddr().ipaddr().is_public() )	return;
	// if echoed_ipaddr is not public,, do nothing
	if( !echoed_ipaddr.is_public() )		return;

	// log to debug
	KLOG_ERR("notifying local_ipaddr_pview=" << echoed_ipaddr << " to ndiag_watch_t");

	// notify it to the ndiag_watch_t	
	ndiag_watch_t *	ndiag_watch	= ndiag_watch_get();
	ndiag_watch->notify_ipaddr_pview(echoed_ipaddr);
}

/** \brief Function used to update the listen_pview *DURING* kad_listener_t run
 * 
 * - this function updates the linked kad_peer_t too
 */
void	kad_listener_t::update_listen_pview(const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter NOT YET IMPLEMENTED new_listen_pview=" << new_listen_pview);
	// update the m_listen_aview
	m_listen_aview.pview	(new_listen_pview);

	// go thru all the kad_peer_t
	std::list<kad_peer_t *>::iterator	iter;
	for( iter = peer_db.begin(); iter != peer_db.end(); iter++ ){
		kad_peer_t *	kad_peer	= *iter;
		// update the listen_pview in this kad_peer_t
		kad_peer->update_listen_pview(new_listen_pview);
	}
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       kad_srvcnx_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_srvcnx_t receive a packet
 * 
 * - this function only forward the packet to the proper kad_peer_t, the actual
 *   processing is happening in the kad_peer_t itself
 */
bool kad_listener_t::neoip_kad_srvcnx_cb(void *cb_userptr, kad_srvcnx_t &cb_kad_srvcnx
				, pkt_t &pkt, const ipport_addr_t &local_oaddr
				, const ipport_addr_t &remote_oaddr)		throw()
{
	const kad_profile_t &	profile		= peer_db.front()->get_profile();
	pkt_t			orig_pkt	= pkt;
	kad_peer_t *		kad_peer	= NULL;
	kad_pkttype_t		pkttype(profile.pkttype());
	kad_realmid_t		dest_realmid;
	kad_peerid_t		dest_peerid;
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);

	// parse the incoming packet	
	try {
		// read the pkttype
		pkt >> pkttype;
		// read the realm id - if realmid_in_pkt_ok()
		if( profile.rpc().realmid_in_pkt_ok() )		pkt >> dest_realmid;
		// get the destination peerid from the incoming packet - if dest_peerid_in_pkt_ok()
		if( profile.rpc().dest_peerid_in_pkt_ok() )	pkt >> dest_peerid; 
	} catch(serial_except_t &e) {
		KLOG_INFO("Cant parse incoming packet due to " << e.what() );
		goto	error;
	}

	// try to find a kad_peer_t matching this realmid/peerid
	// - it works even if they are null
	kad_peer = get_kad_peer(dest_realmid, dest_peerid);
	// if the realm doesnt exist, log and exit
	if( !kad_peer ){
		// log the event
		KLOG_INFO("Received a packet " << pkttype << " for an unexisting realmid=" << dest_realmid
						<< " peerid=" << dest_peerid);
		// send a kad_pkttype_t::RESET_REPLY, if the packet is not itself a RESET_REPLY
		// - similar to "an icmp error MUST NOT be replied to an icmp error" - rfc1812.4.3.2.7
		if( pkttype != pkttype.RESET_REPLY() ){
			// build the kad_pkttype_t::RESET_REPLY
			pkt	= pkt_t();
			pkt << pkttype.RESET_REPLY();
			pkt.append(orig_pkt.void_ptr(), orig_pkt.length());
			// return now
			return true;
		}
		goto error;
	}
	
	// log to debug
	KLOG_DBG("received a pkttype=" << pkttype << " for realmid=" << dest_realmid 
						<< " peerid=" << dest_peerid);
	// forward the packet to the proper realm
	pkt	= kad_peer->handle_request(orig_pkt, local_oaddr, remote_oaddr);
	return true;
error:;
	// swallow the packet as there is no reply
	pkt	= pkt_t();
	// keep the server running anyway
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//             kad_peer_t management function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/** \brief return a kad_peer_t matching the realmid/peerid, or NULL if none match
 * 
 * @param realmid	the kad_realmid_t to match, if null any match
 * @param peerid	the kad_peerid_t to match, if null any match
 */
kad_peer_t *	kad_listener_t::get_kad_peer(const kad_realmid_t &realmid
						, const kad_peerid_t &peerid)	throw()
{
	std::list<kad_peer_t *>::iterator	iter;
	// go thru all the realm
	for( iter = peer_db.begin(); iter != peer_db.end(); iter++ ){
		kad_peer_t *	kad_peer	= *iter;
		// if the realmid is set and this realm doesnt match, skip it
		if( !realmid.is_null() && realmid != kad_peer->get_realmid() )	continue;
		// if the peerid is set and this realm doesnt match, skip it
		// - TODO should be peerid.is_any()
		if( !peerid.is_null() && peerid != kad_peer->local_peerid() )	continue;
		// if this point is reached, the kad_peer_t matches
		return kad_peer;
	}
	// if this point is reached, no kad_peer_t matches
	return NULL;
}

/** \brief return true if this new kad_peer_t is compatible with currently attached one
 */
bool	kad_listener_t::is_peer_compatible(kad_peer_t *kad_peer)		throw()
{
	// if the current peer_db is empty, any new kad_peer is compatible
	if( peer_db.empty() )	return true;

	// get the profile
	const kad_profile_t &	curprof	= peer_db.front()->get_profile();
	const kad_profile_t &	newprof	= kad_peer->get_profile();
	// check if the kad_profile of the new kad_peer is compatible with the current kad_peer_t
	// - the following fields MUST be equal in all kad_profile_t because they are used during the 
	//   packet request parsing in the kad_listener_t.
	//   -# rpc().dest_peerid_in_pkt_ok()
	//   -# rpc()realmid_in_pkt_ok()
	//   -# pkttype()
	if( curprof.rpc().dest_peerid_in_pkt_ok() != newprof.rpc().dest_peerid_in_pkt_ok() )	return false;
	if( curprof.rpc().realmid_in_pkt_ok() != newprof.rpc().realmid_in_pkt_ok() )		return false;
	if( curprof.pkttype() != newprof.pkttype() )						return false;
	
	// check that the new kad_peer_t is distinguishable from the current ones
	// - it uses get_kad_peer() aka the same function used to get the kad_peer_t when
	//   as request packet is received.
	kad_realmid_t	dest_realmid;
	kad_peerid_t	dest_peerid;
	if( curprof.rpc().realmid_in_pkt_ok() )		dest_realmid	= kad_peer->get_realmid();
	if( curprof.rpc().dest_peerid_in_pkt_ok() )	dest_peerid	= kad_peer->local_peerid();
	if( get_kad_peer(dest_realmid, dest_peerid) )	return false;
	
	// if all tests passed the new kad_peer_t is declared compatible
	return true;
}

/** \brief link a kad_peer_t to this kad_listener_t
 * 
 * - check about the compatibility of the core request header option in kad_rpc_profile_t
 */
void	kad_listener_t::peer_link(kad_peer_t *kad_peer)	throw()
{
	// sanity check - the new kad_peer_t MUST be compatible with the currently attached ones
	DBG_ASSERT( is_peer_compatible(kad_peer) );
	
	// init the kad_srvcnx_t here as it need the pkttype_profile_t from the kad_profile_t
	// - as the pkttype_profile_t is the same for all kad_profile_t, init kad_srvcnx_t using
	//   the pkttype_profile_t of the first kas_peer_t
	if( peer_db.empty() ){
		pkttype_profile_t	pkttype_profile = kad_peer->get_profile().pkttype();
		kad_err_t		kad_err;
		// start the server connection using the listen_oaddr_local
		kad_srvcnx	= nipmem_new kad_srvcnx_t();
		kad_err 	= kad_srvcnx->start(udp_vresp, pkttype_profile, this, NULL);
		DBG_ASSERT( kad_err.succeed() );
	}
	
	// add this kad_peer_t to the database
	peer_db.push_back(kad_peer);
}

/** \brief unlink a kad_peer_t to this kad_listener_t
 */
void	kad_listener_t::peer_unlink(kad_peer_t *kad_peer)	throw()
{
	// remove the kad_peer_t from the peer_db
	peer_db.remove( kad_peer);	

	// deinit the kad_srvcnx_t here as it need the pkttype_profile_t from the kad_profile_t
	if( peer_db.empty() )	nipmem_zdelete kad_srvcnx;
}

NEOIP_NAMESPACE_END


