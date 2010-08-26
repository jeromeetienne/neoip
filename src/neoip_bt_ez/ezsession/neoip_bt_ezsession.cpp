/*! \file
    \brief Class to handle the bt_ezsession_t

\par Brief Description
\ref bt_ezsession_t is a helper on top of bt_session_t which handle all the 
'sattelites' classes usually used around a bt_session_t.
- additionnaly it handle a ndiag_aview_t for udp and tcp

*/

/* system include */
/* local include */
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_io_cache_pool.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_kad.hpp"
#include "neoip_nslan.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_ndiag_aview.hpp"
#include "neoip_ndiag_err.hpp"
#include "neoip_rate_sched.hpp"
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
bt_ezsession_t::bt_ezsession_t()		throw()
{
	// zero some field
	m_bt_session	= NULL;
	m_io_cache_pool	= NULL;
	m_ndiag_aview_tcp= NULL;
	// zero stuff for peersrc_nslan
	m_nslan_listener= NULL;
	m_nslan_peer	= NULL;
	// zero stuff for peersrc_kad
	m_udp_vresp	= NULL;
	m_kad_listener	= NULL;
	m_kad_peer	= NULL;
	m_ndiag_aview_udp= NULL;
	// zero stuff for xmit_rsched and recv_rsched
	m_xmit_rsched	= NULL;
	m_recv_rsched	= NULL;
}

/** \brief Destructor
 */
