/*! \file
    \brief Definition of the \ref casto_swarm_t


- TODO clean up this long comment

- TODO this doesnt handle the bt_ezswarm_state_t::ERROR


\par Brief Description
casto_swarm_t hold a bt_ezswarm_t and all the attached bt_httpo_full_t.


- TODO it is now possible to handle the http error in the connection
  - handle it here for httpo_full_t

\par Misc to sort
- TODO handle the peer/piece slow/fast ala azureus
  - currently requesting a block from a peer will prevent it from being 
    requested on another peer until the first request timeout.
  - so if the first peer is slow, the piece may wait a 'long' time before being
    completed.
  - this is espeically important when there are throughput requirement like
    when delivering video/sound.
  - to class peer and piece as slow/fast would avoid to have many pieces
    stuck on waiting on slow peers
    - typically observed in wikidbg as many piece almost completed but 
      waiting for a single request on a slow peer
  - maybe with some sort of priority per connection
    - thus it will handle the ecnx priority too (which are important for neoip-oload)
    
- OPTIMAL: all casto got the same pieceq as casti
  - thus they can trade pieces inside the pieceq
  - almost achieved thanks to the piece deletion which delete only the pieces
    which are no more in the pieceq
  - moreover the httpo_full_initial_position() try to put the bt_pieceprec_arr_t
    just at the end of the pieceq.
  - those 2 factors together tend to fill the pieceq
    - this is not true only at the begining of the bt_httpo_full_t when the 
      player has not yet read at lease one pieceq len. 
      cast_mdata().pieceq_maxlen() * bt_mfile.piecelen()

- OPTIMAL VIDEO: all players shows the same image at the same time
  - in cast_swarm_t, this mean that bt_httpo_full_t::current_pos() 
    minus bt_http_full_t::player_buflen() is equal for all bt_httpo_full_t 
    in *all* neoip-casto 
  - In practice, this is hard to achieve because various players read the stream
    differently. e.g. they may read byte by byte to fill their buflen, e.g. 
    they may read by larger chunk such as 32kbyte or other.
  - Another factor is the guessing of the prediction of the player_buflen
    which is quite inaccurate.
  - all player pos equal is food for video syncronization
  - all bt_httpo_full_t::current_pos equal is good for network trading

- OPTIMAL VIDEO: the neoip-cast jitter is 0.
  - the 'neoip-cast jitter' is the difference between the position 
    between a player connected directed to the original stream source and 
    the player connected to the neoip-casto stream source 
  - this is obviously impossible to achieve, because the multicast part of the
    protocol assumes that some piece are buffered by the neoip-cast system
    in order to be traded between the peers.
  - but the lower the better, as it reproduces the effect of live streaming
  - this may be estimated by casti bt_httpi_t::cur_offset() minus
    the player position
  
- OPTIMAL: all bt_httpo_full_t::current_pos() equals for all neoip-casto
  - thus all neoip-casto wishes the sames pieces and allows efficient trading
    - this may be seen as a plain bittorrent cases with all pieces are equally
      wished, but limited to the bt_pieceprec_arr_t at bt_httpo_full_t::current_pos()

- casti pieceq is assumed full while determining httpo_full_initial_pos 
  - it is not full only at the begining of neoip-casti boot 
  - TODO what are the consequence of neoip-casto connecting when casti pieceq
    is not yet full.
    
- hint: player_buflen + slide_curs.nb_piece() < cast_mdata.pieceq_maxlen
  - thus the whole player buflen is inside the pieceq and fstart peer
    download those pieces
  - thus the whole slide_curs.nb_piece is inside the pieceq and all peers
    may trade pieces inside it    

\par Computation of bt_httpo_full_t initial position
- The Computation of bt_httpo_full_t initial position is crucial to control
  the efficiency of the piece trading and to synchronize the images seens 
  by players on all neoip-casto

\par Estimation of bt_htti_t::current_off() in neoip-casto
- A precise estimation of bt_htti_t::current_off() in neoip-casto allows to 
  have a precise idea of the current pieceq in neoip-casti
- issue: neoip-casti has to limit the number of bt_cast_mdata_t sent over the
  network in order to minimize the overhead.
- solution:
  - have a bt_cast_mdata_t::piece_byteoff() which give the byte position within
    the incomplete piece after bt_cast_mdata_t::pieceq_end(). Thus give the 
    position of bt_htti_t at the byte level.



\par About piece deletion
- the goal is to keep all the pieceq which are still in neoip-casti
- this is done via bt_cast_helper_t::remove_piece_outside_pieceq at each
  reception of bt_cast_udata_t

\par Definition of 'zapping latency'
- the 'zapping latency' is the delay between the player connecting casto_swarm_t
  and first image/sound appearing in the player
- the 'zapping latency' is crucial for the user experience.
  - the lower the zapping latency, the bettwe the user experience.
- bt_utmsg_fstart_t is dedicated to help reduce this latency

\par Definition of 'player'
- a player is the external http client connected to casto_swarm_t
- each player implementation may have a different buflen
  - the buflen is the length of the prebuffer done by the player in order
    to absorb the variation of download speed.
  - bt_httpo_full_t::player_buflen() contains a guess of this buffer len
    base on the User-Agent of the http connection
- the player curren tposition may be estimated by bt_httpo_full_t::current_pos() 
  minus bt_httpo_full_t::player_buflen()
- The guessing of the player_buflen is just an hint and should not be relied on. 
  - TODO is the player_buflen() changes with time for a given player/codec
  - TODO is player_buflen() depends on the codecs
  - TODO is player_buflen() depends on the position of the stream
- the data between bt_httpo_full_t::current_pos() and the player position 
  are not *required* to be stored in neoip-casto.
  - but they are kept anyway to potentially help the piece trading with other
    peers

\par About overrun in bt_httpo_full_t
- when a bt_httpo_full_t is not able to deliver data as fast as neoip-casti
  provide them, it may lead to overrun.
- possible reason:
  - neoip-casto not able to receive the neoip-casti data fast enougth
  - player not able to read neoip-casto data fast enougth 
- a bt_httpo_full_t is said in overrun, when bt_httpo_full_t::current_pos()
  is not inside the cast_mdata pieceq
- a bt_httpo_full_t in overrun is deleted immediatly
- checking for overrun is done at every new update of cast_mdata()

\par About bt_utmsg_fstart_t
- this is a full-trust bt_utmsg_vapi_t which declare a peer as being in 'fstart'
- this is simply a bool which is set to true at the begining and false after that
- this value is used by the bt_peerpick_vapi_t to elect the remote peer in fstart
  with more probability than others
- the algo to determine the local_fstart is:
  - if any bt_httpo_full_t has not yet delivered at least its player_buflen()
    then local_fstart is true
  - else it is false
- This is reevaluated at new piece available to bt_httpo_full_t as it may
  change when bt_httpo_full_t deliver pieces. 
- TODO would it be good to have different bt_pieceprec_arr_t depending on 
  the value of local_fstart.
  - currently it is not clear, so it is not coded
  - the advantage is not even clear
  
\par Note to sort about bt_httpo_full_t initial position
- cast_mdata/udata to provide the byte position of bt_httpi_t withing the
  incomplete piece, cast_mdata.piece_byteoff()
- need delta time between now in casto and when cast_mdata was valid
  - time in os time, not in wall clock, to avoid dependency ?
  - cast_mdata/udead boath contains a casti_date of the new in casti
    when it has been originated
  - cast_mdata_client_t is assumed instantaneous
  - a time delta computed in casto_swarm_t::start() to compute the difference
    of ostime between casto and casti
- httpi_rate estimation must to used by neoip-casto to estimate the 
  current bt_httpi_t::cur_offset() in neoip-casti
  - it allow to handle the position as the byte granularity and to decorrelate
    the bt_mfile.piecelen() and the bt_httpo_full_t desync
- resync unstraarted bt_httpo_full_t at each new piece notification from bt_swarm_t
  
\par Possible Improvement - httpo_full_t initial position to the precise frame (with flv)?
- the httpo_full_t initial position is currently done with estimation on the
  httpi_t rate.
  - this is rather crude. 
- with the bt_scasti_mod_flv_t it would be possible to know the exact byte
  position of each frame, and thus using the frame rate (TODO found where ?)
  one could predict the exact frame byteposition on which to start.    
- TODO relation with the key frame and inter frame is unknown
  - is there any advantage is starting on a inter frame ? even in flv
  - the flashplayer may/is likely to simply skip frame until the first
    key frame.
- TODO as this improvement is valid only FLV (at the moment), determine the gain 
  - if any, because it is quite unclear
  - and this modification will cost a lot of code and special case, so the 
    gain *needs* to be clear before doing the work

\par about idletimeout if no bt_cast_udata_t is not received
- this helps the detections of neoip-casti unreachability.
- ALGO:
  - have a bt_cast_mdata_t::recv_udata_maxdelay() which is the maximum delay
    to wait for a bt_cast_udata_t
  - if no bt_cast_udata_t is received during this delay, autodelete casto_swarm_t     

*/

