/*! \file
    \brief Class to handle the bt_peersrc_http_t

\par About the events
- the first event sent is always "started".
- if the bt_swarm_t just becomed seed after the last request, a "completed" is sent
- NOTE: it never send the "stopped" event because it would cause a "delayed
  deletion" e.g. a kinda tcp linger when a context needs to be kept during the 
  deletion.
  - instead this is handled by bt_ezswarm_stopping_t, the bt_ezswarm_t has the 
    state to handle this "linger"

*/

/* system include */
/* local include */
#include "neoip_bt_peersrc_http.hpp"
#include "neoip_bt_peersrc_event.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bt_tracker_client.hpp"
#include "neoip_bt_tracker_request.hpp"
#include "neoip_bt_tracker_reply.hpp"
#include "neoip_rand.hpp"
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
bt_peersrc_http_t::bt_peersrc_http_t()		throw()
{
	// zero some fields
	bt_swarm		= NULL;
	callback		= NULL;
	bt_tracker_client	= NULL;
	m_nb_seeder		= 0;
	m_nb_leecher		= 0;
	nb_request_sent		= 0;
}

/** \brief Destructor
 */
bt_peersrc_http_t::~bt_peersrc_http_t()		throw()
{
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( callback ){
		bool	tokeep	= notify_callback( bt_peersrc_event_t::build_unregister() );
		DBG_ASSERT( tokeep == true );
	}
	// delete the bt_tracker_client_t if needed
	nipmem_zdelete	bt_tracker_client;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_peersrc_http_t &bt_peersrc_http_t::set_profile(const bt_peersrc_http_profile_t &profile)	throw()
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
bt_err_t	bt_peersrc_http_t::start(bt_swarm_t *bt_swarm, bt_peersrc_cb_t *callback
						, void *userptr)	throw()
{
	// sanity check - the bt_mfile_t MUST contains a announce_uri
	DBG_ASSERT( !bt_swarm->get_mfile().announce_uri().is_null() );
	// copy the parameter
	this->bt_swarm	= bt_swarm;
	this->callback	= callback;
	this->userptr	= userptr;
	// register this peersrc to the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	bool	tokeep	= notify_callback( bt_peersrc_event_t::build_doregister() );
	DBG_ASSERT( tokeep == true );

	// get a tracker_key at random
	m_tracker_key	= OSTREAMSTR(uint32_t(neoip_rand(1000)));

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
void	bt_peersrc_http_t::update_listen_pview(const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter NOT YET IMPLEMENTED new_listen_pview=" << new_listen_pview);
	// TODO to code
	// - in this case, only the port matter ?
	// - to trigger a request now, to update the tracker with the new listen_pview

	// delete the bt_tracker_client_t if needed
	nipmem_zdelete	bt_tracker_client;
	// launch a new one immedialty to update the tracker with the new listen_pview
	next_request_timeout.start(delay_t::from_sec(0), this, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool bt_peersrc_http_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	bt_err_t	bt_err;
	std::string	event_str;

	// if it if the first request sent for this bt_swarm_t, set the event to "started"
	if( nb_request_sent == 0 ){
		event_str	= "started";
	}else if( bt_swarm->is_seed() != prev_seed_state ){
		// if the bt_swarm_t just becomed seed, set the event to "completed"
		event_str	= "completed";
	}	
	// update the prev_seed_state
	prev_seed_state	= bt_swarm->is_seed();
	// update the nb_request_sent
	nb_request_sent++;
	
	// start the bt_tracker_client_t
	bt_tracker_client	= nipmem_new bt_tracker_client_t();
	bt_err			= bt_tracker_client->start(build_tracker_request(event_str), this, NULL);
	if( bt_err.failed() ){
		// log the event
		KLOG_INFO("Connection to tracker failed due to " << bt_err);		
		// relaunch the next_request_timeout
		next_request_timeout.start(profile.retry_delay_local_err(), this, NULL);
		return true;
	}
	// stop the timer during the bt_tracker_client_t - and relaunch it once completed
	next_request_timeout.stop();
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_tracker_request_t 
 */
bt_tracker_request_t	bt_peersrc_http_t::build_tracker_request(const std::string &event_str)	const throw()
{
	const bt_mfile_t &	bt_mfile		= bt_swarm->get_mfile();
	const bt_session_t *	bt_session		= bt_swarm->get_session();
	const ipport_addr_t &	listen_ipport_pview	= bt_session->listen_pview();
	const bt_swarm_stats_t &swarm_stats		= bt_swarm->swarm_stats();
	file_size_t		totfile_avail		= bt_unit_t::totfile_avail(bt_swarm->local_pavail(), bt_mfile);
	file_size_t		totfile_size		= bt_mfile.totfile_size();
	bt_tracker_request_t	tracker_request;
	// build the tracker
	tracker_request.announce_uri	( bt_mfile.announce_uri()	);
	tracker_request.infohash	( bt_mfile.infohash()		);
	tracker_request.peerid		( bt_session->local_peerid()	);
	tracker_request.port		( listen_ipport_pview.port()	);
	// set the request ip address when it is set in the bt_session
	if( !listen_ipport_pview.ipaddr().is_any() )
		tracker_request.ipaddr	( listen_ipport_pview.ipaddr()	);
	tracker_request.left		( totfile_size - totfile_avail 	);
	tracker_request.downloaded	( swarm_stats.dloaded_datalen()	);
	tracker_request.uploaded	( swarm_stats.uloaded_datalen()	);
	tracker_request.event		( event_str			);
	tracker_request.key		( m_tracker_key			);

	// TODO handle the bt_swarm_t bt_cnx_mode_t here
	
	
	// set the "compact" field depending on the profile
	// - NOTE: some tracker requires to use compact request
	// - e.g. demonoid.com replies a faillure reason "client too old" if "compact" is not requested
	if( profile.use_compact_request() )
		tracker_request.compact	( true				);

	// log to debug
	KLOG_ERR("tracker_request=" << tracker_request);
	// return the just built request
	return tracker_request;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_tracker_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_tracker_client_t to provide event
 */
bool	bt_peersrc_http_t::neoip_bt_tracker_client_cb(void *cb_userptr, bt_tracker_client_t &cb_bt_tracker_client
						, const bt_err_t &client_err
						, const bt_tracker_reply_t &tracker_reply)	throw() 
{
	// log to debug
	KLOG_ERR("enter bt_err=" << client_err << " reply=" << tracker_reply);

	// delete the bt_tracker_client_t
	nipmem_zdelete bt_tracker_client;
	
	// in case of http/tcp error, retry later
	if( client_err.failed() ){
		// set the faillure reason
		m_failure_reason= "from connection: " + client_err.to_string();
		// log the event
		KLOG_INFO("Connection to tracker failed due to " << client_err);
		// relaunch the next_request_timeout
		next_request_timeout.start(profile.retry_delay_cnx_err(), this, NULL);
		return false;
	}

	// in case of http/tcp error, retry later
	if( !tracker_reply.failure_reason().empty() ){
		// copy the faillure_reason locally
		m_failure_reason= "from tracker: " + tracker_reply.failure_reason();
		// log the event
		KLOG_INFO("Connection to tracker failed due to faillure reason " << tracker_reply.failure_reason());
		// relaunch the next_request_timeout
		next_request_timeout.start(profile.retry_delay_bt_err(), this, NULL);
		return false;
	}
	
	// zero the faillure reason as the request have been successfull
	m_failure_reason= std::string();
	
	
	// copy the parameters received in the bt_tracker_reply_t
	m_nb_seeder	= tracker_reply.nb_seeder();
	m_nb_leecher	= tracker_reply.nb_leecher();
	request_period	= tracker_reply.request_period();

	// feed the bt_peersrc_t with the replied peers
	const bt_tracker_peer_arr_t &	peer_arr	= tracker_reply.peer_arr();
	for(size_t i = 0; i < peer_arr.size(); i++){
		bt_peersrc_peer_t peersrc_peer	= peer_arr[i];
		// notify the caller
		bool	tokeep	= notify_callback(bt_peersrc_event_t::build_new_peer(&peersrc_peer));
		if( !tokeep )	return false;
	}
	
	// relaunch the next_request_timeout based on the tracker_reply.period()
	next_request_timeout.start(tracker_reply.request_period(), this, NULL);
			
	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_peersrc_http_t::notify_callback(const bt_peersrc_event_t &event)	throw()
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





