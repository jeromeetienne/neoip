/*! \file
    \brief Class to handle the bt_peersrc_kad_t

\par About retry timeout delay
- TODO how come the kad_rec_src_t is not used ?
  - this is coded exactly for this purpose
  - doing the same thing here is duplication aka breaking the unicity rules
- the first kad_query_t is done immediatly 
  - TODO should be tunable via profile
- if the kad_query_t returns a fatal event, 
- if the kad_query_t returns no new peer
- if the kad_query_t returns new peer
- As the kad_query_t consumes rescources on the whole realmid, those query
  should be used with care. especially with the bt_peersrc_utpex_t which 
  allows peers to exchange peers without loading peer which are not in the 
  swarm.
  
\par TODO About the published ipport_addr_t
- how to handle this ipport_addr_t
- which value to put in this field ? 
- the goal is to get the ipport_addr_t at which to connect the remote peer
- issue with the lview/pview
- the kad_rec_t may be originated from a peer on the LAN, but may be stored
  on a very remote peer out of the LAN.
- so the read the remote peer address of the socket wont work here
- NOTE: what about the LAN id ? i though a little bit about it
- what about putting all the lview and put them all in the bt_swarm_peersrc_t ?
- POSSIBLE SOLUTION:
  - publish the ipport_aview_t::pview() in the bt_peersrc_kad_t record
    - so publish IFF the ipport_aview_t.pview() is available
  - add another bt_peersrc_nslan_t publishing a record containing ipport_aview_t::lview()
    - this allow the connectivity even when the ipport_aview_t::pview() is unknown
      or when it is known but the peer is behind a non-lback nat
- the issue is similar in the bt_peersrc_kad_t and in the router_peer_t   
      
*/

/* system include */
/* local include */
#include "neoip_bt_peersrc_kad.hpp"
#include "neoip_bt_peersrc_event.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_kad.hpp"
#include "neoip_kad_publish.hpp"
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
bt_peersrc_kad_t::bt_peersrc_kad_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	callback	= NULL;
	m_nb_seeder	= 0;
	m_nb_leecher	= 0;
	kad_publish	= NULL;
	kad_query	= NULL;
}

/** \brief Destructor
 */
