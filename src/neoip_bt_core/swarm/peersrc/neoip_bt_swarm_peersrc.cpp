/*! \file
    \brief Class to handle the bt_swarm_peersrc_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_peersrc.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_itor.hpp"
#include "neoip_bt_peersrc_vapi.hpp"
#include "neoip_bt_peersrc_event.hpp"
#include "neoip_bt_peersrc_peer_arr.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_socket_itor_arg.hpp"
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
bt_swarm_peersrc_t::bt_swarm_peersrc_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	itor_blacklist	= NULL;
}

/** \brief Destructor
 */
bt_swarm_peersrc_t::~bt_swarm_peersrc_t()		throw()
{
	// sanity check - all the bt_peersrc_vapi_t MUST be unregistered before reahing this point
	DBG_ASSERT( peersrc_db.empty() );
	// close all pending bt_swarm_itor_t
	while( !itor_db.empty() )	nipmem_delete itor_db.front();
	// delete itor_blacklist if needed
	nipmem_zdelete	itor_blacklist;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_swarm_peersrc_t::start(bt_swarm_t *bt_swarm
				, const bt_peersrc_peer_arr_t &peersrc_peer_arr)	throw()
{
	// copy the parameter
	this->bt_swarm	= bt_swarm;

	// init the itor_blacklist
	itor_blacklist	= nipmem_new itor_blacklist_t(bt_swarm->profile().itor_blacklist_delay());
	
	// feed the initial bt_peersrc_peer_arr_t
	// - TODO should i do a zerotimer to launch them ?
	//   - YES YES i really dont like the launch them immediatly thing
	// - there is a matter of priority, the bt_peersrc_peer_t which are the more
	//   likely to be alive, should be tried first. 
	//   - e.g. the one from a current request should be tried before the one
	//     from a previous run
	//   - later even prioritize the peerid that you like
	// - some ttl, birthdate, stale etc... to put in bt_peersrc_peer_t to handle them
	//   with more finess
	//   - priority for the one which are a positive account balance
	//   - additionnaly the one which caused a 'connection unreachable' should
	//     be treated differently
	for(size_t i = 0; i < peersrc_peer_arr.size(); i++)
		feed_new_peer( peersrc_peer_arr[i] );
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_peersrc_vapi_t  callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback to receive bt_peersrc_event_t from the various bt_peersrc_vapi_t
 */
bool	bt_swarm_peersrc_t::neoip_bt_peersrc_cb(void *cb_userptr, bt_peersrc_vapi_t &cb_peersrc_vapi
				, const bt_peersrc_event_t &peersrc_event)	throw()
{
	bt_peersrc_vapi_t *	peersrc_vapi	= &cb_peersrc_vapi;
	// log to debug
	KLOG_DBG("enter peersrc_event=" << peersrc_event);
	
	// handle each possible events from its type
	switch( peersrc_event.get_value() ){
	case bt_peersrc_event_t::DOREGISTER:	peersrc_db.push_back(peersrc_vapi);		break;
	case bt_peersrc_event_t::UNREGISTER:	peersrc_db.remove(peersrc_vapi);		break;
	case bt_peersrc_event_t::NEW_PEER:	feed_new_peer(*peersrc_event.get_new_peer());	break;
	default:				DBG_ASSERT( 0 );
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      handle the itor_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the bt_swarm_itor_t matching this remote_addr, or NULL if none does
 */
bt_swarm_itor_t	*bt_swarm_peersrc_t::itor_by_remote_addr(const socket_addr_t &remote_addr) throw()
{
	std::list<bt_swarm_itor_t *>::iterator	iter;
	// go thru the whole itor_db
	for(iter = itor_db.begin(); iter != itor_db.end(); iter++){
		bt_swarm_itor_t *	swarm_itor	= *iter;
		// if this swarm_itor remote_addr matches, return it
		if( remote_addr == swarm_itor->remote_addr() )	return swarm_itor;
	}
	// if this point is reached, no bt_swarm_itor_t matches, so return NULL
	return NULL;
}

/** \brief return true if it is allowed to create a new bt_swarm_itor_t
 */
bool	bt_swarm_peersrc_t::is_new_itor_allowed()	const throw()
{
	// if no new full is allowed, return false
	// - it is a waste of rescource to try establish new connection if newly established connection
	//   if they wont be accepted once established
	if( !bt_swarm->is_new_full_allowed() )				return false;
	// if current number of bt_swarm_itor_t is >= than the no_new_itor_limit, forbid any new one
	if( itor_db.size() >= bt_swarm->profile().no_new_itor_limit() )	return false;
	// else allow it
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  utility function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief a new peer is feed to the bt_swarm_peersrc_t
 * 
 * - this function doesnt 
 */
void	bt_swarm_peersrc_t::feed_new_peer(const bt_peersrc_peer_t &peersrc_peer)	throw()
{
	// log to debug
	KLOG_DBG("enter tracker_peer=" << peersrc_peer);
	// sanity check - dest_addr() MUST be fully_qualified as it will be a unicast dest addr
	// TODO to implement is_fully_qualified in socket_addr_t
	//DBG_ASSERT( peersrc_peer.dest_addr().is_fully_qualified() );

	// if this bt_peersrc_peer_t is contained in the itor_blacklist, do nothing
	if( itor_blacklist->contain(peersrc_peer) )			return;
	// if this dest_addr is already used for a bt_swarm_itor_t, do nothing
	if( itor_by_remote_addr(peersrc_peer.dest_addr()) )		return;
	// if this dest_addr is already used for a bt_swarm_full_t, do nothing
	if( bt_swarm->full_by_remote_addr(peersrc_peer.dest_addr()) )	return;

	// if this bt_peersrc_peer_t is already present in the peersrc_peer_db, do nothing
	// - NOTE: currently peersrc_peer_db is a list, so it is slow 
	std::list<bt_peersrc_peer_t>::iterator	iter;
	iter	= std::find(peersrc_peer_db.begin(), peersrc_peer_db.end(), peersrc_peer);
	if( iter != peersrc_peer_db.end() )		return;
	
	// queue this new peer
	// - TODO unclear i should put it at the end.... 
	//   - idea: the livest peer should be tried first.
	//   - the idea seems good but imply to know how recent is the peersrc_peer
	//     - currently it is not possible
	//     - if from bt_peersrc_http_t, it is unknown. may be approximated by half
	//       the request period
	//     - if from bt_peersrc_nslan_t, it is immediate
	//     - if from bt_peersrc_utpex_t, it is immediate
	//     - if from bt_peersrc_kad_t, it is unknown. would require to handle the 
	//       record age or even last update in kad
	peersrc_peer_db.push_back(peersrc_peer);

	// launch the new bt_swarm_itor_t
	may_launch_new_itor();
}

/** \brief Return the number of seeder from all the peers
 */
size_t	bt_swarm_peersrc_t::nb_seeder()			const throw()
{
	size_t	nb_seeder	= 0;
	// go thru all the registered bt_peersrc_vapi_t
	std::list<bt_peersrc_vapi_t *>::const_iterator	iter;
	for(iter = peersrc_db.begin(); iter != peersrc_db.end(); iter++){
		const bt_peersrc_vapi_t *peersrc_vapi	= *iter;
		// update the local counter with the one of this bt_peersrc_vapi_t
		nb_seeder	+= peersrc_vapi->nb_seeder();
	}
	// return the value
	return nb_seeder;
}

/** \brief Return the number of leecher from all the peers
 */
size_t	bt_swarm_peersrc_t::nb_leecher()			const throw()
{
	size_t	nb_leecher	= 0;
	// go thru all the registered bt_peersrc_vapi_t
	std::list<bt_peersrc_vapi_t *>::const_iterator	iter;
	for(iter = peersrc_db.begin(); iter != peersrc_db.end(); iter++){
		const bt_peersrc_vapi_t *peersrc_vapi	= *iter;
		// update the local counter with the one of this bt_peersrc_vapi_t
		nb_leecher	+= peersrc_vapi->nb_leecher();
	}
	// return the value
	return nb_leecher;
}

/** \brief Trigger the zerotimer_t to potentially launch new bt_swarm_itor_t
 */
void	bt_swarm_peersrc_t::may_launch_new_itor()	throw()
{
	// launch the newitor_zerotimer if not already running
	if( newitor_zerotimer.empty() )	newitor_zerotimer.append(this, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the newitor_zerotimer expire
 * 
 * - only used to avoid any nested notification issue
 */
bool	bt_swarm_peersrc_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// simply try to launch new bt_swarm_itor_t
	try_launch_new_itor();
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     try_launch_new_itor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_tracker_client_t to provide event
 */
void	bt_swarm_peersrc_t::try_launch_new_itor()	throw() 
{
	bt_session_t *	bt_session	= bt_swarm->get_session();
	bt_err_t	bt_err;

	// log to debug
	KLOG_DBG("enter");

	// loop until the peersrc_peer_db is not empty and that new itor are allowed
	while( is_new_itor_allowed() && !peersrc_peer_db.empty() ){
		// dequeue the first bt_peersrc_peer_t
		bt_peersrc_peer_t	peersrc_peer	= peersrc_peer_db.front();
		peersrc_peer_db.pop_front();

		// if this bt_peersrc_peer_t is contained in the itor_blacklist, goto the next
		if( itor_blacklist->contain(peersrc_peer) )	continue;

		// build some alias
		const socket_addr_t &	remote_addr	= peersrc_peer.dest_addr();
		ipport_addr_t		remote_ipport	= remote_addr.get_peerid_vapi()->to_string()
							+ std::string(":")
							+ remote_addr.get_portid_vapi()->to_string();

		// if peersrc_peer is the local peer, skip it. peersrc_peer declared the local peer IF
		// - it is the local peer, if peersrc_peer.dest_addr() is local listen_pview
		// - it is the local peer, if peersrc_peer.dest_addr() is local listen_lview
		// - it is the local peer, if peersrc_peer.peerid() is local_peerid
		// - all those conditions doesnt garantee to identify the local peer because
		//   the peersrc_peer.peerid() may be unknown or the bt_session->listen_pview()
		//   may not be fully qualified
		//  - so an additionnal check is done by the peerid after the handshake exchange
		//    in bt_swarm_itor_t and in bt_session_cnx_t.
		if( remote_ipport == bt_session->listen_pview() )	continue;
		if( remote_ipport == bt_session->listen_lview() )	continue;
		if( peersrc_peer.peerid() == bt_session->local_peerid())continue;

		// if this remote_addr is already used for a bt_swarm_itor_t, skip it
		if( itor_by_remote_addr(remote_addr) )			continue;
		// if this remote_addr is already used for a bt_swarm_full_t, skip it
		if( bt_swarm->full_by_remote_addr(remote_addr) )	continue;
		
		// create and start a bt_swarm_itor_t for this peer
		bt_swarm_itor_t *	swarm_itor;
		swarm_itor	= nipmem_new bt_swarm_itor_t();
		bt_err		= swarm_itor->start(this, peersrc_peer, this, NULL);
		if( bt_err.failed() )	nipmem_zdelete	swarm_itor;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_swarm_itor_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_swarm_itor_t when to notify an event
 * 
 * @return true if the bt_swarm_itor_t is still valid after the callback
 */
bool	bt_swarm_peersrc_t::neoip_bt_swarm_itor_cb(void *cb_userptr, bt_swarm_itor_t &cb_swarm_itor
					, const bt_err_t &cb_bt_err, socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw()
{
	bt_swarm_itor_t * swarm_itor	= &cb_swarm_itor; 
	// log to debug
	KLOG_ERR("enter bt_err=" << cb_bt_err << " peersrc_peer=" << swarm_itor->peersrc_peer());

	// if bt_swarm_itor_t succeed, spawn a bt_swarm_full_t, else put it in itor_blacklist
	if( cb_bt_err.succeed() ){
		bt_swarm_full_t*swarm_full;
		bt_err_t	bt_err;
		// start a bt_swarm_full_t
		swarm_full	= nipmem_new bt_swarm_full_t();
		bt_err		= swarm_full->start(bt_swarm, socket_full, recved_data, remote_handshake);
		if( bt_err.failed() )	KLOG_ERR("Cant start bt_swarm_full_t due to " << bt_err);
	}else{	
#if 1	// TODO to remove - only to debug
		DBG_ASSERT( 0 );
#endif
		// put it in itor_blacklist
		itor_blacklist->update(swarm_itor->peersrc_peer());
	}

	// delete the notifier bt_swarm_itor_t
	nipmem_zdelete	swarm_itor;
	// try to launch new ones
	may_launch_new_itor();
	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END





