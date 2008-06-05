/*! \file
    \brief Class to handle the bt_swarm_full_sched_t

\par Brief Description
bt_swarm_full_sched_t handle the scheduler information for a 
specific bt_swarm_full_t.

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_sched_request.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_event.hpp"
#include "neoip_bt_swarm_profile.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_cmd.hpp"
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
bt_swarm_full_sched_t::bt_swarm_full_sched_t(bt_swarm_full_t *swarm_full)		throw()
{
	// copy the parameter
	this->swarm_full	= swarm_full;
	// init some parameters
	m_reqauth_type		= bt_reqauth_type_t::DENY;
}

/** \brief Destructor
 */
bt_swarm_full_sched_t::~bt_swarm_full_sched_t()		throw()
{
	// delete all the pending bt_swarm_sched_request_t
	while( !request_db.empty() )	nipmem_delete *request_db.begin();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return maximum amount of data to request at a given time
 * 
 * - currently it is based on a pseudo bandwidth-delay product
 *   - http://en.wikipedia.org/wiki/Bandwidth_delay_product
 * - NOTE: it may be good to get various policy for this queue, tunnable from
 *   outside the bt_swarm_t
 * - bt_http_ecnx_cnx_t has the same function
 */
size_t	bt_swarm_full_sched_t::req_queue_maxlen()	const throw()
{
	bt_swarm_t *			bt_swarm	= swarm_full->bt_swarm;
	const bt_swarm_sched_profile_t &sched_profile	= bt_swarm->swarm_sched()->profile();
	double				recv_rate	= swarm_full->recv_rate_avg();
	// compute a pseudo bandwidth-delay product - it is pseudo because:
	// - the latency is hardcoded in the profile as it is hard/impossible
	//   to measure in this context
	// - a request, BLOCK_REQ, may not be sent immediatly e.g. if a BLOCK_REP is in progress
	size_t	req_queue_len	= size_t(sched_profile.req_queue_delay().to_sec_double() * recv_rate);
	// clamp the req_queue_len based on the profile minlen/maxlen
	req_queue_len	= std::max(req_queue_len, sched_profile.req_queue_minlen());
	req_queue_len	= std::min(req_queue_len, sched_profile.req_queue_maxlen());
	// return the computed result
	return req_queue_len;
}
	
/** \brief Return true if there is a pending request matching those parameters
 */
bool	bt_swarm_full_sched_t::has_matching_request(const file_range_t &totfile_range)	const throw()
{
	request_db_t::const_iterator	iter;
	// go thru the whole request_db
	for(iter = request_db.begin(); iter != request_db.end(); iter++){
		const bt_swarm_sched_request_t *	sched_request = *iter;
		if( sched_request->totfile_range() == totfile_range )	break;
	}
	// if no pending bt_swarm_sched_request_t matches, return false
	if( iter == request_db.end() )	return false;
	// if there is a match, return true
	return true;
}

/** \brief Return the first bt_swarm_sched_request_t for a given pieceidx - NULL if no matches
 */
bt_swarm_sched_request_t *bt_swarm_full_sched_t::request_by_pieceidx(size_t pieceidx)	const throw()
{
	request_db_t::const_iterator	iter;
	// go thru the whole request_db
	for(iter = request_db.begin(); iter != request_db.end(); iter++){
		bt_swarm_sched_request_t *	sched_request = *iter;
		if( sched_request->piece_idx() == pieceidx )	return sched_request;
	}
	// if this point is reached, there is no match
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the request authorization for the bt_swarm_full_t
 * 
 * - WARNING: do not send the bt_cmd_t thru the bt_swarm_full_t
 *   - use the sync_doauth_req_cmd() method for that
 *   - required because the reqauth is first denied to all then allocated to the proper
 *     if given bt_swarm_full_t is reelected, it will be denied and then authorized, sending
 *     the command simultaneously will cause useless traffic and *especially* cancel all the 
 *     pending request without any need.
 */
void	bt_swarm_full_sched_t::reqauth_type(const bt_reqauth_type_t &m_reqauth_type
						, const delay_t &expire_delay)	throw()
{
	// copy the parameter
	this->m_reqauth_type	= m_reqauth_type;
	// handle the reqauth_timeout according to the reqauth_type
	reqauth_timeout.start(expire_delay, this, NULL);
}

/** \brief Synchronise the bt_swarm_full_t state about local_doauth_req with the reqauth_type
 * 
 * - see ::reqauth_type() for the motivation of this function
 */
void	bt_swarm_full_sched_t::sync_doauth_req_cmd()				throw()
{
	// handle the reqauth_timeout according to the reqauth_type
	switch(reqauth_type().get_value()){
	case bt_reqauth_type_t::DENY:	swarm_full->set_doauth_req(false);	break;
	default:			swarm_full->set_doauth_req(true);	break;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  bt_swarm_sched_cnx_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink and init a new bt_swarm_sched_request_t to this connection
 */
void	bt_swarm_full_sched_t::sched_req_dolink_start(bt_swarm_sched_request_t *request) 	throw()
{
	bt_swarm_t *	bt_swarm	= swarm_full->bt_swarm;
	const bt_mfile_t &bt_mfile	= bt_swarm->get_mfile();
	// add it to the request_db
	request_db.insert(request);
	// start the request - send bt_cmd_t::BLOCK_REQ
	bt_prange_t	bt_prange	= bt_prange_t::from_totfile_range(request->totfile_range(), bt_mfile);
	bt_cmd_t 	bt_cmd		= bt_cmd_t::build_block_req(bt_prange);
	swarm_full->send_cmd(bt_cmd);
}

/** \brief UnLink a new bt_swarm_sched_request_t from this connection
 */
void	bt_swarm_full_sched_t::sched_req_unlink(bt_swarm_sched_request_t *request)	throw()
{
	// remove it from the request_db
	request_db.erase(request);	
}

/** \brief Cancel a request made on this connection
 * 
 * - send a bt_cmdtype_t::BLOCK_DEL
 */
void	bt_swarm_full_sched_t::sched_req_cancel(bt_swarm_sched_request_t *sched_request)	throw()
{
	bt_swarm_t *	bt_swarm	= swarm_full->bt_swarm;
	const bt_mfile_t &bt_mfile	= bt_swarm->get_mfile();
	// build the bt_cmd_t to send
	bt_prange_t	bt_prange	= bt_prange_t::from_totfile_range(sched_request->totfile_range(), bt_mfile);
	bt_cmd_t    	bt_cmd		= bt_cmd_t::build_block_del(bt_prange);
	// send the bt_cmd_t thru the swarm_full
	swarm_full->send_cmd(bt_cmd);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool bt_swarm_full_sched_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// stop the timer
	reqauth_timeout.stop();

	// notify the callback of this event
	bool	tokeep	= swarm_full->notify_callback( bt_swarm_full_event_t::build_reqauth_timedout() );
	if( !tokeep )	return false;	

	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END





