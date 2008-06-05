/*! \file
    \brief Definition of the ntudp_peer_t

- TODO change all the _addr for ipport_addr_t into _iaddr for inner address
  - same principle as in kad
  
\par About profile.peer().no_npos_watch() and chicken-egg in ntudp bootrapping
If this option is set, the ntudp_peer_t uses a special a way to determine 
the network position, usually done via ntudp_npos_watch_t.
This special case is required to solve a chicken-egg issue:
- To be able to determine the network position the usual way via ntudp_npos_watch_t
  requires some public server ntudp_pserver_t to help the testing.
- BUT to become public server ntudp_pserver_t, a ntudp_peer_t MUST be declared
  inetreach ok. So having a ntudp_npos_watch_t declaring the ntudp_peer_t as 
  such.
  - so to become a ntudp_pserver_t, other ntudp_pserver_t are required. This 
    is the chiken-egg problem.
- This chicken-egg problem is solved by using a special way to determine the 
  network position for the 'first' ntudp_pserver_t
  - in short, this is deduced from the listening ipport_aview_t based on 
    assumption.
  - those assumptions are ok ONLY because it is supposed to be checked by 
    the user running ntdup-bstrapntudp which is supposed to be an experienced
    one.

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_peer_rec.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_ntudp_npos_server.hpp"
#include "neoip_ntudp_npos_watch.hpp"
#include "neoip_ntudp_pserver.hpp"
#include "neoip_ntudp_relpeer.hpp"
#include "neoip_ntudp_aview_pool.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_kad.hpp"
#include "neoip_kad_publish.hpp"
#include "neoip_ndiag_aview.hpp"
#include "neoip_ndiag_err.hpp"
#include "neoip_netif_addr_watch.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_ntudp_peer_wikidbg.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_peer_t::ntudp_peer_t()				throw()
{
	// log to debug
	KLOG_ERR("enter");
	// zero some parmeters
	m_udp_vresp	= NULL;
	m_kad_listener	= NULL;
	m_kad_peer	= NULL;
	m_ndiag_aview	= NULL;
	
	kad_publish	= NULL;

	m_pserver_pool	= NULL;
	
	m_npos_server	= NULL;
	m_npos_watch	= NULL;

	ntudp_pserver	= NULL;
	ntudp_relpeer	= NULL;
	netif_addr_watch= NULL;
	m_aview_pool	= NULL;
}

/** \brief destructor
 */