/* system include */
/* local include */
#include "neoip_casto_swarm.hpp"
#include "neoip_casto_swarm_httpo.hpp"
#include "neoip_casto_swarm_udata.hpp"
#include "neoip_casto_apps.hpp"

#include "neoip_bt_cast_helper.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_bt_swarm_utmsg.hpp"

#include "neoip_bt_utmsg_fstart.hpp"
#include "neoip_bt_pselect_slide_curs.hpp"

#include "neoip_bt_httpo_full.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
casto_swarm_t::casto_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_casto_apps	= NULL;
	bt_ezswarm	= NULL;
	m_swarm_httpo	= NULL;
	m_swarm_udata	= NULL;
}

/** \brief Destructor
 */
casto_swarm_t::~casto_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_casto_apps_t
	if( m_casto_apps )	m_casto_apps->swarm_unlink(this);
	// delete the casto_swarm_httpo_t if needed
	nipmem_zdelete	m_swarm_httpo;
	// delete the casto_swarm_udata_t if needed
	nipmem_zdelete	m_swarm_udata;
	// delete the bt_ezswarm_t if needed
	nipmem_zdelete	bt_ezswarm;
}


/** \brief autodelete this object, and return false for tokeep convenience
 */
bool	casto_swarm_t::autodelete(const std::string &reason)			throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// delete httpo_full_db if needed
	if( m_swarm_httpo )	swarm_httpo()->httpo_full_db_dtor(reason);
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
bt_err_t casto_swarm_t::start(casto_apps_t *p_casto_apps, const http_uri_t &m_mdata_srv_uri
				, const std::string &m_cast_name
				, const std::string &m_cast_privhash
				, const bt_cast_mdata_t &p_cast_mdata)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->m_casto_apps	= p_casto_apps;
	this->m_cast_name	= m_cast_name;
	this->m_cast_privhash	= m_cast_privhash;
	this->m_mdata_srv_uri	= m_mdata_srv_uri;
	this->m_cast_mdata	= p_cast_mdata;
	// link this object to the bt_casto_apps_t
	m_casto_apps->swarm_dolink(this);
	
	// init the casto_swarm_httpo_t
	m_swarm_httpo	= nipmem_new casto_swarm_httpo_t();
	bt_err		= m_swarm_httpo->start(this);
	if( bt_err.failed() )	return bt_err;

	// compute the casti_dtime - aka difference between local time and casti time 
	// - NOTE: stored in a int64_t because delay_t doesnt support negative delay
	m_casti_dtime	= date_t::present().to_uint64() - cast_mdata().casti_date().to_uint64();

	// build the bt_mfile_t for this bt_cast_mdata_t
	bt_mfile_t	bt_mfile;
	bt_mfile	= bt_cast_helper_t::build_mfile(cast_mdata());
	if( !bt_mfile.is_fully_init() )
		return bt_err_t(bt_err_t::ERROR, "Cant build the bt_mfile_t for "+ cast_name());

	// start the bt_ezswarm_t
	bt_err		= launch_ezswarm(bt_mfile);
	if( bt_err.failed() )	return bt_err;
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			function related to bt_utmsg_fstart_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the local bt_utmsg_fstart_t value
 * 
 * - NOTE: this wont send any network message if the value is unchanged 
 */
