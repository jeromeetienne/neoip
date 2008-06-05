/*! \file
    \brief Definition of the \ref btrelay_swarm_t
    


\par Brief Description
btrelay_swarm_t hold a bt_ezswarm_t.
there cant be a btrelay_itor_t and btrelay_swarm_t for the same link (in relation
with the multiple link in btrelay_apps_t which is not yet implemented)

\par TODO issue with link_type 'cast
- it send a LOT of useless bt_utmsg_piecewish_t
- one for *each* piece of bt_mfile_t which is not in the pieceq
  - well in fact 2, one dowish followed immediatly by one nowish
- this is due to a flackyness in bt_relay_t which overselect the piece
- this is specific to bt_relay_t and is a known issue with it
- all the estimation of how valuable is a piece is bogus

\par About link_type "cast" and bt_utmsg_bcast_handler_t
- btrelay_swarm_t hooks a bt_utmsg_bcast_handler_t to gather the
  casti_swarm_t::UTMSG_BCAST_KEY messages
  broadcasted by neoip-casti, aka bt_cast_udata_t
- at each reception of bt_cast_udata_t, the bt_relay_t is reevaluated
  - this provides a kind of synchronisation between the 2
- this is used to determine which pieces are still valid and which are obsoletes
- this is used to delete the pieces which have been downloaded by the local peer
  and are now obsolete.
  - without this, the piece will remains cached by bt_relay_t and on the next
    iteration of the circularidx, this obsolete piece will be confused with the
    actual one with the same pieceidx.
  - and thus it will wrongly delivered and introduce bad data into the swarm.
- technic: this is done by reviewing the current bt_pieceprec_arr_t of the bt_swarm_t
  - if the pieceidx is currently marked as needed but is no more in the bt_cast_mdata_t
    pieceq, then it is forced as bt_pieceprec_t::NONEEDED

\par About cpu performance
- bt_relay_t is coded such as it is directly depending on the total number of piece
  - so its performance is related to the sum of bt_mfile_t::nb_piece() for
    ALL the attached bt_swarm_t
- additionnaly the setting of new bt_pieceprec_arr_t is slow too as it depends
  on bt_mfile_t::nb_piece() too
- in case of link_type "cast", it is even worst because each reception 
  of bt_cast_udata_t, does an additional set of bt_pieceprec_arr_t to ensure
  the piece obsoleted by neoip-casti, are not kept by bt_relay_t
- on the other hand, this is the reevaluation of every bt_pieceprec_arr_t is 
  a rather rare event.

\par About delseed process - experimentation
- to keep seed connection when all the relayed pieces have been downloaded is useless
  - remote peer with all pieces available will never download piece from bt_relay_t
- to keep connection with seed remote peer reduces the efficiency of the relay
  - because seed remote peers uses connections which may not be used by leech connection
  - so there are less leech connections which may benefit from the bt_relay_t 
- This is valid IIF cast_mdata.is_null() because the notion of 'seed' for bt_swarm_t
  used in 'cast' is not valid.
- to solve this, there is a 'delseed' process which 
  - periodically check if all pieces elected for download by bt_relay_t have been downloaded
    - this is done with bt_pselect_vapi_t 
    - bt_pselect_vapi_t::local_pdling MUST be all zero and has_selectable MUST be false
  - if they are downloaded, it deletes all bt_swarm_full_t which are seed 
  - and leave all bt_swarm_full_t which are leech
- NOTE: this is rather kludgy because it deletes internal stuff directly in the bt_swarm_t
  - a more controlled way would be better
  - but currenlty i have no brain and so this is the 'best' stuff i came up with
  - it is not even clear that it works... but i least it tries
  - the implemetation is bad because dirty
  - the solution seems to produce quite flacky stuff in the btrelay too
    - it download a lot more
    - like if deleting the seed bt_swarm_full_t would change the piece precedence
      computing from the bt_relay_t
  - the other 'best' solution is to put a very large bt_swarm_profile_t::NO_NEW_FULL_LIMIT
    - thus a lot of leech are connection even if there is a majority of seed 
      in the swarm
    - bad too :)
- NOTE: the second one
  - the issue was clear when using the ubuntu 7.04 livecd which has 10seed for 1 leech
    - so one which is rather useless/hard to relay
  - when relaying the ubuntu 7.04 dvd which has 1 seed per leech, the story is 
    WAY different
    - the relay works ok without the delseed experimentation
    
*/