ntudp_peer_t::~ntudp_peer_t()				throw()
{
	// log to debug
	KLOG_ERR("enter");
	// notify the event_hook that the ntudp_peer_t gonna deleted
	event_hook.notify(HOOK_PRE_PEER_DELETION);
	// close all pending ntudp_resp_t
	DBGNET_ASSERT( ntudp_resp_db.empty() );
	while( !ntudp_resp_db.empty() )	nipmem_delete ntudp_resp_db.front();
	// close all pending ntudp_itor_t
	DBGNET_ASSERT( ntudp_itor_db.empty() );
	while( !ntudp_itor_db.empty() )	nipmem_delete ntudp_itor_db.front();
	// close all pending ntudp_full_t
	DBGNET_ASSERT( ntudp_full_db.empty() );
	while( !ntudp_full_db.empty() )	nipmem_delete ntudp_full_db.front();

	// delete the aview_pool if needed
	nipmem_zdelete m_aview_pool;
	// delete the ntudp_server if needed
	nipmem_zdelete ntudp_pserver;
	// delete the ntudp_relpeer if needed
	nipmem_zdelete ntudp_relpeer;
	// delete the netif_addr_watch
	nipmem_zdelete netif_addr_watch;
	// delete the ntudp_npos_watch_t if needed
	nipmem_zdelete m_npos_watch;
	// delete the ntudp_npos_server_t if needed
	nipmem_zdelete m_npos_server;
	// delete the pserver_pool if needed
	nipmem_zdelete m_pserver_pool;	
	// delete the kad_publish_t if needed
	nipmem_zdelete kad_publish;
	// delete the kad_peer_t if needed
	nipmem_zdelete m_kad_peer;
	// delete the kad_listener_t if needed
	nipmem_zdelete m_kad_listener;
	// delete the udp_vresp_t if needed
	nipmem_zdelete m_udp_vresp;
	// delete the ndiag_aview_t if needed
	nipmem_zdelete m_ndiag_aview;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            start() functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the ntudp_rdvpt_ftor_t
 * 
 * - NOTE: may be used before AND after being started
 */
ntudp_peer_t &	ntudp_peer_t::set_rdvpt_ftor(const ntudp_rdvpt_ftor_t &rdvpt_ftor)	throw()
{
	// copy the parameter
	this->rdvpt_ftor	= rdvpt_ftor;
	// if the ntudp_peer_t is already started, republish the peer record
	if( is_started() )	publish_peer_rec();
	// return the object itself
	return *this;
}

/** \brief Set the profile for this object
 */
ntudp_peer_t &	ntudp_peer_t::set_profile(const ntudp_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the actions
 */
ntudp_err_t	ntudp_peer_t::start(const ipport_aview_t &m_listen_aview_cfg
					, const ntudp_npos_res_t &init_npos_res
					, const ntudp_peerid_t &m_local_peerid)	throw()
{
	ntudp_err_t	ntudp_err;
	inet_err_t	inet_err;
	netif_err_t	netif_err;

	// sanity check - the udp_vresp_t MUST be NULL
	DBG_ASSERT( m_udp_vresp == NULL );
	// sanity check - m_local_peerid MUST be fully qualified
	DBG_ASSERT( m_local_peerid.is_fully_qualified() );
	// snaity check - init_npos_res MUST be completed
	DBG_ASSERT( init_npos_res.completed() );
	// sanity check - the m_listen_aview_cfg MUST NOT be is_cfg_ok()
	DBG_ASSERT( m_listen_aview_cfg.is_cfg_ok() );
	
	// copy the parameter
	this->m_listen_aview_cfg= m_listen_aview_cfg;
	this->m_local_peerid	= m_local_peerid;
	this->current_npos_res	= init_npos_res;

	// initialize the event_hook_t
	event_hook	= event_hook_t(HOOK_MAX);
	
	// create the udp_vresp
	m_udp_vresp	= nipmem_new udp_vresp_t();
	inet_err 	= udp_vresp()->start(listen_aview_cfg().lview());
	// if the udp_vresp_t failed, report the error
	if( inet_err.failed() )	return ntudp_err_from_inet(inet_err);

	// update the listen_aview_udp in case udp_vresp_t did dynamic binding
	this->m_listen_aview_cfg.update_once_bound	( udp_vresp()->get_listen_addr() );

	// create a ndiag_aview_t for udp
	ndiag_err_t	ndiag_err;
	m_ndiag_aview	= nipmem_new ndiag_aview_t();
	ndiag_err	= ndiag_aview()->start(listen_aview_cfg(), upnp_sockfam_t::UDP
					, "neoip ntudp UDP", this, NULL);
	if( ndiag_err.failed() )		return ntudp_err_from_ndiag(ndiag_err);
		
	// init kad_listener
	kad_err_t	kad_err;
	m_kad_listener	= nipmem_new kad_listener_t();
	// TODO this one should NOT use listen_aview_cfg as it may have been updated by ndiag_aview_t
	kad_err 	= kad_listener()->start("255.255.255.255:7777", udp_vresp(), listen_aview_cfg());
	if( kad_err.failed() )	return ntudp_err_from_kad(kad_err);
	

	// get a kad_profile_t and make it suitable for udp_vresp_t
	kad_profile_t	kad_profile;
	kad_profile.pkttype( pkttype_profile_t(NEOIP_NTUDP_PKTTYPE_KAD_OFFSET+ 1 /* +1 for the NONE which no more exist */
						, NEOIP_NTUDP_PKTTYPE_KAD_MAX_NB_PACKET - 1 
						, pkttype_profile_t::UINT8) );
	// set some variables for kad_peer_t
	kad_realmid_t	kad_realmid	= "kad_realm_ntudp";
	kad_peerid_t	kad_peerid	= kad_peerid_t::build_random();
	m_kad_peer	= nipmem_new kad_peer_t();
	kad_err		= kad_peer()->set_profile(kad_profile).start(kad_listener(), kad_realmid, kad_peerid);
	if( kad_err.failed() )		return ntudp_err_from_kad(kad_err);

	// create the ntudp_pserver_pool_t and start it	
	m_pserver_pool	= nipmem_new ntudp_pserver_pool_t();
	ntudp_err	= pserver_pool()->set_profile(profile.pserver_pool()).start(this);
	if( ntudp_err.failed() )	return ntudp_err;
	
	// create the ntudp_npos_server_t
	m_npos_server	= nipmem_new ntudp_npos_server_t();
	ntudp_err	= npos_server()->start(udp_vresp());
	if( ntudp_err.failed() )	return ntudp_err;

	// to create the ntudp_npos_watch_t IIF profile.peer().no_npos_watch() is false
	if( !profile.peer().no_npos_watch() ){
		m_npos_watch	= nipmem_new ntudp_npos_watch_t();
		ntudp_err	= npos_watch()->set_profile(profile.npos_watch())
						.start(this, this, NULL);
		if( ntudp_err.failed() )	return ntudp_err;
	}

#ifndef _WIN32	// NOTE: currently libneoip_netif doesnt compile on _WIN32
	// start a netif_addr_watch if listen_addr_lview_cfg has ANY as ip address
	// - it allows to build the destination address from the listen_addr_lview
	if( listen_addr_lview().ipaddr().is_any() ){
		netif_addr_watch= nipmem_new netif_addr_watch_t();
		netif_err	= netif_addr_watch->start(this, NULL);
		if( netif_err.failed() )	return ntudp_err_from_netif(netif_err);		
	}
#else
	KLOG_ERR("libneoip_netif REQUIRED. and doesnt yet compile on _WIN32");
	return ntudp_err_t(ntudp_err_t::ERROR, "libneoip_netif REQUIRED. and doesnt yet compile on _WIN32");
	EXP_ASSERT(0);
#endif
	// initial launch of the services depending on the current_npos_res
	if( current_npos_res.inetreach() )	become_inetreach_ok();
	else					become_inetreach_ko();
	if( current_npos_res.natted() )		become_natted_ok();
	else					become_natted_ko();

	// publish the peer record on the underlying NS
	publish_peer_rec();

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/ntudp_peer_" + local_peerid().to_string());

	// return no error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the local view of the listening address
 * 
 * - the listen_addr_lview will likely listen on a ANY:port
 *   - this allows to receives connection from any connected LAN
 * - the listen_addr_lview ALWAYS have a qualified port and MAY have a qualified 
 *   ip address (if the listen_addr_lview_cfg specify it).
 */
const ipport_addr_t &	ntudp_peer_t::listen_addr_lview()	const throw()
{
	// sanity check - the port MUST be set 
	// - ok it is always set due to udp but this line remind me of it :)
	DBG_ASSERT( udp_vresp()->get_listen_addr().port() != 0 );
	// return the address itself
	return udp_vresp()->get_listen_addr();
}


/** \brief return the current public view of the listening address
 * 
 * - the *current* is NOT to be confused with the *configuration* one
 * - this is only usefull in case of inetreach
 */
ipport_addr_t	ntudp_peer_t::listen_addr_pview()	const throw()
{
	// sanity check - the listen_addr_pview is availlable IIF the peer is inetreach
	DBG_ASSERT( current_npos_res.inetreach() );
	// sanity check - the listen_aview_cfg.pview() MUST qualify a port
	DBG_ASSERT( listen_aview_cfg().pview().port() != 0 );

	// if the listen_aview_cfg.pview() doesnt qualify the ipaddr, get it from the current_npos_res
	ipport_addr_t	listen_addr_pview = listen_aview_cfg().pview();
	if( listen_addr_pview.ipaddr().is_any() ){
		// sanity check - a inetreach OK ntudp_npos_res_t MUST qualify the ip address
		DBG_ASSERT( current_npos_res.local_ipaddr_pview().is_null() == false );
		// set the ip address from the local_ipaddr_view of the current_npos_res
		listen_addr_pview.ipaddr( current_npos_res.local_ipaddr_pview() );
	}
	// sanity check - a listen_addr_pview MUST be fully qualified
	DBG_ASSERT( listen_addr_pview.is_fully_qualified() );
	
	// return the result
	return listen_addr_pview;
}

/** \brief Return all the rendez point for this peer
 */
ntudp_rdvpt_arr_t 	ntudp_peer_t::get_rdvpt_arr()		const throw()
{
	ntudp_rdvpt_t		rdvpt;
	ntudp_rdvpt_arr_t	rdvpt_arr;
	
	// sanity check - the listen_addr_lview post MUST be set
	DBG_ASSERT( listen_addr_lview().port() );
	// if the listen_addr_lview is fully_qualified, copy the listen_addr_lview in the rdvpt_arr
	if( listen_addr_lview().is_fully_qualified() ){
		// add the listen_addr_lview as DIRECT
		rdvpt_arr	+= ntudp_rdvpt_t(listen_addr_lview(), ntudp_rdvpt_t::DIRECT);
	}else{
		// sanity check - the listen_addr_lview ipaddr MUST be ANY
		DBG_ASSERT( listen_addr_lview().ipaddr().is_any() );
#ifndef _WIN32
		// if the listen_addr_lview ip address is ANY, build all destination addresses from netif
		netif_addr_arr_t netif_addr_arr = netif_addr_t::get_all_netif();
		for(size_t i = 0; i < netif_addr_arr.size(); i++){
			ip_addr_t	netif_addr = netif_addr_arr[i].get_ip_netaddr().get_base_addr();
			// build the destination address for this netif 
			// - the ip address is the base address of this netif
			// - the port is the one of the listen_addr_lview
			ipport_addr_t	dest_addr(netif_addr, listen_addr_lview().port() );
			// add this destination address as DIRECT
			rdvpt_arr	+= ntudp_rdvpt_t(dest_addr, ntudp_rdvpt_t::DIRECT);
		}
#else
		EXP_ASSERT(0);
#endif
	}
	
	// if the local peer is inetreach OK, add the listen_addr_pview to the rdvpt_arr
	if( current_npos_res.inetreach() ){
		rdvpt_arr	+= ntudp_rdvpt_t(listen_addr_pview(), ntudp_rdvpt_t::DIRECT);
	}else{
		// if the local peer is inetreach KO, add the pserver_addr of ntudp_relpeer_t tunnels
		DBG_ASSERT( ntudp_relpeer );
		rdvpt_arr	+= ntudp_relpeer->get_established_tunnel_rdvpt();
	}
	
	// pass the result thru the ntudp_rdvpt_ftor_t if there is any
	ntudp_rdvpt_arr_t	rdvpt_arr_copy;
	// go thru the whole rdvpt_arr - a copy is needed as it is 'delete while walking'
	for(size_t i = 0; i < rdvpt_arr.size(); i++){
		// if the filter ask to remove this item, skip it
		if( rdvpt_ftor(rdvpt_arr[i]) )	continue;
		// else copy it
		rdvpt_arr_copy += rdvpt_arr[i];
	}
	// copy back the result
	rdvpt_arr	= rdvpt_arr_copy;
	
	// return the result
	return rdvpt_arr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function for connection
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return ntudp_resp_t listening of this listen_portid, or NULL if none is found
 */
ntudp_resp_t *	ntudp_peer_t::resp_from_portid(const ntudp_portid_t &listen_portid)	throw()
{
	std::list<ntudp_resp_t *>::iterator	iter;
	// go thru all the ntudp_resp_db
	for( iter = ntudp_resp_db.begin(); iter != ntudp_resp_db.end(); iter++ ){
		ntudp_resp_t *	ntudp_resp	= *iter;
		// if this ntudp_resp is listening on the proper portid, return it
		if( listen_portid == ntudp_resp->listen_addr().portid() )
			return ntudp_resp;
	}
	// return NULL as none has been found.
	return NULL;
}

/** \brief Return ntudp_itor_t matching this connection address, or NULL if none is found
 */
ntudp_itor_t *	ntudp_peer_t::itor_from_cnxaddr(const ntudp_addr_t &local_addr
							, const ntudp_addr_t &remote_addr)	throw()
{
	std::list<ntudp_itor_t *>::iterator	iter;
	// go thru all the ntudp_itor_db
	for( iter = ntudp_itor_db.begin(); iter != ntudp_itor_db.end(); iter++ ){
		ntudp_itor_t *	ntudp_itor	= *iter;
		// if this ntudp_itor_t doesnt matching the connection address, skip it
		if( local_addr  != ntudp_itor->local_addr() )	continue;
		if( remote_addr != ntudp_itor->remote_addr() )	continue;
		// if this point is reached, the ntudp_itor_t matches, so return its pointer
		return ntudp_itor;
	}
	// return NULL as none has been found.
	return NULL;
}

/** \brief Return ntudp_full_t matching this connection address, or NULL if none is found
 */
ntudp_full_t *	ntudp_peer_t::full_from_cnxaddr(const ntudp_addr_t &local_addr
							, const ntudp_addr_t &remote_addr)	throw()
{
	std::list<ntudp_full_t *>::iterator	iter;
	// go thru all the ntudp_full_db
	for( iter = ntudp_full_db.begin(); iter != ntudp_full_db.end(); iter++ ){
		ntudp_full_t *	ntudp_full	= *iter;
		// if this ntudp_full_t doesnt matching the connection address, skip it
		if( local_addr  != ntudp_full->local_addr() )	continue;
		if( remote_addr != ntudp_full->remote_addr() )	continue;
		// if this point is reached, the ntudp_full_t matches, so return its pointer
		return ntudp_full;
	}
	// return NULL as none has been found.
	return NULL;
}

/** \brief Return true is this connection address is bound, false otherwise
 */
bool	ntudp_peer_t::cnxaddr_is_bound(const ntudp_addr_t &local_addr, const ntudp_addr_t &remote_addr) throw()
{
	// if a ntudp_full_t already uses it, return inuse
	if( full_from_cnxaddr(local_addr, remote_addr) )	return true;
	// if a ntudp_itor_t already uses it, return inuse
	if( itor_from_cnxaddr(local_addr, remote_addr) )	return true;
	// else return not inuse
	return false;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			handle the ntudp_peer_rec_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief publish the peer record 
 */
void	ntudp_peer_t::publish_peer_rec()	throw()
{
	// if the peer record was already published, delete the kad_publish
	nipmem_zdelete kad_publish;

	// build the peer record
	ntudp_peer_rec_t	peer_rec(local_peerid(), get_rdvpt_arr(), current_npos_res);
	
	// log to debug
	KLOG_DBG("publishing peer record=" << peer_rec);
	
	// build the kad_rec_t out of it
	bytearray_t	payload	= bytearray_t().serialize(peer_rec);
	kad_keyid_t	keyid	= local_peerid().to_canonical_string();
	kad_recid_t	recid	= kad_recid_t::build_random();
	delay_t		ttl	= profile.peer().peer_record_ttl();
	kad_rec_t	kad_rec = kad_rec_t(recid, keyid, ttl, payload.to_datum());

	// publish the just-built kad_rec_t
	kad_err_t	kad_err;
	kad_publish	= nipmem_new kad_publish_t();
	kad_err		= kad_publish->start(kad_peer(), kad_rec, this, NULL);
	if( kad_err.failed() )
		KLOG_ERR("Peer record publication failed due to " << kad_err);
}

/** \brief callback notified when a kad_publish_t has an event to notify
 */
bool ntudp_peer_t::neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
						, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the kad_even_t MUST be publish_ok()
	DBG_ASSERT( kad_event.is_publish_ok() );

	// NOTE: this function is only a place-holder for the kad_publish of the peer record

	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ndiag_aview_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ndiag_aview_t to notify an event
 */
bool 	ntudp_peer_t::neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
						, const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter NOT YET IMPLEMENTED new_listen_pview=" << new_listen_pview);

	// update the kad_listener listen_pview
	// - NOTE: up to kad_listener_t to update the kad_peer_t's
	kad_listener()->update_listen_pview(new_listen_pview);
	
	// update the m_listen_aview
	m_listen_aview_cfg.pview	(new_listen_pview);

	// reevaluate the ntudp_npos_res_t with for this new listen_aview_cfg()
	if( !profile.peer().no_npos_watch() ){
		// trigger a ntudp_pos_res_t evaluation as soon as possible
		npos_watch()->trigger_eval_asap();
	}
	
	// update the local ntudp_npos_res_t with the new result
	update_npos_res();

	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     ntudp_npos_watch_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_npos_watch_t when the ntudp_npos_res_t changes
 */
bool ntudp_peer_t::neoip_ntudp_npos_watch_cb(void *cb_userptr, ntudp_npos_watch_t &cb_ntudp_npos_watch
					, const ntudp_npos_res_t &new_npos_res)	throw()
{
	// log to debug
	KLOG_ERR("enter new_npos_res=" << new_npos_res);
	// handle the new ntudp_npos_res_t
	update_npos_res();
	// return tokeep
	return true;
}

/** \brief update the local ntudp_npos_res_t with the various sources
 */
void	ntudp_peer_t::update_npos_res()	throw()
{
	ntudp_npos_res_t	old_npos_res	= current_npos_res;
	// log to debug
	KLOG_DBG("enter res=" << new_npos_res);

	// update the current_npos_res depending on the available network position result
	// - available from ntudp_npos_watch_t or ndiag_aview_t
	if( m_npos_watch && m_npos_watch->npos_res().is_null() == false ){
		// if npos_watch_t has a non null ntudp_npos_res_t, use it
		current_npos_res	= m_npos_watch->npos_res(); 
	}else{
		// else take it from ndiag_aview_t
		// - TODO is this goot to do the liten_aview_run() ?
		current_npos_res	= ntudp_npos_res_t::from_ipport_aview(listen_aview_cfg());
	}

	// adapt the services to the new network position	
	if( !old_npos_res.inetreach() &&  current_npos_res.inetreach())	become_inetreach_ok();
	if(  old_npos_res.inetreach() && !current_npos_res.inetreach())	become_inetreach_ko();
	if( !old_npos_res.natted()    &&  current_npos_res.natted() )	become_natted_ok();
	if(  old_npos_res.natted()    && !current_npos_res.natted() )	become_natted_ko();

	// republish the peer record on the underlying NS
	publish_peer_rec();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     netif_addr_watch_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when netif_addr_watch_t has an event to report
 */
bool ntudp_peer_t::neoip_netif_addr_watch_cb(void *cb_userptr
						, netif_addr_watch_t &cb_netif_addr_watch
						, const netif_addr_arr_t &netif_addr_arr)	throw()
{
	// log to debug 
	KLOG_DBG("enter");
	// sanity check - netif_addr_watch_t MUST be running IIF listen_addr_lview ip addr is ANY
	DBG_ASSERT( listen_addr_lview().ipaddr().is_any() );

	// republish the peer record on the underlying NS
	publish_peer_rec();
		
	// return 'tokeep'
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     ntudp_relpeer_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when netif_addr_watch_t has an event to report
 */
bool ntudp_peer_t::neoip_ntudp_relpeer_cb(void *cb_userptr, ntudp_relpeer_t &cb_ntudp_relpeer)	throw()
{
	// log to debug 
	KLOG_ERR("enter");
	// sanity check - the ntudp_relpeer MUST NOT be null
	DBG_ASSERT( ntudp_relpeer != NULL );

	// republish the peer record on the underlying NS
	publish_peer_rec();
	
	// return 'tokeep'
	return true;
}
					
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//              Adaptation to the network location of this peer
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to be called when the local peer become inetreach OK
 * 
 * -# free the ntudp_relpeer if needed
 * -# init the ntudp_server
 */
ntudp_err_t	ntudp_peer_t::become_inetreach_ok()	throw()
{
	ntudp_err_t	ntudp_err;

	// delete ntudp_relpeer_t if needed
	nipmem_zdelete ntudp_relpeer;

	// launch ntudp_pserver_t 
	DBG_ASSERT( ntudp_pserver == NULL );
	ntudp_pserver	= nipmem_new ntudp_pserver_t();
	ntudp_err	= ntudp_pserver->set_profile(profile.pserver()).start(this);
	if( ntudp_err.failed() )	return ntudp_err;
	
	// return no error
	return ntudp_err_t::OK;
}

/** \brief to be called when the local peer become inetreach KO
 * 
 * -# free the ntudp_pserver if needed
 * -# init the ntudp_relpeer
 */
ntudp_err_t	ntudp_peer_t::become_inetreach_ko()	throw()
{
	ntudp_err_t	ntudp_err;

	// delete ntudp_pserver_t if needed
	nipmem_zdelete ntudp_pserver;
	
	// launch ntudp_relpeer_t 
	DBG_ASSERT( ntudp_relpeer == NULL );
	ntudp_relpeer	= nipmem_new ntudp_relpeer_t();
	ntudp_err	= ntudp_relpeer->set_profile(profile.relpeer()).start(this, this, NULL);
	if( ntudp_err.failed() )	return ntudp_err;
	
	// return no error
	return ntudp_err_t::OK;
}

/** \brief to be called when the local peer become natted OK
 * 
 * -# init the ntudp_aview_pool_t
 */
ntudp_err_t	ntudp_peer_t::become_natted_ok()	throw()
{
	ntudp_err_t	ntudp_err;

	// launch ntudp_aview_pool_t 
	DBG_ASSERT( m_aview_pool == NULL );
	m_aview_pool	= nipmem_new ntudp_aview_pool_t();
	ntudp_err	= aview_pool()->start(this, profile.peer().nb_needed_aview_init());
	if( ntudp_err.failed() )	return ntudp_err;
	// return no error
	return ntudp_err_t::OK;
}

/** \brief to be called when the local peer become natted KO
 * 
 * -# free the ntudp_aview_pool_t
 */
ntudp_err_t	ntudp_peer_t::become_natted_ko()	throw()
{
	// delete the aview_pool and mark it unused
	nipmem_zdelete	m_aview_pool;
	// return no error
	return ntudp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         hook event
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief hook an event in \ref ntudp_peer_t
 */
void ntudp_peer_t::event_hook_append(int level_no, event_hook_cb_t *callback, void *userptr) throw()
{
	event_hook.append(level_no, callback, userptr);
}

/** \brief unhook an event in \ref ntudp_peer_t
 */
void ntudp_peer_t::event_hook_remove(int level_no, event_hook_cb_t *callback, void *userptr) throw()
{
	event_hook.remove(level_no, callback, userptr);
}


NEOIP_NAMESPACE_END