bt_peersrc_kad_t::~bt_peersrc_kad_t()		throw()
{
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( callback ){
		bool	tokeep	= notify_callback( bt_peersrc_event_t::build_unregister() );
		DBG_ASSERT( tokeep == true );
	}
	// delete kad_publish_t if needed
	nipmem_zdelete	kad_publish;
	// delete the kad_query if needed
	nipmem_zdelete	kad_query;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_peersrc_kad_t &bt_peersrc_kad_t::set_profile(const bt_peersrc_kad_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_peersrc_kad_t::start(bt_swarm_t *bt_swarm, kad_peer_t *kad_peer
					, bt_peersrc_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->bt_swarm	= bt_swarm;
	this->kad_peer	= kad_peer;
	this->callback	= callback;
	this->userptr	= userptr;
	// register this peersrc to the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	bool	tokeep	= notify_callback( bt_peersrc_event_t::build_doregister() );
	DBG_ASSERT( tokeep == true );

	// publish the peer record
	bt_err_t	bt_err;
	bt_err		= publish_peer_record();
	if( bt_err.failed() )	return bt_err;
	
	// launch the next_request_timeout
	next_request_timeout.start(delay_t::from_sec(0), this, NULL);
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Function used to update the listen_pview *DURING* bt_peersrc_kad_t run
 */
void	bt_peersrc_kad_t::update_listen_pview(const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter NOT YET IMPLEMENTED new_listen_pview=" << new_listen_pview);

#if 1
	// simply republish the peer record
	bt_err_t	bt_err;
	bt_err	= publish_peer_record();
	if( bt_err.failed() )	KLOG_ERR("Cant publish peer record due to " << bt_err);
#endif
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      handle the ntudp_peer_rec_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief publish the peer record 
 */
bt_err_t	bt_peersrc_kad_t::publish_peer_record()				throw()
{
	bt_session_t *		bt_session	= bt_swarm->get_session();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	kad_err_t		kad_err;
	// if the peer record was already published, delete the kad_publish
	nipmem_zdelete	kad_publish;

	// if bt_session listen_ipport_pview is NOT fully qualified, dont publish the record
	if( !bt_session->listen_pview().is_fully_qualified() )	return bt_err_t::OK;

	// build the dest_addr 
	socket_addr_t	dest_addr;
	dest_addr	= std::string("stcp://") + bt_session->listen_pview().to_string();

	// build the kad_rec_t out of it
	bytearray_t	payload;
	payload << bt_session->local_peerid();
	payload << bt_swarm->is_seed();
	payload << dest_addr;
	// TODO change that to get a bt_peersrc_peer_t directly
	// - bt_swarm->to_peersrc_peer_pview()

	
	// build the peer record
	// - the recid is directly derived from the bt_session peerid + bt_mfile_t infohash
	//   - thus it is predictable and may be regenerated to delete the record later if needed
	kad_keyid_t	keyid		= bt_mfile.infohash().to_canonical_string();
	kad_recid_t	recid		= bt_session->local_peerid().to_canonical_string() + bt_mfile.infohash().to_canonical_string();
	delay_t		record_ttl	= profile.record_ttl();
	kad_rec_t	kad_rec		= kad_rec_t(recid, keyid, record_ttl, payload.to_datum());
	// log to debug
	KLOG_DBG("keyid=" << keyid << " recid=" << recid);

	// publish the just-built kad_rec_t
	kad_publish	= nipmem_new kad_publish_t();
	kad_err		= kad_publish->start(kad_peer, kad_rec, this, NULL);
	if( kad_err.failed() )	return bt_err_from_kad(kad_err);

	// return no error
	return bt_err_t::OK;
}

/** \brief callback notified when a kad_publish_t has an event to notify
 */
bool	bt_peersrc_kad_t::neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
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


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool bt_peersrc_kad_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	kad_keyid_t		record_keyid	= bt_swarm->get_mfile().infohash().to_canonical_string();
	delay_t			query_timeout	= profile.query_timeout();
	kad_err_t		kad_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - kad_query_t MUST NOT be in progress
	DBG_ASSERT( !kad_query );
	// create and start the kad_query_t
	kad_query	= nipmem_new kad_query_t();
	kad_err		= kad_query->start(kad_peer, record_keyid, 0, query_timeout, this, NULL);
	if( kad_err.failed() ){
		// log the event
		KLOG_INFO("launching kad_query_t failed due to " << kad_err);		
		// relaunch the next_request_timeout
		next_request_timeout.start(profile.retry_delay_local_err(), this, NULL);
		return true;
	}
	// stop the timer during the bt_tracker_client_t - and relaunch it once completed
	next_request_timeout.stop();
	// return tokeep
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_query_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a command in a kad_query_t has an event to notify
 */
bool	bt_peersrc_kad_t::neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw()
{	
	// log to debug
	KLOG_DBG("enter event=" << kad_event);
	// sanity check - the kad_event MUST be query_ok()
	DBG_ASSERT( kad_event.is_query_ok() );
	// sanity check - the next_request_timeout MUST NOT be running
	DBG_ASSERT( next_request_timeout.is_running() == false );
	
	// if the kad_query_t failed, retry later
	if( kad_event.is_fatal() ){
		// log the event
		KLOG_INFO("kad_query_t tracker failed due to " << kad_event);
		// relaunch the next_request_timeout
		next_request_timeout.start(profile.retry_delay_cnx_err(), this, NULL);
		// delete kad_query_t
		nipmem_zdelete kad_query;
		return false;
	}

	// sanity check - the kad_event_t MUST be a kad_event_t::RECDUPS
	DBG_ASSERT( kad_event.is_recdups() );

	// copy the kad_recdups_t before deleting the command
	kad_recdups_t	kad_recdups	= kad_event.get_recdups(NULL);

	// delete the kad_query_t and mark is unused
	nipmem_zdelete	kad_query;

	// if some records have been found, handle the received kad_recdups_t	
	handle_recved_kad_recdups(kad_recdups);

	// return 'dontkeep' as the command has just been deleted
	return false;
}

/** \brief Handle the received kad_recdups
 */
void	bt_peersrc_kad_t::handle_recved_kad_recdups(const kad_recdups_t &kad_recdups)	throw()
{
	// log to debug
	KLOG_DBG("enter kad_recdups=" << kad_recdups);
	// zero the seeder/leecher counter
	m_nb_seeder	= 0;
	m_nb_leecher	= 0;
	// go thru all the received kad_rec_t
	for(size_t i = 0; i < kad_recdups.size(); i++){
		const kad_rec_t &	kad_rec	= kad_recdups[i];
		bytearray_t		payload( kad_rec.get_payload() );
		bt_id_t			remote_peerid;
		socket_addr_t		remote_addr;
		bool			is_seeder;
		// parse the incoming packet
		try {
			// TODO change that to get a bt_peersrc_peer_t directly
			payload	>> remote_peerid;
			payload >> is_seeder;
			payload	>> remote_addr;
		}catch(serial_except_t &e){
			// log the event
			KLOG_ERR("Can't parse peer record " << kad_rec << " due to " << e.what() );
			// goto the next record even if it is a serious error which MUST NOT happen
			continue;
		}
		// log to debug
		KLOG_DBG("remote_peerid=" << remote_peerid << " remote_addr=" << remote_addr << " is_seeder=" << is_seeder);
		// if remote_addr is null, the publish of this record is not reachable, discard it
		// - TODO should this be removed / adapted ?
		// - now remote_addr is supposed to be ALWAYS fully qualified as it is published
		//   only in this case
		if( remote_addr.is_null() )	continue;
		// update the seeder/leecher counter
		if( is_seeder )		m_nb_seeder++;
		else			m_nb_leecher++;
		// build the bt_tracker_peer_t from the record payload
		bt_peersrc_peer_t	peersrc_peer(remote_peerid, remote_addr);
		// notify the caller
		bool	tokeep	= notify_callback(bt_peersrc_event_t::build_new_peer(&peersrc_peer));
		if( !tokeep )	return;		
	}
	// relaunch the next_request_timeout
	next_request_timeout.start(profile.retry_delay_succeed(), this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_peersrc_kad_t::notify_callback(const bt_peersrc_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_peersrc_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





