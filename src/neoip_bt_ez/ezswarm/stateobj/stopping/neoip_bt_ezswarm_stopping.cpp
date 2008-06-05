/*! \file
    \brief Class to handle the bt_ezswarm_stopping_t
    
\par Brief Description
\ref bt_ezswarm_stopping_t handle the bt_ezswarm_state_t::STOPPING for bt_ezswarm_t

- it always notify completion as success and never as faillure as it is kindof
  like a constructor.

This is kinda kludgy. currently it is only used to unregister from the bt_peersrc_http_t
and bt_peersrc_kad_t
- if there is a bt_peersrc_http_t, it is sending a "stopped" event
- if there is a bt_peersrc_kad_t, delete the record
- if there are none, it is just doing a zerotimer_t to notify the completion
- when both are completed, the completion is notified



*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm_stopping.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bt_tracker_client.hpp"
#include "neoip_bt_tracker_request.hpp"
#include "neoip_bt_tracker_reply.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_bt_io_stopping.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_kad.hpp"
#include "neoip_kad_delete.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_ezswarm_stopping_t::bt_ezswarm_stopping_t()		throw()
{
	// zero some field
	bt_ezswarm		= NULL;
	bt_tracker_client	= NULL;
	kad_delete		= NULL;
	io_stopping		= NULL;
}

/** \brief Destructor
 */