/* system include */
/* local include */
#include "neoip_btrelay_swarm.hpp"
#include "neoip_btrelay_apps.hpp"

#include "neoip_bt_relay.hpp"

#include "neoip_casti_swarm.hpp"	// to get casti_swarm_t::UTMSG_BCAST_KEY

#include "neoip_bt_cast_udata.hpp"
#include "neoip_bt_cast_pidx.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_bt_swarm_utmsg.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_helper.hpp"

#include "neoip_bt_utmsg_bcast.hpp"
#include "neoip_bt_utmsg_bcast_handler.hpp"

#include "neoip_pkt.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
btrelay_swarm_t::btrelay_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	bt_ezswarm	= NULL;
	bcast_handler	= NULL;
}

/** \brief Destructor
 */
btrelay_swarm_t::~btrelay_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// if currently linked to relay, unlink it 
	if( bt_ezswarm && bt_ezswarm->in_share() )	unlink_relay();
	// unlink this object from the bt_btrelay_apps_t
	if( btrelay_apps )	btrelay_apps->swarm_unlink(this);
	// delete the bt_utmsg_bcase_handler_t if needed
	nipmem_zdelete	bcast_handler;
	// delete the bt_ezswarm_t if needed
	nipmem_zdelete	bt_ezswarm;
}


/** \brief autodelete this object, and return false for tokeep convenience
 */
bool	btrelay_swarm_t::autodelete(const std::string &reason)		throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR("autodelete due to " << reason);
	// autodelete this 
	nipmem_delete	this;
	// return false
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t btrelay_swarm_t::start(btrelay_apps_t *btrelay_apps, const bt_mfile_t &bt_mfile
				, const bt_cast_mdata_t &m_cast_mdata)		throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->btrelay_apps	= btrelay_apps;
	this->m_cast_mdata	= m_cast_mdata;
	
	// link this object to the bt_btrelay_apps_t
	btrelay_apps->swarm_dolink(this);

	// start the bt_ezswarm_t
	bt_err_t	bt_err;
	bt_err		= launch_ezswarm(bt_mfile);
	if( bt_err.failed() )	return bt_err;
	
	// start the delseed_timeout IIF cast_mdata.is_null()
	if( cast_mdata().is_null() )
		delseed_timeout.start(btrelay_apps->profile().delseed_period(), this, NULL);
	
	// return no error
	return bt_err_t::OK;
}

/** \brief Launch the bt_ezswarm_t
 */