bt_ezsession_t::~bt_ezsession_t()		throw()
{
	// sanity check - the ezswarm_db MUST be empty at this point
	DBG_ASSERT( ezswarm_db().empty() );

	// delete the nslan_peer_t
	nipmem_zdelete	m_nslan_peer;
	// delete the nslan_listener_t
	nipmem_zdelete	m_nslan_listener;
	
	// delete the ndiag_aview_tcp
	nipmem_zdelete	m_ndiag_aview_tcp;
	// delete bt_session if needed
	nipmem_zdelete	m_bt_session;
	// delete bt_io_cache_pool_t if needed
	nipmem_zdelete	m_io_cache_pool;

	// delete the ndiag_aview_udp
	nipmem_zdelete	m_ndiag_aview_udp;
	// delete the kad_peer_t if needed
	nipmem_zdelete	m_kad_peer;
	// delete the kad_listener_t if needed
	nipmem_zdelete	m_kad_listener;
	// delete the udp_vresp_t if needed
	nipmem_zdelete	m_udp_vresp;

	// delete the xmit_rsched if needed
	nipmem_zdelete	m_xmit_rsched;
	// delete the recv_rsched if needed
	nipmem_zdelete	m_recv_rsched;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_ezsession_t &bt_ezsession_t::profile(const bt_ezsession_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation when there is a bt_swarm_resumedata_t
 */
bt_err_t	bt_ezsession_t::start(const bt_ezsession_opt_t &p_ezsession_opt
						, const ipport_aview_t &listen_aview_tcp_orig
						, const ipport_aview_t &listen_aview_udp_orig
						, const kad_peerid_t &kad_peerid
						, const bt_id_t &local_peerid)	throw()
{
	bt_err_t	bt_err;
	ipport_aview_t	listen_aview_tcp	= listen_aview_tcp_orig;
	ipport_aview_t	listen_aview_udp	= listen_aview_udp_orig;
	bt_ezsession_opt_t	ezsession_opt	= p_ezsession_opt;
	// log to debug 
	KLOG_DBG("enter");
	KLOG_ERR("listen_aview_tcp_orig=" << listen_aview_tcp_orig);

	// start the kad_peer stuff if needed
	if( ezsession_opt.is_kad_peer() ){
		// create the udp_vresp
		inet_err_t	inet_err;
		m_udp_vresp	= nipmem_new udp_vresp_t();
		inet_err 	= udp_vresp()->start(listen_aview_udp.lview());
		if( inet_err.failed() )		return bt_err_from_inet(inet_err);
		// update the listen_aview_udp in case udp_vresp_t did dynamic binding
		listen_aview_udp.update_once_bound	( udp_vresp()->get_listen_addr() );
		// create a ndiag_aview_t for udp
		ndiag_err_t	ndiag_err;
		m_ndiag_aview_udp= nipmem_new ndiag_aview_t();
		ndiag_err	= m_ndiag_aview_udp->start(listen_aview_udp, upnp_sockfam_t::UDP
						, "neoip bt UDP", this, NULL);
		if( ndiag_err.failed() )	return bt_err_from_ndiag(ndiag_err);
		// update the listen_aview_udp with the ipport_pview_curr()
		// - TODO poorly written - do a ndiag_aview_t->current_aview or something
		// - close to the 'bad naming' tendency i got recently
		listen_aview_udp.pview	(ndiag_aview_udp()->ipport_pview_curr());		
		// init kad_listener
		kad_err_t	kad_err;
		m_kad_listener	= nipmem_new kad_listener_t();
		kad_err 	= kad_listener()->start(profile().nslan_addr(), udp_vresp(), listen_aview_udp);
		if( kad_err.failed() )		return bt_err_from_kad(kad_err);
		// set some variables for kad_peer_t
		m_kad_peer	= nipmem_new kad_peer_t();
		kad_err		= kad_peer()->start(kad_listener(), profile().kad_realmid(), kad_peerid);
		if( kad_err.failed() )		return bt_err_from_kad(kad_err);
	}

	// start the nslan_peer stuff if needed
	// - NOTE: IIF !ezsession_opt.is_kad_peer(), because else it use kad_peer_t one
	// - see bt_ezsession_t::nslan_peer() function for details
	if( ezsession_opt.is_nslan_peer() && !ezsession_opt.is_kad_peer() ){
		inet_err_t	inet_err;
		// create a nslan_listener_t
		m_nslan_listener= nipmem_new nslan_listener_t();
		inet_err	= m_nslan_listener->start(profile().nslan_addr());
		if( inet_err.failed() )	return bt_err_from_inet(inet_err);
		// create a nslan_peer_t
		m_nslan_peer	= nipmem_new nslan_peer_t(m_nslan_listener, profile().nslan_realmid());
	}

	// start a rate_sched_t for xmit if needed
	if( ezsession_opt.is_xmit_rsched() ){
		rate_err_t	rate_err;
		m_xmit_rsched	= nipmem_new rate_sched_t();
		rate_err	= xmit_rsched()->start(profile().xmit_maxrate());
		if( rate_err.failed() )	return bt_err_from_rate(rate_err);
	}

	// start a rate_sched_t for recv if needed
	if( ezsession_opt.is_recv_rsched() ){
		rate_err_t	rate_err;
		m_recv_rsched	= nipmem_new rate_sched_t();
		rate_err	= recv_rsched()->start(profile().recv_maxrate());
		if( rate_err.failed() )	return bt_err_from_rate(rate_err);
	}

	// start a bt_io_cache_pool_t if needed
	if( ezsession_opt.is_use_io_cache() ){
		m_io_cache_pool	= nipmem_new bt_io_cache_pool_t();
		bt_err		= io_cache_pool()->profile(profile().io_cache_pool()).start();
		if( bt_err.failed() )	return bt_err;
	}

	// start the bt_session_t
	m_bt_session	= nipmem_new bt_session_t();
	KLOG_ERR("listen_aview_tcp=" << listen_aview_tcp);
	bt_err		= bt_session()->profile(m_profile.session()).start(listen_aview_tcp, local_peerid);
	if( bt_err.failed() )		return bt_err;

	// update the listen_aview_tcp in case tcp_vresp_t did dynamic binding
	listen_aview_tcp.update_once_bound( bt_session()->listen_lview() ); 
	// create a ndiag_aview_t for tcp
	ndiag_err_t	ndiag_err;
	m_ndiag_aview_tcp= nipmem_new ndiag_aview_t();	
	ndiag_err	= ndiag_aview_tcp()->start(listen_aview_tcp, upnp_sockfam_t::TCP
							, "neoip bt TCP", this, NULL);
	if( ndiag_err.failed() )	return bt_err_from_ndiag(ndiag_err);
	// notify bt_session_t immediatly if ndiag_aview_tcp has a different ipport_pview_curr()
	if( bt_session()->listen_pview() != ndiag_aview_tcp()->ipport_pview_curr() )
		bt_session()->update_listen_pview( ndiag_aview_tcp()->ipport_pview_curr() );


	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/bt_ezsession_" + OSTREAMSTR(this));
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the bt_ezswarm_t matching this infohash, or NULL is none is found
 */
bt_ezswarm_t *	bt_ezsession_t::bt_ezswarm_by_infohash(const bt_id_t &infohash)	const throw()
{
	ezswarm_db_t::const_iterator	iter;
	// go thru the whole ezswarm_db
	for(iter = ezswarm_db().begin(); iter != ezswarm_db().end(); iter++){
		bt_ezswarm_t *	bt_ezswarm	= *iter;
		// if the infohash of this bt_ezswarm_t matches, return the pointer
		if( bt_ezswarm->mfile().infohash() == infohash )	return bt_ezswarm;
	}
	// if this point is reached, no match have been found, return NULl
	return NULL;
}

/** \brief return a pointer to nslan_peer_t of this bt_ezsession_t
 * 
 * - i first coded it by using the nslan_peer_t of kad_listener_t
 *   - i dont remember why... 
 *   - maybe to avoid 2 nslan_listener_t ? well sure but why ?
 * - but then i wanted to get nslan_peer_t without having a kad_listener_t
 *   - to get bt_ezswarm_opt_t::PEERSRC_NSLAN without having bt_ezswarm_opt_t::PEERSRC_KAD
 * - so i keep using nslan_peer_t of kad_listener_t if possible
 * - but if no kad_listener_t, create a nslan_listener_t and nslan_peer_t
 * 
 * - in fact in neoip-webpeer, there is 3 nslan_listener_t. 
 *   - one for each apps oload, casto, casti
 *
 * - TODO to change in a cleaner manner
 */
nslan_peer_t *	bt_ezsession_t::nslan_peer()	const throw()
{
	// sanity check - if there is a m_kad_peer, then m_nslan_peer MUST NOT be init 
	if( m_kad_peer )	DBG_ASSERT( !m_nslan_peer );

	// if there is a m_kad_peer, return the kad_peer_t nslan_peer_t
	if( m_kad_peer )	return m_kad_peer->get_listener()->nslan_peer();
	// if there is a nslan_peer_t return it
	if( m_nslan_peer )	return m_nslan_peer;
	// sanity check - this case should not happen
	DBG_ASSERT(0);	
	// return NULL
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ndiag_aview_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ndiag_aview_t to notify an event
 */
bool 	bt_ezsession_t::neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
						, const ipport_addr_t &new_listen_pview)	throw()
{
	// forward the callback to the proper sub-callback
	if( m_ndiag_aview_udp == &cb_ndiag_aview )	return ndiag_aview_udp_cb(cb_userptr, cb_ndiag_aview, new_listen_pview);
	if( m_ndiag_aview_tcp == &cb_ndiag_aview )	return ndiag_aview_tcp_cb(cb_userptr, cb_ndiag_aview, new_listen_pview);
	// sanity check - this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return true;
}

/** \brief callback notified by ndiag_aview_udp to notify an event
 * 
 * - NOTE about the order of notification
 *   - bt_ezsession_t notify the kad_listener_t
 *   - up to the kad_listener_t to update the attached kad_peer_t's
 */
bool 	bt_ezsession_t::ndiag_aview_udp_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
						, const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter new_listen_pview=" << new_listen_pview);
	// update the kad_listener listen_pview
	// - up to kad_listener_t to update the attached kad_peer_t's
	kad_listener()->update_listen_pview(new_listen_pview);
	// return tokeep
	return true;
}

/** \brief callback notified by ndiag_aview_tcp to notify an event
 * 
 * - NOTE about the order of notification
 *  - bt_ezsession update the bt_session_t and bt_ezswarm_t
 *  - bt_session_t update the bt_swarm_t
 *  - bt_ezswarm_t update all the satellite class BUT NOT bt_swarm_t
 */
bool 	bt_ezsession_t::ndiag_aview_tcp_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
						, const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter new_listen_pview=" << new_listen_pview);
	// update the bt_session_t listen_pview
	// - NOTE: up to bt_session_t to update the bt_swarm_t linked to it
	bt_session()->update_listen_pview(new_listen_pview);
	// update all the bt_ezswarm_t listen_pview
	// - NOTE: they only update the satellites class as bt_swarm_t is 
	//   already updated by bt_session_t
	ezswarm_db_t::const_iterator	iter;
	// go thru the whole ezswarm_db
	for(iter = ezswarm_db().begin(); iter != ezswarm_db().end(); iter++){
		bt_ezswarm_t *	bt_ezswarm	= *iter;
		// update this bt_ezswarm_t listen_pview
		bt_ezswarm->update_listen_pview(new_listen_pview);
	}
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END