bt_ezswarm_stopping_t::~bt_ezswarm_stopping_t()		throw()
{
	// delete the bt_tracker_client_t if needed
	nipmem_zdelete	bt_tracker_client;
	// delete the kad_delete_t if needed
	nipmem_zdelete	kad_delete;
	// delete the bt_io_stopping_t if needed
	nipmem_zdelete	io_stopping;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_ezswarm_stopping_t::start(bt_ezswarm_t *bt_ezswarm)		throw()
{
	bt_err_t	bt_err;
	// copy the parameter
	this->bt_ezswarm	= bt_ezswarm;
		
	// start the bt_tracker_client_t if needed
	if( bt_ezswarm->opt().is_peersrc_http() ){
		bt_tracker_client	= nipmem_new bt_tracker_client_t();
		bt_err			= bt_tracker_client->start(build_tracker_request(), this, NULL);
		if( bt_err.failed() )	return bt_err;
	}

	// start the kad_deleted if needed
	if( bt_ezswarm->opt().is_peersrc_kad() ){
		bt_session_t *		bt_session	= bt_ezswarm->ezsession()->bt_session();
		const bt_mfile_t &	bt_mfile	= bt_ezswarm->mfile();
		const bt_ezswarm_profile_t &profile	= bt_ezswarm->profile();
		// generate the keyid and recid for the record 
		// - NOTE: it MUST be the same as the ones in bt_peersrc_kad_t
		kad_keyid_t	keyid	= bt_mfile.infohash().to_canonical_string();
		kad_recid_t	recid	= bt_session->local_peerid().to_canonical_string() + bt_mfile.infohash().to_canonical_string();
		kad_err_t	kad_err;
		// log to debug
		KLOG_DBG("keyid=" << keyid << " recid=" << recid);
		// start a kad_delete_t
		kad_delete	= nipmem_new kad_delete_t();
		kad_err		= kad_delete->start(profile.peersrc_kad_peer(), recid, keyid
					, profile.peersrc_kad_stopping_timeout(), this, NULL);
		if( kad_err.failed() )	return bt_err_from_kad(kad_err);
	}
	
	// if the bt_ezswarm_t bt_io_vapi_t need stopping, start one
	if( bt_ezswarm->io_vapi()->need_stopping() )
		io_stopping	= bt_ezswarm->io_vapi()->stopping_ctor(this, NULL); 		

	// if the stopping is already completed, notify the completion thru a zerotimer_t
	if( is_completed() ){
		zerotimer.append(this, NULL);
		return bt_err_t::OK;
	}
		
	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - this zerotimer_t is used IIF there are no tracker to 'unregister'
 */
bool	bt_ezswarm_stopping_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// sanity check - bt_ezswarm_stopping_t MUST be completed
	DBG_ASSERT( is_completed() );
	// notify the bt_ezswarm_t that this stateobj has been sucessfully completed
	return bt_ezswarm->stateobj_notify_success();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true is the bt_io_ezswarm_stopping_t is completed
 */
bool	bt_ezswarm_stopping_t::is_completed()		const throw()
{
	// test if all subpart are completed
	if( bt_tracker_client )	return false;
	if( kad_delete )	return false;
	if( io_stopping )	return false;
	// if all previous tests passed, return true
	return true;
}

/** \brief Build a bt_tracker_request_t 
 */
bt_tracker_request_t	bt_ezswarm_stopping_t::build_tracker_request()	const throw()
{
	const bt_mfile_t &	bt_mfile		= bt_ezswarm->mfile();
	const bt_session_t *	bt_session		= bt_ezswarm->ezsession()->bt_session();
	const ipport_addr_t &	listen_ipport_pview	= bt_session->listen_pview();
	bt_tracker_request_t	tracker_request;
	// build the tracker
	tracker_request.announce_uri	( bt_mfile.announce_uri() );
	tracker_request.infohash	( bt_mfile.infohash() );
	tracker_request.peerid		( bt_session->local_peerid() );
	tracker_request.port		( listen_ipport_pview.port() );
	// set the request ip address when it is set in the bt_session
	if( !listen_ipport_pview.ipaddr().is_any() )
		tracker_request.ipaddr	( listen_ipport_pview.ipaddr()	);

	// NOTE: it is impossible to forward the uploaded/downloaded/left field
	// - there are stored in the bt_swarm_t and the object no more exists in this state

	// set the stopped event to ask the tracker to remove this peer from the list
	tracker_request.event		( "stopped" );

	// log to debug
	KLOG_DBG("tracker_request=" << tracker_request);
	// return the just built request
	return tracker_request;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_tracker_client_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_tracker_client_t to provide event
 */
bool	bt_ezswarm_stopping_t::neoip_bt_tracker_client_cb(void *cb_userptr
						, bt_tracker_client_t &cb_bt_tracker_client
						, const bt_err_t &client_err
						, const bt_tracker_reply_t &tracker_reply)	throw() 
{
	// sanity check - the bt_ezswarm_opt_t MUST have PEERSRC_HTTP
	DBG_ASSERT( bt_ezswarm->opt().is_peersrc_http() );
	// log to debug
	KLOG_DBG("enter bt_err=" << client_err << " tracker_reply=" << tracker_reply);
			
	// delete the bt_tracker_client_t
	nipmem_zdelete	bt_tracker_client;

	// after a completed bt_ezswarm_stopping_t, notify the bt_ezswarm_t
	if( is_completed() )	return bt_ezswarm->stateobj_notify_success();
	
	// return dontkeep as the bt_tracker_client_t has just been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			kad_delete_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_delete_t has an event to notify
 */
bool	bt_ezswarm_stopping_t::neoip_kad_delete_cb(void *cb_userptr, kad_delete_t &cb_kad_delete
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_DBG("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be delete_ok()
	DBG_ASSERT( kad_event.is_delete_ok() );
	
	// if an error occurs, log the event
	if( !kad_event.is_completed() )
		KLOG_INFO("unable to delete the bt_peersrc_kad_t record.");

	// delete the kad_delete_t
	nipmem_zdelete	kad_delete;

	// after a completed bt_ezswarm_stopping_t, notify the bt_ezswarm_t
	if( is_completed() )	return bt_ezswarm->stateobj_notify_success();

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_io_stopping_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a bt_io_stopping_t is completed
 */
bool	bt_ezswarm_stopping_t::neoip_bt_io_stopping_cb(void *cb_userptr, bt_io_stopping_t &cb_io_stopping
						, const bt_err_t &bt_err)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err);
	
	// if an error occurs, log the event
	if( bt_err.failed() )
		KLOG_INFO("bt_io_stopping_t reported error due to " << bt_err);

	// delete the bt_io_stopping_t
	nipmem_zdelete	io_stopping;

	// after a completed bt_ezswarm_stopping_t, notify the bt_ezswarm_t
	if( is_completed() )	return bt_ezswarm->stateobj_notify_success();

	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END