bt_err_t	btrelay_swarm_t::launch_ezswarm(const bt_mfile_t &bt_mfile)	throw()
{
	bt_ezsession_t *	bt_ezsession	= btrelay_apps->bt_ezsession();
	bt_swarm_resumedata_t	swarm_resumedata;
	bt_err_t		bt_err;
	
	// build the bt_swarm_resumedata_t from the resulting bt_mfile_t
	swarm_resumedata	= bt_swarm_resumedata_t::from_mfile(bt_mfile);

	// sanity check - the bt_swarm_resumedata_t MUST check().succeed()
	DBG_ASSERT( swarm_resumedata.check().succeed() );	
	// sanity check - the bt_ezsession_t MUST already bt init
	DBG_ASSERT( bt_ezsession );

	// build the bt_ezsession_profile_t
	bt_ezswarm_profile_t	ezswarm_profile;
	bt_swarm_sched_profile_t &sched_profile	= ezswarm_profile.swarm().sched();
	// copy the kad_peer_t pointer from the bt_ezsession_t to the bt_ezswarm_profile_t
	ezswarm_profile.peersrc_kad_peer(bt_ezsession->kad_peer());
	// copy the io_pfile_dirname for the bt_io_vapi_t
	ezswarm_profile.io_pfile().dest_dirpath		(btrelay_apps->io_pfile_dirpath());
	// set the bt_pselect_policy_t::FIXED - required for bt_relay_t
	ezswarm_profile.swarm().pselect_policy		(bt_pselect_policy_t::FIXED);
	// set the bt_peerpick_mode_t::RELAY - to give as much as requested for relay
	ezswarm_profile.peerpick_mode			(bt_peerpick_mode_t::RELAY);
	// set the bt_swarm_profile_t::no_new_full_limit to 1000
	// - GOAL: always have a large number of leech, thus potential people willing
	//   to download from me.
	ezswarm_profile.swarm().no_new_full_limit	(1000);
	// set the bt_swarm_sched_profile_t::request_timeout_endgame as request_timeout_normal
	// - it cancels any changes for the bt_swarm_sched_t::in_endgame() case
	// - it is done because in_endgame() tend to duplicate the request and so
	//   download multiple times.
	// - this is ok when the goal is the download speed, but neoip-btrelay aims
	//   to give as much as possible, thus duplicating download is against this goal
	sched_profile.request_timeout_endgame		(sched_profile.request_timeout_normal());
	// sanity check - at this point, the ezswarm_profile MUST be check().succeed()
	DBG_ASSERT( ezswarm_profile.check().succeed() );

	// set the bt_ezswarm_opt_t
	bt_ezswarm_opt_t	ezswarm_opt;
	ezswarm_opt	|= bt_ezswarm_opt_t::IO_PFILE;
	ezswarm_opt	|= bt_ezswarm_opt_t::ECNX_HTTP;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_KAD;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_NSLAN;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_UTPEX;
	// if the bt_mfile_t has a announce_uri(), use the bt_peersrc_http_t too
	if( !bt_mfile.announce_uri().is_null())	ezswarm_opt |= bt_ezswarm_opt_t::PEERSRC_HTTP;

	// start a bt_swarm_t depending on the availability of a bt_swarm_resumedata_t
	bt_ezswarm	= nipmem_new bt_ezswarm_t();
	bt_err		= bt_ezswarm->set_profile(ezswarm_profile)
				.start(swarm_resumedata, ezswarm_opt, bt_ezsession, this, NULL);
	if( bt_err.failed() )	return bt_err;
	// make bt_ezswarm_t to gracefully goto SHARE
	bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::SHARE);

	// return no error
	return bt_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_relay_t linking
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink this bt_swarm_t to the bt_relay_t
 */
void	btrelay_swarm_t::dolink_relay()	throw()
{
	bt_relay_t * bt_relay	= btrelay_apps->bt_relay();
	// sanity check - the bt_ezswarm_t MUST be in SHARE
	DBG_ASSERT( bt_ezswarm );
	DBG_ASSERT( bt_ezswarm->in_share() );
	// get an alias on bt_swarm;
	bt_swarm_t * bt_swarm	= bt_ezswarm->share()->bt_swarm();
	// unlink the bt_swarm_t
	bt_relay->swarm_dolink( bt_swarm );
}

/** \brief UnLink this bt_swarm_t from the bt_relay_t
 */
