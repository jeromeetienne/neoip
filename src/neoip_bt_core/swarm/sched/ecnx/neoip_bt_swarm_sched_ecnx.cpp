/*! \file
    \brief Class to handle the bt_swarm_sched_ecnx_t

\par Brief Description
Handle the scheduling for the bt_ecnx_vapi_t connections. 
- bt_ecnx_vapi_t are much more efficient if the requests are sequentials as it 
  reduces the overhead. 
  - it allows to create much larger extenal request
  - it reduces network bandwidth due to bt_ecnx_vapi_t header
  - it increase the speed by reducing the number of external connection to establish

\par Possible improvement
- it could be nice to have an autotuning of the request timeout
  - especially when bt_swarm_t data for delivered to an http connection
  - the closer it is from the http cursor position, the most important it is
  - we dont want the stream to stop!
  - how to get code this ?
  - seems like a job for priority

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched_ecnx.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_sched_piece.hpp"
#include "neoip_bt_swarm_sched_block.hpp"
#include "neoip_bt_swarm_sched_request.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_ecnx_vapi.hpp"
#include "neoip_bt_ecnx_event.hpp"
#include "neoip_bt_pieceavail.hpp"
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
bt_swarm_sched_ecnx_t::bt_swarm_sched_ecnx_t(bt_swarm_sched_t *swarm_sched)	throw()
{
	// copy the parameter
	this->swarm_sched	= swarm_sched;
}

/** \brief Destructor
 */
bt_swarm_sched_ecnx_t::~bt_swarm_sched_ecnx_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief try to launch request on all the bt_ecnx_vapi_t which dont have any request
 */