void	casto_swarm_t::local_fstart(bool new_value)			throw()
{
	// get a pointer on bt_utmsg_fstart_t in the current bt_swarm_t
	bt_swarm_t * 		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	bt_swarm_utmsg_t *	swarm_utmsg	= bt_swarm->swarm_utmsg();
	bt_utmsg_vapi_t *	utmsg_vapi	= swarm_utmsg->utmsg_vapi(bt_utmsgtype_t::FSTART);
	bt_utmsg_fstart_t *	utmsg_fstart	= dynamic_cast<bt_utmsg_fstart_t*>(utmsg_vapi);
	DBG_ASSERT( utmsg_fstart );
	// set the new_value 
	// - NOTE: this wont send any network message if the value is unchanged 
	utmsg_fstart->local_fstart(new_value);
}

/** \brief update the local_fstart to the 'proper value'
 * 
 * - the proper value is estimated according to the following algo:
 *   - if any bt_httpo_full_t has delivered less than its player_buflen()
 *     local_fstart is set to true
 *   - else it is set to false
 * - this is consistent with the idea that bt_utmsg_fstart_t is designed
 *   to reduce the initialization latency of the bt_swarm_t
 *   - moreover in the case of neoip-casto, the amount of time to fill the
 *     bt_httpo_full_t::player_buflen() is directly related to the amount
 *     of time between the user tunning in the cast and the time when he/she
 *     start seeing/hearing the cast.
 *   - this delay is called the zapping latency and is CRUCIAL to the user experience
 *     - the lower the zapping latency is, the better is the user experience
 */