void	btrelay_swarm_t::unlink_relay()	throw()
{
	bt_relay_t * bt_relay	= btrelay_apps->bt_relay();
	// sanity check - the bt_ezswarm_t MUST be in SHARE
	DBG_ASSERT( bt_ezswarm );
	DBG_ASSERT( bt_ezswarm->in_share() );
	// unlink the bt_swarm_t
	bt_swarm_t * bt_swarm	= bt_ezswarm->share()->bt_swarm();
	bt_relay->swarm_unlink( bt_swarm );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_ezswarm_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_ezswarm_t when to notify an event
 */
bool 	btrelay_swarm_t::neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw()
{
	// log to debug
	KLOG_ERR("enter ezevent=" << ezswarm_event);
	

	// handle the bt_ezswarm_event_t depending of its type
	switch(ezswarm_event.get_value()){
	case bt_ezswarm_event_t::ENTER_STATE_POST:
		// if just entered in bt_ezswarm_state_t::SHARE, dolink in bt_relay_t
		if( bt_ezswarm->in_share() ){
			dolink_relay();
			// ctor the bcast_handler if cast_mdata() is not null
			if( !cast_mdata().is_null() )	bcast_handler_ctor();
		}
		// if it is a fatal bt_ezswarm_t, autodelete
		if( bt_ezswarm->in_error() ){
			KLOG_ERR("Received a fatal event " + bt_ezswarm->cur_state().reason());
			return autodelete();
		}
		break;
	case bt_ezswarm_event_t::LEAVE_STATE_PRE:
		// if about to leave bt_ezswarm_state_t::SHARE, unlink in bt_relay_t
		if( bt_ezswarm->in_share() ){
			unlink_relay();
			// dtor the bcast_handler if cast_mdata() is not null
			if( !cast_mdata().is_null() )	bcast_handler_dtor();
		}
		break;
	default:	break;
	}
	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_utmsg_bcast_handler_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build the bt_utmsg_bcast_handler_t for bt_utmsgtype_t::BCAST
 */
void	btrelay_swarm_t::bcast_handler_ctor()	throw()
{
	// sanity check - bcast_handler MUST NOT be set
	DBG_ASSERT( bcast_handler == NULL );
	// sanity check - bt_ezswarm MUST be in share
	DBG_ASSERT( bt_ezswarm->in_share() ); 

	// get the pointer on bt_utmsg_bcast_t
	bt_swarm_t *		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	bt_swarm_utmsg_t *	swarm_utmsg	= bt_swarm->swarm_utmsg();
	bt_utmsg_vapi_t*	utmsg_vapi	= swarm_utmsg->utmsg_vapi(bt_utmsgtype_t::BCAST);
	bt_utmsg_bcast_t *	utmsg_bcast	= dynamic_cast<bt_utmsg_bcast_t *>(utmsg_vapi); 
	// sanity check - utmsg_bcast MUST NOT be null
	DBG_ASSERT( utmsg_bcast );	

	// start the bcast_handler
	bt_err_t	bt_err;
	bcast_handler	= nipmem_new bt_utmsg_bcast_handler_t();
	// TODO replace this hardcoded "casti" by a casti_swarm_t::UTMSG_BCAST_KEY
	bt_err		= bcast_handler->start(utmsg_bcast, "casti", this, NULL);
	DBG_ASSERT( bt_err.succeed() );
}

/** \brief Destruct the bt_utmsg_bcast_handler_t
 */
void	btrelay_swarm_t::bcast_handler_dtor()	throw()
{
	// sanity check - bcast_handler MUST be set
	DBG_ASSERT( bcast_handler );
	// sanity check - bt_ezswarm MUST be in share
	DBG_ASSERT( bt_ezswarm->in_share() ); 
	// delete the bcast_handler
	nipmem_zdelete	bcast_handler;
}

/** \brief callback notified by \ref bt_utmsg_vapi_t when to notify a recved pkt_t
 */
bool	btrelay_swarm_t::neoip_bt_utmsg_bcast_handler_cb(void *cb_userptr
						, bt_utmsg_bcast_handler_t &cb_bcast_handler
						, pkt_t &recved_pkt)	throw()
{
	bt_swarm_t *		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// log to debug
	KLOG_DBG("enter recved_pkt=" << recved_pkt);
	// sanity check - bcast_handler MUST be set
	DBG_ASSERT( bcast_handler );
	
	// read a bt_cast_udata_t from the received packet
	bt_cast_udata_t	cast_udata;
	try {
		recved_pkt >> cast_udata;
	}catch(serial_except_t &e){
		KLOG_ERR("failed to parse bt_cast_udata_t in " << recved_pkt);
		return true;
	}

	// update the cast_mdata with the received bt_cast_udata_t
	m_cast_mdata.updated_with(cast_udata, bt_mfile);

	// log to debug
	KLOG_ERR("cast_udata=" << cast_udata);

	// notify the bt_relay_t to reevaluate now
	// - NOTE: this is a synchronisation with the bt_cast_udata_t... somehow
	bt_relay_t * bt_relay	= btrelay_apps->bt_relay();
	bt_relay->reevaluate_now();

#if 1
/* NOTE:
 * - the following code ensure that all pieces which are no more in pieceq are not handled
 * - here handled means being locally available or being currently in downloading
 * - the implementation go thru EACH pieceidx of the bt_mfile
 *   - if the pieceidx is_in the pieceq, do nothing
 *   - if the pieceidx is currently locally available, declare it nomore_avail
 *   - if the pieceidx IS NOT in bt_pieceprec_t::NOTNEEDED, force it to bt_pieceprec_t::NOTNEEDED
 *     - this will remove all piece in dling which are out of the pieceq
 * - ISSUE: this is handling every pieceidx. aka the cpu efficiency is proportionnal to 
 *   the bt_mfile.nb_piece()
 *   - while in fact only a few pieces different
 *   - look at casti_swarm_profile_t::xmit_udata_max_piece() which is currently 4
 */
 
	// create some alias to ease the readability
	bt_cast_pidx_t	cast_pidx	= bt_cast_pidx_t().modulo(bt_mfile.nb_piece());
	size_t		pieceq_beg	= cast_mdata().pieceq_beg();
	size_t		pieceq_end	= cast_mdata().pieceq_end();

	// reinit the pieceprec_arr with a forced bt_pieceprec_t::NOTNEEDED for
	// all the pieces which are no more in the pieceq
	bt_pieceprec_arr_t	pieceprec_arr	= bt_swarm_helper_t::get_pieceprec_arr(bt_swarm);
	// go thru each pieceidx
	for(size_t pieceidx = 0; pieceidx < bt_mfile.nb_piece(); pieceidx++ ){
		// if this piece is in the pieceq, goto the next
		if( cast_pidx.index(pieceidx).is_in(pieceq_beg,pieceq_end) )	continue;
		// if this piece is not needed, goto the next 
		if( pieceprec_arr[pieceidx].is_notneeded() )			continue;
		// if this piece is needed and not in pieceq, set it to NOTNEEDED
		pieceprec_arr[pieceidx]	= bt_pieceprec_t::NOTNEEDED;
	}
	// set back the bt_pieceprec_arr to the bt_swarm_t and delete notneeded piece
	bt_swarm_helper_t::pieceprec_arr_del_notneeded(bt_swarm, pieceprec_arr);
#endif
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 * 
 */
bool btrelay_swarm_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug 
	KLOG_ERR("enter");

#if 0	// NOTE: experimental code for delseed

	// sanity check - cast_mdata MUST be null
	DBG_ASSERT( cast_mdata().is_null() ); 
	
	// if the bt_ezswarm_t is not in share, do nothing and return now
	if( !bt_ezswarm->in_share() )	return true;

	// compute some alias to ease readability
	bt_swarm_t *		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
	// if bt_swarm_t is still downloading pieces, do nothing and return true
	if( pselect_vapi->local_pdling().is_any_set() )	return true;
	// if pselect_vapi still has selectable pieces, do nothing and return true
	if( pselect_vapi->has_selectable() )		return true;
	
	// log to debug
	KLOG_ERR("try to delete all the bt_swarm_full_t which are seed.");
	
	// delete all bt_swarm_full_t which are seed
	// - it operate on a copy of bt_swarm_full_t as it may 'delete during walk'
	std::list<bt_swarm_full_t *>	swarm_full_db	= bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::iterator iter;
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_t *	swarm_full	= *iter;
		// if this bt_swarm_full_t is leech, goto the next
		if( swarm_full->is_leech() )	continue;
		// delete the bt_swarm_full_t
		nipmem_zdelete	swarm_full;
	}
#endif
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END;




