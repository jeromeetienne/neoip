/*! \file
    \brief Class to handle the bt_swarm_sched_full_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched_full.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_sched_profile.hpp"
#include "neoip_bt_swarm_sched_piece.hpp"
#include "neoip_bt_swarm_sched_block.hpp"
#include "neoip_bt_swarm_sched_request.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_event.hpp"
#include "neoip_bt_swarm_full_prec.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_peersrc.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_bt_pieceavail.hpp"
#include "neoip_bt_peerpick_plain.hpp"
#include "neoip_pkt.hpp"
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
bt_swarm_sched_full_t::bt_swarm_sched_full_t(bt_swarm_sched_t *swarm_sched)	throw()
{
	// zero some parameter
	this->m_peerpick_vapi	= NULL;
	// copy the parameter
	this->swarm_sched	= swarm_sched;
}

/** \brief Destructor
 */
bt_swarm_sched_full_t::~bt_swarm_sched_full_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief  try to launch request on all the bt_swarm_full_t which dont have any request
 */
void	bt_swarm_sched_full_t::try_send_request_on_idle_cnx()	throw()
{
	bt_swarm_t *				bt_swarm	= swarm_sched->bt_swarm();
	std::list<bt_swarm_full_t *> &		swarm_full_db	= bt_swarm->get_full_db();	
	std::list<bt_swarm_full_t *>::iterator	iter;
	// go thru the whole swarm_full_db
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_t * 	swarm_full	= *iter;
		bt_swarm_full_sched_t &	full_sched	= *swarm_full->full_sched();
		// if this bt_swarm_full_t has pending request, it is not idle, so skip it
		if( full_sched.nb_pending_request() )	continue;
		// else try to send some request on it
		try_send_request(*swarm_full);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      piece selection
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief try to send a bt_swarm_sched_request_t on this bt_swarm_full_t
 */
void	bt_swarm_sched_full_t::try_send_request(bt_swarm_full_t &swarm_full)	throw()
{
	bt_swarm_t *		bt_swarm	= swarm_sched->bt_swarm();
	const bt_swarm_profile_t &swarm_profile	= bt_swarm->profile();
	const bt_swarm_sched_profile_t &profile	= swarm_sched->profile();
	bt_swarm_full_sched_t &	full_sched	= *swarm_full.full_sched();
	bt_swarm_sched_cnx_vapi_t *sched_cnx	= &full_sched;
	size_t			request_nb_max	= full_sched.req_queue_maxlen() / swarm_profile.xmit_req_maxlen();
	size_t			pieceidx_hint	= swarm_sched->next_pidx_hint();
	
	// log to debug
	KLOG_DBG("enter");
	// if this bt_swarm_full_t has doauth_req == false, dont send any request
	if( !swarm_full.other_doauth_req() )		return;

	// loop until the maximum number of sched_request are pending
	while( full_sched.nb_pending_request() < request_nb_max ){
		bt_swarm_sched_piece_t *	sched_piece;
		// try to get a bt_swarm_sched_piece_t to request
		sched_piece	= swarm_sched->select_piece_to_request(sched_cnx
						, swarm_full.remote_pavail(), pieceidx_hint);
		// if no sched_piece can be request, return now
		if( !sched_piece )	return;
		// compute the req_timeout depending on if bt_swarm_sched_t is in_endgame()
		delay_t	req_timeout;
		if( swarm_sched->in_endgame() )	req_timeout = profile.request_timeout_endgame();
		else				req_timeout = profile.request_timeout_normal();		
		// sanity check - the sched_piece MUST be remotely available
		DBG_ASSERT( swarm_full.remote_pavail().is_avail(sched_piece->pieceidx()) );
		// launch as many requests as possible on this sched_piece
		while( full_sched.nb_pending_request() < request_nb_max ){
			bt_swarm_sched_block_t *	sched_block;
			// get a bt_swarm_sched_block_t to request
			sched_block	= sched_piece->get_block_to_request(sched_cnx);
			// if this sched_piece has no sched_block to request, leave the loop
			// - a new piece will be selected
			if( !sched_block )	break;
			// launch a request on the first block which isnt locally avail and which needs request
			// - FIXME to make the request timeout tunable would be better
			//   - yep but the goal is unclear
			nipmem_new bt_swarm_sched_request_t(sched_cnx, sched_block,req_timeout);
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
//                     peer selection 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Update the peer selection
 */
void	bt_swarm_sched_full_t::peer_select_update()				throw()
{
	// sanity check - bt_peerpick_vapi_t MUST be set
	DBG_ASSERT( peerpick_vapi() );
	// update the peer pick
	peerpick_vapi()->peerpick_update();
}

/** \brief kludgy way for initializing bt_peerpick_t
 * 
 * - TODO real kludgy the way it is passed thru here is quite stealthy
 *   - it may very well be forgotten, i dont like it at all
 *   - all the init of bt_peerpick_vapi_t is crappy, see neoip_bt_peerpick_plain.cpp
 *     header for details
 */
void	bt_swarm_sched_full_t::peerpick_vapi(bt_peerpick_vapi_t *new_peerpick)	throw()
{
	// sanity check - bt_peerpick_vapi_t MUST be null
	DBG_ASSERT( m_peerpick_vapi == NULL );
	// copy the new value
	m_peerpick_vapi	= new_peerpick;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bt_swarm_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_swarm_full_t to provide event
 * 
 * @return a tokeep for the bt_swarm_full_t
 */
bool	bt_swarm_sched_full_t::neoip_bt_swarm_full_cb(void *cb_userptr, bt_swarm_full_t &swarm_full
						, const bt_swarm_full_event_t &full_event)	throw() 
{
	// log to debug
	KLOG_DBG("enter event=" << full_event);

	// parse the event according to its value
	switch( full_event.get_value() ){
	case bt_swarm_full_event_t::CNX_OPENED:		return parse_cnx_opened(swarm_full, full_event);
	case bt_swarm_full_event_t::CNX_CLOSED:		return parse_cnx_closed(swarm_full, full_event);
	case bt_swarm_full_event_t::REQAUTH_TIMEDOUT:	return parse_reqauth_timedout(swarm_full, full_event);
	case bt_swarm_full_event_t::UNAUTH_REQ:		return parse_unauth_req(swarm_full, full_event);
	case bt_swarm_full_event_t::DOAUTH_REQ:		return parse_doauth_req(swarm_full, full_event);
	case bt_swarm_full_event_t::UNWANT_REQ:		return parse_unwant_req(swarm_full, full_event);
	case bt_swarm_full_event_t::DOWANT_REQ:		return parse_dowant_req(swarm_full, full_event);
	case bt_swarm_full_event_t::PIECE_ISAVAIL:	return parse_piece_isavail(swarm_full, full_event);
	case bt_swarm_full_event_t::PIECE_UNAVAIL:	return parse_piece_unavail(swarm_full, full_event);
	case bt_swarm_full_event_t::PIECE_BFIELD:	return parse_piece_bfield(swarm_full, full_event);
	case bt_swarm_full_event_t::BLOCK_REP:		return parse_block_rep(swarm_full, full_event);
	case bt_swarm_full_event_t::PWISH_DOINDEX:	return parse_pwish_doindex(swarm_full, full_event);
	case bt_swarm_full_event_t::PWISH_NOINDEX:	return parse_pwish_noindex(swarm_full, full_event);
	case bt_swarm_full_event_t::PWISH_DOFIELD:	return parse_pwish_dofield(swarm_full, full_event);
	default:	DBG_ASSERT( 0 );
	}
	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief parse a cnx_opened event
 */
bool	bt_swarm_sched_full_t::parse_cnx_opened(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	bt_pselect_vapi_t *	pselect_vapi	= swarm_sched->bt_swarm()->pselect_vapi();
	// log to debug
	KLOG_WARN("enter bt_swarm_full=" << &swarm_full);

	// update the bt_pselect_vapi_t
	// - NOTE: no need to update pselect_vapi->remote_pfreq as remote_pavail is full-zero
	//   because the bitfield of the pieces available is not yet received
	pselect_vapi->inc_remote_pwish( swarm_full.remote_pwish()  );
	
	// return tokeep
	return true;
}


/** \brief parse a cnx_closed event
 */
bool	bt_swarm_sched_full_t::parse_cnx_closed(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	bt_swarm_full_sched_t * full_sched	= swarm_full.full_sched();
	bt_pselect_vapi_t *	pselect_vapi	= swarm_sched->bt_swarm()->pselect_vapi();
	// log to debug
	KLOG_WARN("enter bt_swarm_full=" << &swarm_full);
	
	// update the bt_pselect_vapi_t
	pselect_vapi->dec_remote_pfreq( swarm_full.remote_pavail() );
	pselect_vapi->dec_remote_pwish( swarm_full.remote_pwish()  );
	
	// close the pending request on this bt_swarm_full_t
	bt_swarm_full_sched_t::request_db_t & request_db = swarm_full.full_sched()->get_request_db();
	while( !request_db.empty() )	nipmem_delete *request_db.begin();
	// try to launch request on all idle connections
	// - in case they could replace the just deleted bt_swarm_sched_request_t
	swarm_sched->try_send_request_on_idle_cnx();
	
	// if this bt_swarm_full_t WAS NOT in bt_reqauth_type_t::DENY, relaunch a new one to replace it
	if( full_sched->reqauth_type() != bt_reqauth_type_t::DENY ){
		// pass it in NONE to prevent it from interfering in the new computation
		full_sched->reqauth_type(bt_reqauth_type_t::NONE);
		// update the peer selection to replace this one
		peer_select_update();
		// if needed, send the reqauth command in sync with the current bt_reqauth_type_t
		// - NOTE: quite useless as the bt_swarm_full_t gonna be closed but to 
		//   comply to the 'regularity rules'. it wasnt there before and i asked myself
		//   the question several times.
		full_sched->sync_doauth_req_cmd();
	}
	
	// notify the bt_swarm_peersrc_t that it may launch new bt_swarm_itor_t
	// - as one bt_swarm_full_t gonna be closed, it may leave a empty room for new itor
	bt_swarm_peersrc_t *	swarm_peersrc	= swarm_sched->bt_swarm()->swarm_peersrc();
	swarm_peersrc->may_launch_new_itor();

	// return tokeep
	return true;
}

/** \brief parse a reqauth_timedout event
 */
bool	bt_swarm_sched_full_t::parse_reqauth_timedout(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	bt_swarm_full_sched_t * full_sched	= swarm_full.full_sched();	
	// log to debug
	KLOG_WARN("enter");
	
	// set this bt_swarm_full_sched_t in bt_reqauth_type_t::DENY
	full_sched->reqauth_type(bt_reqauth_type_t::DENY);
	// update the peer selection
	peer_select_update();
	// if needed, send the reqauth command in sync with the current bt_reqauth_type_t
	full_sched->sync_doauth_req_cmd();
	
	// if swarm_full is still in bt_reqauth_type_t::DENY after peer_select_update()
	// THEN zero the bt_swarm_full_prec_t::reqauth_prec_xmit 
	if( full_sched->reqauth_type() == bt_reqauth_type_t::DENY )
		swarm_full.full_prec()->reqauth_prec_xmit(0);
		

	// return tokeep
	return true;
}

/** \brief parse a unauth_req event
 */
bool	bt_swarm_sched_full_t::parse_unauth_req(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	// log to debug
	KLOG_WARN("enter");

	// close the pending request on this bt_swarm_full_t
	bt_swarm_full_sched_t::request_db_t & request_db = swarm_full.full_sched()->get_request_db();
	while( !request_db.empty() )	nipmem_delete *request_db.begin();
	// try to launch request on all idle connections
	// - in case they could replace the just deleted bt_swarm_sched_request_t
	swarm_sched->try_send_request_on_idle_cnx();

	// return tokeep
	return true;
}

/** \brief parse a doauth_req event
 */
bool	bt_swarm_sched_full_t::parse_doauth_req(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// try to send a request on this bt_swarm_full_t
	try_send_request(swarm_full);
	// return tokeep
	return true;
}


/** \brief parse a unwant_req event
 */
bool	bt_swarm_sched_full_t::parse_unwant_req(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// return tokeep
	return true;
}

/** \brief parse a dowant_req event
 */
bool	bt_swarm_sched_full_t::parse_dowant_req(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// update the peer selection as this new peer wanting to request may change it
	peer_select_update();
	// return tokeep
	return true;
}

/** \brief parse a piece_isavail event
 */
bool	bt_swarm_sched_full_t::parse_piece_isavail(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	bt_pselect_vapi_t *	pselect_vapi	= swarm_sched->bt_swarm()->pselect_vapi();
	size_t			pieceidx	= full_event.get_piece_isavail();
	// log to debug
	KLOG_WARN("enter swarm_full=" << &swarm_full);
	// update remote_pfreq in the bt_pselect_vapi_t
	pselect_vapi->inc_remote_pfreq( pieceidx );

	// try to send a request on this bt_swarm_full_t - in case this new piece may be requested now
	try_send_request(swarm_full);
	// return tokeep
	return true;
}

/** \brief parse a piece_unavail event
 */
bool	bt_swarm_sched_full_t::parse_piece_unavail(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	bt_pselect_vapi_t *	pselect_vapi	= swarm_sched->bt_swarm()->pselect_vapi();
	bt_swarm_full_sched_t *	full_sched	= swarm_full.full_sched();
	size_t			pieceidx	= full_event.get_piece_unavail();
	// log to debug
	KLOG_WARN("enter");
	// update remote_pfreq in the bt_pselect_vapi_t
	pselect_vapi->dec_remote_pfreq( pieceidx );

	// if requests are pending on this swarm_full for this pieceidx, delete them
	while( true ){
		bt_swarm_sched_request_t* sched_request;
		// try to find a sched_request for this pieceidx
		sched_request	= full_sched->request_by_pieceidx(pieceidx);
		// if none is found, leave the loop
		if( !sched_request )	break;
		// delete sched_request and go on the loop
		nipmem_delete sched_request;
	}

	// try to send a request on this bt_swarm_full_t - in case this new piece may be requested now
	try_send_request(swarm_full);
	// return tokeep
	return true;
}

/** \brief parse a piece_bfield event
 */
bool	bt_swarm_sched_full_t::parse_piece_bfield(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	bt_pselect_vapi_t *	pselect_vapi	= swarm_sched->bt_swarm()->pselect_vapi();
	const bt_pieceavail_t &	remote_pavail	= swarm_full.remote_pavail();
	// log to debug
	KLOG_WARN("enter swarm_full=" << &swarm_full);
	// update remote_pfreq in the bt_pselect_vapi_t
	pselect_vapi->inc_remote_pfreq( remote_pavail );

	// try to send a request on this bt_swarm_full_t
	try_send_request(swarm_full);
	// return tokeep
	return true;
}

/** \brief parse a block_rep event
 */
bool	bt_swarm_sched_full_t::parse_block_rep(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
	bt_swarm_t *		bt_swarm	= swarm_sched->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	bt_cmd_t		bt_cmd;
	pkt_t *			reply_pkt	= full_event.get_block_rep(bt_cmd);
	const bt_prange_t &	cmd_prange	= bt_cmd.prange();
	// log to debug
	KLOG_DBG("enter bt_cmd=" << bt_cmd);

	// get the matching sched_piece
	bt_swarm_sched_piece_t*	sched_piece	= swarm_sched->piece_by_idx(cmd_prange.piece_idx());
	// sanity check - a sched_piece MUST match this piece_idx
	DBG_ASSERT( sched_piece );

	// get the matching sched_block withing the sched_piece
	bt_swarm_sched_block_t*	sched_block	= sched_piece->block_by_totfile_beg(cmd_prange.totfile_beg(bt_mfile));
	// NOTE: at this point, the block_rep MUST match a pending request
	// - this condition is checked before reaching here in bt_swarm_full_t handler for BLOCK_REP
	// sanity check - the sched_block MUST have been found
	DBG_ASSERT( sched_block );
	// sanity check - the length of this sched_block MUST match the reply one
	DBG_ASSERT( sched_block->length() == cmd_prange.length() );

	// notify the sched_block that it has been replied
	// - NOTE: this method delete the sched_block, DO NOT use it after that
	datum_t	reply_datum	= reply_pkt->to_datum(datum_t::NOCOPY);
	sched_block->notify_complete_from(swarm_full.full_sched(), reply_datum);

	// try to send a request on this bt_swarm_full_t
	try_send_request(swarm_full);

	// return tokeep
	return true;
}

/** \brief parse a pwish_doindex event
 */
bool	bt_swarm_sched_full_t::parse_pwish_doindex(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
#if 1
	bt_pselect_vapi_t *	pselect_vapi	= swarm_sched->bt_swarm()->pselect_vapi();
	size_t			pieceidx	= full_event.get_pwish_doindex();
	// update remote_pwish in the bt_pselect_vapi_t
	pselect_vapi->inc_remote_pwish( pieceidx );
#endif
	// TODO do something else ? try sending new request ? other ? 

	// return tokeep
	return true;
}

/** \brief parse a pwish_noindex event
 */
bool	bt_swarm_sched_full_t::parse_pwish_noindex(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
#if 1
	bt_pselect_vapi_t *	pselect_vapi	= swarm_sched->bt_swarm()->pselect_vapi();
	size_t			pieceidx	= full_event.get_pwish_noindex();
	// update remote_pwish in the bt_pselect_vapi_t
	pselect_vapi->dec_remote_pwish( pieceidx );
#endif

	// TODO do something else ? try sending new request ? other ? 

	// return tokeep
	return true;
}

/** \brief parse a pwish_dofield event
 */
bool	bt_swarm_sched_full_t::parse_pwish_dofield(bt_swarm_full_t &swarm_full
				, const bt_swarm_full_event_t &full_event)	throw()
{
#if 1
	bt_pselect_vapi_t *	pselect_vapi	= swarm_sched->bt_swarm()->pselect_vapi();
	const bitfield_t *	old_pwish_field;
	const bitfield_t *	new_pwish_field	= full_event.get_pwish_dofield(&old_pwish_field);
	// update remote_pwish in the bt_pselect_vapi_t
	pselect_vapi->dec_remote_pwish( *old_pwish_field );
	pselect_vapi->inc_remote_pwish( *new_pwish_field );
#endif
	// TODO do something else ? try sending new request ? other ? 

	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END