void	bt_swarm_sched_ecnx_t::try_send_request_on_idle_cnx()	throw()
{
	bt_swarm_t *				bt_swarm	= swarm_sched->bt_swarm();
	bt_swarm_t::ecnx_vapi_db_t &		ecnx_vapi_db	= bt_swarm->get_ecnx_vapi_db();
	bt_swarm_t::ecnx_vapi_db_t::iterator	iter;
	// go thru the whole ecnx_vapi_db
	for(iter = ecnx_vapi_db.begin(); iter != ecnx_vapi_db.end(); iter++){
		bt_ecnx_vapi_t * ecnx_vapi	= iter->second;
		// if this bt_ecnx_vapi_t has pending request, aka is not idle, skip it
		if( ecnx_vapi->nb_pending_request() )	continue;
		// else try to send some request on it
		try_send_request(*ecnx_vapi);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 try_send_request
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Try to send a request to this bt_ecnx_vapi_t
 */
void	bt_swarm_sched_ecnx_t::try_send_request(bt_ecnx_vapi_t &ecnx_vapi)	throw()
{
	bt_swarm_t *		bt_swarm	= swarm_sched->bt_swarm();
	const bt_swarm_profile_t &swarm_profile	= bt_swarm->profile();	
	const bt_swarm_sched_profile_t &profile	= swarm_sched->profile();
	bt_swarm_sched_cnx_vapi_t *sched_cnx	= ecnx_vapi.sched_cnx_vapi();
	const bt_pieceavail_t &	remote_pieceavail= ecnx_vapi.remote_pavail();
	size_t			request_nb_max	= ecnx_vapi.req_queue_maxlen() / swarm_profile.xmit_req_maxlen();
	size_t			pieceidx_hint	= swarm_sched->next_pidx_hint();
	// log to debug
	KLOG_WARN("enter req_queue_maxlen=" << ecnx_vapi.req_queue_maxlen());

	// loop until the maximum number of sched_request are pending
	while( ecnx_vapi.nb_pending_request() < request_nb_max ){
		bt_swarm_sched_piece_t *	sched_piece;
		// try to get a bt_swarm_sched_piece_t to request
		sched_piece	= swarm_sched->select_piece_to_request(sched_cnx
							, remote_pieceavail, pieceidx_hint);
		// if no sched_piece can be request, return now
		if( !sched_piece )	return;
		// log to debug
		KLOG_WARN("enter");
		// sanity check - the sched_piece MUST be remotely available
		DBG_ASSERT( remote_pieceavail.is_avail(sched_piece->pieceidx()) );
		// launch as many requests as possible on this sched_piece
		while( ecnx_vapi.nb_pending_request() < request_nb_max ){
			bt_swarm_sched_block_t *	sched_block;
			// get a bt_swarm_sched_block_t to request
			sched_block	= sched_piece->get_block_to_request(sched_cnx);
			// if this sched_piece has no sched_block to request, leave the loop
			// - a new piece will be selected
			if( !sched_block )	break;
			// launch a request on the first block which isnt locally avail and which needs request
			// - FIXME to make the request timeout tunable would be better	
			nipmem_new bt_swarm_sched_request_t(ecnx_vapi.sched_cnx_vapi(), sched_block
							, profile.request_timeout_normal());
		}

		// update the local copy pieceidx_hint as the next piece to request
		// - this reduce the overhead of the bt_ecnx_vapi_t by maximizing
		//   the number of request made in sequence.
		pieceidx_hint	= sched_piece->pieceidx();
		if( !sched_piece->may_request_block_thru(sched_cnx) ){
			const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
			pieceidx_hint	= (pieceidx_hint + 1) % bt_mfile.nb_piece();
		}
		// set the bt_swarm_sched_t next_pidx_hint with the local pieceidx_hint
		swarm_sched->next_pidx_hint(pieceidx_hint);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          bt_ecnx_vapi_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_ecnx_vapi_t when to notify an event
 * 
 * @return true if the bt_ecnx_t is still valid after the callback
 */
bool	bt_swarm_sched_ecnx_t::neoip_bt_ecnx_vapi_cb(void *cb_userptr, bt_ecnx_vapi_t &cb_ecnx_vapi
					, const bt_ecnx_event_t &ecnx_event)	throw()
{
	bt_ecnx_vapi_t & ecnx_vapi	= cb_ecnx_vapi;
	// log to debug
	KLOG_DBG("enter ecnx_event=" << ecnx_event );

	// parse the event according to its value
	switch( ecnx_event.get_value() ){
	case bt_ecnx_event_t::CNX_OPENED:	return parse_cnx_opened(ecnx_vapi, ecnx_event);
	case bt_ecnx_event_t::CNX_CLOSED:	return parse_cnx_closed(ecnx_vapi, ecnx_event);
	case bt_ecnx_event_t::BLOCK_REP:	return parse_block_rep(ecnx_vapi, ecnx_event);
	default:				DBG_ASSERT( 0 );
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bt_ecnx_event_t parsing
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief parse a bt_ecnx_event_t::CNX_OPENED
 */
bool	bt_swarm_sched_ecnx_t::parse_cnx_opened(bt_ecnx_vapi_t &ecnx_vapi, const bt_ecnx_event_t &ecnx_event)	throw()
{
	bt_swarm_t *	bt_swarm	= swarm_sched->bt_swarm();
	// log to debug
	KLOG_DBG("enter ecnx_event=" << ecnx_event);
	// add this ecnx_vapi to the bt_swarm->ecnx_vapi_db
	bt_swarm->ecnx_vapi_dolink(&ecnx_vapi);
	// update prarity in bt_pselect_vapi_t with the ecnx_vapi bt_piece_avail_t
	bt_swarm->pselect_vapi()->inc_remote_pfreq( ecnx_vapi.remote_pavail() );	
	// try to send a request to this bt_ecnx_vapi_t
	try_send_request(ecnx_vapi);
	// return tokeep
	return true;
}

/** \brief parse a bt_ecnx_event_t::CNX_CLOSED
 */
bool	bt_swarm_sched_ecnx_t::parse_cnx_closed(bt_ecnx_vapi_t &ecnx_vapi, const bt_ecnx_event_t &ecnx_event)	throw()
{
	bt_swarm_t *	bt_swarm	= swarm_sched->bt_swarm();
	// log to debug
	KLOG_DBG("enter ecnx_event=" << ecnx_event);
	// remove this ecnx_vapi from the bt_swarm->ecnx_vapi_db
	bt_swarm->ecnx_vapi_unlink(&ecnx_vapi);	
	// update prarity in bt_pselect_vapi_t with the ecnx_vapi bt_piece_avail_t
	bt_swarm->pselect_vapi()->dec_remote_pfreq( ecnx_vapi.remote_pavail() );
	// try to launch request on all idle connections
	// - in case they could replace the just deleted bt_swarm_sched_request_t
	swarm_sched->try_send_request_on_idle_cnx();
	// return tokeep
	return true;
}


/** \brief parse a bt_ecnx_event_t::BLOCK_REP
 */
bool	bt_swarm_sched_ecnx_t::parse_block_rep(bt_ecnx_vapi_t &ecnx_vapi, const bt_ecnx_event_t &ecnx_event)	throw()
{
	bt_swarm_sched_request_t *	sched_req;
	datum_t *			recved_data	= ecnx_event.get_block_rep(&sched_req);
	// log to debug
	KLOG_DBG("enter ecnx_event=" << ecnx_event);
	
	// if this BLOCK_REP did recved data, notify them to the matching block
	// - aka if it is not a null BLOCK_REP
	if( sched_req ){
		bt_swarm_sched_block_t *sched_block	= sched_req->get_sched_block();
		sched_block->notify_complete_from(sched_req->get_sched_cnx(), *recved_data);
	}

	// try to send request on this bt_ecnx_vapi_t IIF it is an empty BLOCK_REP
	if( !sched_req )	try_send_request(ecnx_vapi);
	
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END