void	casto_swarm_t::update_fstart_if_needed()				throw()
{
	const std::list<bt_httpo_full_t *> &		httpo_full_db	= swarm_httpo()->httpo_full_db();
	std::list<bt_httpo_full_t *>::const_iterator	iter;
	bt_swarm_t * 		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();

	// go thru the whole m_httpo_full_db
	for(iter = httpo_full_db.begin(); iter != httpo_full_db.end(); iter++ ){
		const bt_httpo_full_t * httpo_full	= *iter;
		// leave the loop if this bt_httpo_full_t has not yet delivered 
		// at least one time its bt_pselect_slide_curs_t length
		size_t	buflen	= httpo_full->slide_curs()->nb_piece() * bt_mfile.piecelen();
		if( httpo_full->sent_length() < buflen )	break;
	}
	// if any bt_httpo_full_t has delivered less than its bt_pselect_slide_curs_t length
	// set local_fstart is set to true, else to false
	if( iter == httpo_full_db.end() )	local_fstart( false );
	else					local_fstart( true  );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			launch bt_ezswarm_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_ezswarm_t
 */
bt_err_t	casto_swarm_t::launch_ezswarm(const bt_mfile_t &bt_mfile)	throw()
{
	bt_ezsession_t *	bt_ezsession	= m_casto_apps->bt_ezsession();
	bt_swarm_resumedata_t	swarm_resumedata;
	bt_err_t		bt_err;
	// sanity check - the bt_mfile_t MUST contain exactly 1 bt_mfile_subfile_t
	DBG_ASSERT( bt_mfile.subfile_arr().size() == 1 );
	
	// build the bt_swarm_resumedata_t from the resulting bt_mfile_t
	swarm_resumedata	= bt_swarm_resumedata_t::from_mfile(bt_mfile);

	// sanity check - the bt_swarm_resumedata_t MUST check().succeed()
	DBG_ASSERT( swarm_resumedata.check().succeed() );	
	// sanity check - the bt_ezsession_t MUST already bt init
	DBG_ASSERT( bt_ezsession );

	// build the bt_ezsession_profile_t
	bt_ezswarm_profile_t	ezswarm_profile;
	// set the xmit/recv rate_limit_arg_t if the bt_ezsession_t got a xmit_rsched/recv_rsched
	// TODO the rate_prec_t(50) is abitrary and hardcoded - how bad can i be ? :)
	// - should i put this rate_prec_t in a profile ?
	// - LATER: this comment may be obsolete by bt_swarm_full_prec_t
	if( bt_ezsession->xmit_rsched() )	ezswarm_profile.swarm().xmit_limit_arg().rate_sched(bt_ezsession->xmit_rsched()).rate_prec(rate_prec_t(50));
	if( bt_ezsession->recv_rsched() )	ezswarm_profile.swarm().recv_limit_arg().rate_sched(bt_ezsession->recv_rsched()).rate_prec(rate_prec_t(50));
	// copy the kad_peer_t pointer from the bt_ezsession_t to the bt_ezswarm_profile_t
	ezswarm_profile.peersrc_kad_peer(bt_ezsession->kad_peer());
	// copy the io_pfile_dirname for the bt_io_vapi_t
	ezswarm_profile.io_pfile().dest_dirpath		(m_casto_apps->io_pfile_dirpath());
	ezswarm_profile.io_pfile().has_circularidx	(true);
	// set the bt_pselect_policy_t::SLIDE
	ezswarm_profile.swarm().pselect_policy		(bt_pselect_policy_t::SLIDE);
	// set a low bt_swarm_profile_t::itor_blacklist_delay as it is live content
	// - this mean it is normal behaviour to enter a cast, leave it and then come back
	ezswarm_profile.swarm().itor_blacklist_delay	( delay_t::from_sec(60) );
#if 1
	// set bt_swarm_profile_t::itor_jamrc4_type to support ONLY bt_jamrc4_type_t::DOJAM
	ezswarm_profile.swarm().itor_jamrc4_type	( bt_jamrc4_type_t::DOJAM );
#endif
	// set the bt_peerpick_mode_t::CASTO
	ezswarm_profile.peerpick_mode			(bt_peerpick_mode_t::CASTO);
	
	// set the bt_ezswarm_opt_t
	bt_ezswarm_opt_t	ezswarm_opt;
	ezswarm_opt	|= bt_ezswarm_opt_t::IO_PFILE;
	ezswarm_opt	|= bt_ezswarm_opt_t::ECNX_HTTP;
	// NOTE: disable bt_ezswarm_opt_t::PEERSRC_KAD as kad not stable enougth to be usefull 
	//ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_KAD;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_NSLAN;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_UTPEX;
	// if the bt_mfile_t has a announce_uri(), use the bt_peersrc_http_t too
	if( !bt_mfile.announce_uri().is_null())	ezswarm_opt |= bt_ezswarm_opt_t::PEERSRC_HTTP;

	// start a bt_swarm_t depending on the availability of a bt_swarm_resumedata_t
	bt_ezswarm	= nipmem_new bt_ezswarm_t();
	bt_ezswarm->set_profile(ezswarm_profile);
	bt_err		= bt_ezswarm->start(swarm_resumedata, ezswarm_opt, bt_ezsession, this, NULL);
	if( bt_err.failed() )	return bt_err;
	// make bt_ezswarm_t to gracefully goto SHARE
	bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::SHARE);

	// return no error
	return bt_err_t::OK;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_ezswarm_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_ezswarm_t when to notify an event
 */
bool 	casto_swarm_t::neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw()
{
	// log to debug
	KLOG_ERR("enter ezevent=" << ezswarm_event);
	
	// if it is a fatal bt_ezswarm_t, autodelete
	if( bt_ezswarm->in_error() )	return autodelete(bt_ezswarm->cur_state().reason());

	// handle the bt_ezswarm_event_t depending of its type
	switch(ezswarm_event.get_value()){
	case bt_ezswarm_event_t::ENTER_STATE_POST:
			// if entered in bt_ezswarm_state_t::SHARE, do some init
			if( bt_ezswarm->in_share() ){
				bt_err_t bt_err		= bt_ezswarm_enter_share();
				if( bt_err.failed() )	return autodelete();			
			}
			break;
	case bt_ezswarm_event_t::LEAVE_STATE_PRE:
			// if about to leave bt_ezswarm_state_t::SHARE, do some deinit
			if( bt_ezswarm->in_share() )	bt_ezswarm_leave_share();
			break;
	case bt_ezswarm_event_t::SWARM_EVENT:{
			const bt_swarm_event_t *swarm_event = ezswarm_event.get_swarm_event();
			// if a piece_newly_avail, handle it
			if( swarm_event->is_piece_newly_avail() ){
				size_t	pieceidx	= swarm_event->get_piece_newly_avail();
				return handle_piece_newly_avail(pieceidx);
			}
			break;}		
	default:	break;
	}
	// return tokeep
	return true;
}


/** \brief Some initialization to do when bt_ezswarm_t after entered bt_ezswarm_state_t::SHARE
 */
bt_err_t	casto_swarm_t::bt_ezswarm_enter_share()				throw()
{
	// start all bt_httpo_full_t 
	swarm_httpo()->httpo_full_start_all();
	// start the casto_swarm_udata_t
	DBG_ASSERT( !m_swarm_udata );
	bt_err_t	bt_err;
	m_swarm_udata	= nipmem_new casto_swarm_udata_t();
	bt_err		= m_swarm_udata->start(this);
	if( bt_err.failed() )	return bt_err;	

	// pass the bt_utmsg_fstart_t local_fstart to true
	local_fstart(true);

	// return noerror
	return bt_err_t::OK;	
}

/** \brief Some initialization to do when bt_ezswarm_t before leave bt_ezswarm_state_t::SHARE
 */
void	casto_swarm_t::bt_ezswarm_leave_share()				throw()
{
	// delete the casto_swarm_udata_t
	nipmem_delete	m_swarm_udata;

	// TODO should i delete all the bt_httpo_full_t when leaving SHARE ?
	// - btw what about ERROR ?
	// - this is useless if the stopping is not handled
	// - this is related to the termination of this, which is unclear
	// - so this point is never reached... so assert to be sure
	DBG_ASSERT( 0 );
}

/** \brief Handle the bt_swarm_event_t::PIECE_NEWLY_AVAIL
 */
bool	casto_swarm_t::handle_piece_newly_avail(size_t pieceidx)		throw()
{
	// update_fstart if needed
	update_fstart_if_needed();

	// forward the event to casto_swarm_httpo_t
	bool	tokeep	= swarm_httpo()->handle_piece_newly_avail(pieceidx);
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			httpo_full_push
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief push a new bt_httpo_full_t on this casto_swarm_t
 * 
 * - NOTE: the bt_httpo_full_t ownership is transfered to this object
 */
void	casto_swarm_t::httpo_full_push(bt_httpo_full_t *httpo_full)	throw()
{
	// log to debug 
	KLOG_ERR("enter");
	// forward to casto_swarm_httpo_t
	swarm_httpo()->httpo_full_push(httpo_full);
}

NEOIP_NAMESPACE_END;




