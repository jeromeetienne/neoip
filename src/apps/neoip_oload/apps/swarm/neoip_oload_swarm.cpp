/*! \file
    \brief Definition of the \ref oload_swarm_t
    
\par Brief Description
oload_swarm_t hold a bt_ezswarm_t and all the attached bt_httpo_full_t.
there cant be a oload_itor_t and oload_swarm_t for the same inner_uri

\par About supporting multi-file bt_mfile_t
- it is possible to query multi-file bt_mfile_t
  - it requires the nested_uri to specify the subfile within it
  - this is done by a variable in the outter_uri
- http://localhost:4000/ *subfile*1/http/jmehost2/~jerome/video_torrent_try.torrent/IBM_Linux_Commercial.avi
  - this url will read the IBM_Linux_Commercial.avi subfile inside the video_torrent_try.torrent
    which may be a multifile bt_mfile_t
- LIMITATION:
  - if the nested_uri of the bt_httpo_full_t contains a dupuri which has not 
    been initialized during the first bt_httpo_full_t, this dupuri will be
    discarded, and the event logged
  - this is due to the fact it is not allowed to modify the bt_mfile_t DURING
    the life of a bt_swarm_t

\par About the idle_timeout
- when all the attached bt_httpo_full_t are closed, an idle_timeout is started.
  on expiration, it autodelete the oload_swarm_t
  - the idle_timeout delay is tunable in the oload_swarm_profile_t
- this is usefull when the neoip-oload user will likely do several http connections
  close to each in time BUT with some period with no bt_httpo_full_t at all.
  - e.g. when vlc is seeking thru http, it first close the first connection and
    then open the new one. without the idle_timeout, it would close the oload_swarm_t
    and would have to reinit completly for the second connection.

\par About Piece deletion policy
- it doesnt use the bt_httpo_full_arg.piecedel_in_dtor() because it would delete piece
  as soon as a bt_httpo_full_t disconnect.
  - e.g. if neoip-oload deliver a video, and the player is seeking thru it,
    the player will typically close the http connection at the current position
    and open another one 'close' to the first position.
  - as the first and the second position are typically close to each other
    the pieces downloaded for the first may very well overlap on the pieces
    needed for the second.
  - if the pieces for the first, are all deleted when the connection is closed, 
    and if the second overlap with the first, all the pieces will have to be
    redownloaded (just after they got deleted)
  - this would be a waste of rescource
- additionnaly it uses bt_io_pfile_asyncdel_t it order to remove piece periodically
  - the period is completly independant of the rest, so a http client thru 
    bt_httpo_full_t can not predict how long the data will remain cached
    - TODO make it predictable

\par Possible Improvement - error diagnostic
- when there is an error, i simply disconnect the attached bt_httpo_full_t
  instead of returning a http error code when suitable
  - this is bad for the "404 not found" and such
  - this reduce the possibility of diagnostic for the external client

\par Possible Improvement - banking system
- currently, with no banking system, there is little advantage in caching the 
  pieces already delivered by bt_httpo_full_t. aka the ones 'past' the pieceq
  - this is a direct consequence of the sequential nature of the http delivery
  - the remote peer which would be interested into 'past' pieces wont have the
    one the local peer is interested in because it is 'later' in the file.
  - so caching past piece and allowing remote peers to download 'past' 
    pieces which are no more in the pieceq, is very similar to seeding.
    - aka giving away local resources for the community. altruist goal
- it could be changed to a 'egoist goal' thanks to a banking system
  - past piece could be kept because remote peer which would download them
    would pay for the past pieces
  - the money obtained from past pieces may be used to buy pieces within
    the local pieceq from other remote peers.

\par NOTES
- when a bt_httpo_full_t requests on a multi-file bt_mfile_t a subfile which is not
  the last of the bt_mfile_t, the bt_httpo_full_t will download piece which
  are after the reuested subfile
  - this is due to bt_pselect_slide_curs_t which is unaware of the subfile end
  - when the bt_httpo_full_t will deliver pieces close to the end of the subfile,
    bt_pselect_slide_curs_t will still have the original length and so try to 
    select piece which are further of the subfile
  - this is clearly suboptimal as the local peer may download piece which are
    not really needed
  - NOTE: this is not a big problem as the subfile in multi-file bt_mfile_t
    is not very used.
  - NOTE: this may be solvable by 'simply' make bt_pselect_slide_curs_t aware of 
    the subfile end.
- the bt_ezswarm_state_t::STOPPING is not handled.
  - not that it is a good thing, simply it is non trivial to handle and dont know
    the solution for now
  - it implies to have a oload_swarm_t which enter in stopping when it is about 
    to be deleted, and it is deleted when STOPPED is entered
  - what happen if a httpo_full_t occurs for this oload_swarm_t while it is in
    stopping ?
  - POSSIBLE SOLUTIONS:
    - bt_ezswarm_t goes on the stopping ? and when STOPPED is reached, it relaunch
      itself instead of deleteing itself ?
    - oload_apps_t detects this case and ungracefully delete the oload_swarm_t ?
      - this one is quite simple to code

*/

/* system include */
/* local include */
#include "neoip_oload_swarm.hpp"
#include "neoip_oload_apps.hpp"
#include "neoip_oload_helper.hpp"
#include "neoip_oload_mod_vapi.hpp"

// TODO those long list of .hpp in all my files looks bad - try to find a solution
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_bt_swarm_resumedata_helper.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_bt_io_pfile_asyncdel.hpp"

#include "neoip_http_uri.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_http_nested_uri.hpp"

#include "neoip_mimediag.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
oload_swarm_t::oload_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	oload_apps	= NULL;
	bt_ezswarm	= NULL;
	pfile_asyncdel	= NULL;
}

/** \brief Destructor
 */
oload_swarm_t::~oload_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_oload_apps_t
	if( oload_apps )	oload_apps->swarm_unlink(this);
	// delete httpo_full_db if needed
	httpo_full_db_dtor("Service Unavailable");
	// delete the bt_io_pfile_asyncdel_t if needed
	nipmem_zdelete	pfile_asyncdel;
	// delete the bt_ezswarm_t if needed
	nipmem_zdelete	bt_ezswarm;
}


/** \brief autodelete this object, and return false for tokeep convenience
 */
bool	oload_swarm_t::autodelete(const std::string &reason)		throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR("autodelete due to " << reason);
	// delete httpo_full_db if needed
	httpo_full_db_dtor(reason);
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

/** \brief Set the profile for this object
 */
oload_swarm_t &	oload_swarm_t::profile(const oload_swarm_profile_t &m_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( m_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= m_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t oload_swarm_t::start(oload_apps_t *oload_apps, const http_uri_t &nested_uri
						, const bt_mfile_t &bt_mfile)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// sanity check - the nested_uri MUST be valid
	DBG_ASSERT( http_nested_uri_t::is_valid_nested(nested_uri) );
	// copy the parameter
	this->oload_apps	= oload_apps;
	this->m_inner_uri	= oload_helper_t::parse_inner_uri(nested_uri);
	// link this object to the bt_oload_apps_t
	oload_apps->swarm_dolink(this);

	// start the bt_ezswarm_t
	bt_err_t	bt_err;
	bt_err		= launch_ezswarm(bt_mfile, nested_uri);
	if( bt_err.failed() )	return bt_err;
	// return no error
	return bt_err_t::OK;
}

/** \brief Launch the bt_ezswarm_t
 */
bt_err_t	oload_swarm_t::launch_ezswarm(const bt_mfile_t &_bt_mfile
						, const http_uri_t &nested_uri)	throw()
{
	bt_mfile_t		bt_mfile	= _bt_mfile;
	bt_ezsession_t *	bt_ezsession	= oload_apps->bt_ezsession();
	http_uri_t		outter_uri	= oload_helper_t::parse_outter_uri(nested_uri);
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
	// set the xmit/recv rate_limit_arg_t if the bt_ezsession_t got a xmit_rsched/recv_rsched
	// TODO the rate_prec_t(50) is abitrary and hardcoded - how bad can i be ? :)
	// - moreover it is now dynamic from the bt_swarm_full_prec_t
	//   - i dunno how valid this is rate_prec_t in particular
	//   - to give the rate_sched_t pointer is usefull tho as it gives the rate_sched_t
	//     to use
	if( bt_ezsession->xmit_rsched() )	ezswarm_profile.swarm().xmit_limit_arg().rate_sched(bt_ezsession->xmit_rsched()).rate_prec(rate_prec_t(50));
	if( bt_ezsession->recv_rsched() )	ezswarm_profile.swarm().recv_limit_arg().rate_sched(bt_ezsession->recv_rsched()).rate_prec(rate_prec_t(50));
	// copy the kad_peer_t pointer from the bt_ezsession_t to the bt_ezswarm_profile_t
	ezswarm_profile.peersrc_kad_peer		( bt_ezsession->kad_peer()	);
	// copy the io_pfile_dirname for the bt_io_vapi_t
	ezswarm_profile.io_pfile().dest_dirpath		( oload_apps->io_pfile_dirpath());
	// set the bt_pselect_policy_t::SLIDE
	ezswarm_profile.swarm().pselect_policy		( bt_pselect_policy_t::SLIDE 	);
	// set bt_swarm_profile_t::itor_jamrc4_type to support ONLY bt_jamrc4_type_t::DOJAM
	// - this is the default value. may be change by "do_jamrc4=false" outter_var
	std::string	do_jamrc4_str	= outter_uri.var().get_first_value("do_jamrc4", "true");
	bool		do_jamrc4	= string_t::convert_to_bool(do_jamrc4_str);
	ezswarm_profile.swarm().itor_jamrc4_type(do_jamrc4 ? bt_jamrc4_type_t::DOJAM
							: bt_jamrc4_type_t::NOJAM);
	// sanity check - at this point, the ezswarm_profile MUST be check().succeed()
	DBG_ASSERT( ezswarm_profile.check().succeed() );

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
bool 	oload_swarm_t::neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw()
{
	// log to debug
	KLOG_ERR("enter ezevent=" << ezswarm_event);

	// handle the bt_ezswarm_event_t depending of its type
	switch(ezswarm_event.get_value()){
	case bt_ezswarm_event_t::ENTER_STATE_POST:
		// if entered in bt_ezswarm_state_t::SHARE, start all bt_httpo_full_t
		if( bt_ezswarm->in_share() ){
			bt_swarm_t *	bt_swarm	= bt_ezswarm->share()->bt_swarm();
			// start all the bt_httpo_full_t
			httpo_full_start_all();
			// create the bt_io_pfile_asyncdel_t
			bt_err_t	bt_err;
			pfile_asyncdel	= nipmem_new bt_io_pfile_asyncdel_t();
			bt_err		= pfile_asyncdel->start(bt_io_pfile_asyncdel_policy_t::ALL_NOTREQUIRED, bt_swarm);
			if( bt_err.failed() )	return autodelete(bt_err);
		}
		// if it is a fatal bt_ezswarm_t, autodelete
		if( bt_ezswarm->in_error() ){
			KLOG_ERR("Received a fatal event " + bt_ezswarm->cur_state().reason());
			return autodelete(bt_ezswarm->cur_state().reason());
		}
		break;
	case bt_ezswarm_event_t::LEAVE_STATE_PRE:
		// TODO should i delete all the bt_httpo_full_t when leaving SHARE ?
		// - if you do so, think about deleting the io_pfile_asyncdel as well
		// - this is useless if the stopping is not handled
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

/** \brief Handle the bt_swarm_event_t::PIECE_NEWLY_AVAIL
 */
bool	oload_swarm_t::handle_piece_newly_avail(size_t pieceidx)		throw()
{
	std::list<bt_httpo_full_t *>::iterator	iter;
	// notify this new data to each bt_httpo_full_t within the httpo_full_db
	for(iter = m_httpo_full_db.begin(); iter != m_httpo_full_db.end(); iter++){
		bt_httpo_full_t *httpo_full	= *iter;
		// notify this bt_httpo_full_t of the new data
		httpo_full->notify_newly_avail_piece(pieceidx);
	}
	// return tokeep
	return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief push a new bt_httpo_full_t on this oload_swarm_t
 * 
 * - NOTE: the bt_httpo_full_t ownership is transfered to this object
 */
void	oload_swarm_t::httpo_full_push(bt_httpo_full_t *httpo_full)	throw()
{
	// log to debug 
	KLOG_ERR("enter");
	// sanity check - the inner_uri from the http_reqhd_t MUST be equal to the local one
	DBG_ASSERT(oload_helper_t::parse_inner_uri(httpo_full->http_reqhd().uri()) == inner_uri());

	// add the new httpo_full to the database
	m_httpo_full_db.push_back(httpo_full);

	// if the idle_timeout is running, stop it
	if( idle_timeout.is_running() )	idle_timeout.stop();

	// if bt_ezswarm_t is in_share, start it now
	if( bt_ezswarm->in_share() )	httpo_full_start_one(httpo_full);
}

/** \brief start all the bt_httpo_full_t from the m_httpo_full_db
 */
void	oload_swarm_t::httpo_full_start_all()					throw()
{
	// copy of the list, as element may be deleted during walk
	std::list<bt_httpo_full_t *>	db_copy	= m_httpo_full_db;
	std::list<bt_httpo_full_t *>::iterator	iter;
	// got thru the m_httpo_full_db copy
	for(iter = db_copy.begin(); iter != db_copy.end(); iter++){
		bt_httpo_full_t*httpo_full = *iter;
		httpo_full_start_one(httpo_full);
	}
}

/** \brief Start one bt_httpo_full_t
 * 
 * - remove it from the database in case of error
 */
void	oload_swarm_t::httpo_full_start_one(bt_httpo_full_t *httpo_full)	throw()
{
	bt_err_t	bt_err;
	// start the bt_httpo_full_t immediatly - as bt_ezswarm->in_share() 
	// - this is used when new connections occurs during the bt_ezswarm_state_t::SHARE
	bt_err		= httpo_full_do_start(httpo_full);
	if( bt_err.failed() ){
		// log the event
		KLOG_ERR("Failed to start bt_httpo_full_t due to " << bt_err);
		// remove bt_httpo_full_t from the database
		m_httpo_full_db.remove(httpo_full);
		// delete the object itself
		nipmem_zdelete	httpo_full;
	}
}

/** \brief Do the start for this bt_httpo_full_t - it doesnt handle the database 
 */
bt_err_t	oload_swarm_t::httpo_full_do_start(bt_httpo_full_t *httpo_full)	throw()
{
	bt_swarm_t *		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	const http_reqhd_t &	http_reqhd	= httpo_full->http_reqhd();
	file_range_t		req_range	= http_reqhd.range();
	const http_uri_t &	req_uri		= http_reqhd.uri();
	http_uri_t		outter_uri	= oload_helper_t::parse_outter_uri(req_uri);
	size_t			subfile_idx	= oload_helper_t::subfile_idx_by_nested_uri(bt_mfile, req_uri);
	const bt_mfile_subfile_t&mfile_subfile	= bt_mfile.subfile_arr()[subfile_idx];
	oload_mod_vapi_t *	mod_vapi	= oload_helper_t::mod_vapi_from(httpo_full);
	datum_t			prefix_header	= mod_vapi->prefix_header(this, httpo_full);
	file_range_t		rep_range;
	http_rephd_t		http_rephd;
	bt_err_t		bt_err;
	// log to debug 
	KLOG_ERR("enter");
	// sanity check - the bt_ezswarm_t MUST be in_share()
	DBG_ASSERT( bt_ezswarm->in_share() );
	
	// check if this httpo_full_t is compatible with the current bt_mfile
	bt_err	= httpo_full_is_mfile_compatible(httpo_full);
	if( bt_err.failed() )	return	bt_err;

	// test if the nested_uri points to a valid subfile_idx
	if( oload_helper_t::has_subfile_idx_by_nested_uri(bt_mfile, req_uri) == false )
		return bt_err_t(bt_err_t::ERROR, "Unable to find the subfile_idx in the bt_mfile_t"); 

	// handle the case of the mfile_subfile being 0byte long
	if( mfile_subfile.len() == 0 )	return bt_err_t(bt_err_t::ERROR, "queried a mfile_subfile of 0byte long");

	// handle the case differently depending on presence of req_range in the http_reqhd_t
	if( req_range.is_null() ){
		// compute the range to reply
		// - honoring the subfile_byteoffset
		rep_range.beg	( mfile_subfile.totfile_beg()				);
		rep_range.end	( mfile_subfile.totfile_beg() + mfile_subfile.len()-1	);
		// build the reply header
		http_rephd.version(http_version_t::V1_1).status_code(200).reason_phrase("OK");
	}else{
		// sanity check - mfile_subfile MUST be greater than 0
		DBG_ASSERT( mfile_subfile.len() > 0 );
		// clamp the requested range by the size of the mfile_subfile
		req_range.clamped_by( file_range_t(0, mfile_subfile.len()-1) );
		// if the req_range is out of range, delete the httpo_full immediatly
		if(req_range.is_null())	return bt_err_t(bt_err_t::ERROR, "invalid range");
		// compute the range to reply
		rep_range.beg	( mfile_subfile.totfile_beg() + req_range.beg() );
		rep_range.end	( mfile_subfile.totfile_beg() + req_range.end() );
		// sanity check - the rep_range MUST be fully included into the mfile_subfile
		DBG_ASSERT( rep_range.fully_included_in(mfile_subfile.totfile_range()) );
		// sanity check - req_range length MUST be == to rep_range length
		DBG_ASSERT( req_range.len() == rep_range.len() );
		// build the http_rephd_t
		http_rephd.version(http_version_t::V1_1);
		// if there is no outter_var "use_flv_rangereq", return as from http standard
		if( !outter_uri.var().contain_key("use_flv_rangereq") ){
			http_rephd.status_code(206).reason_phrase("Partial Content");
			http_rephd.content_range(req_range, mfile_subfile.len());
		}else{
			// if "use_flv_rangereq" outter_var is present, reply with 200 instead
			// - thus it allow to mimic the behaviour of request without range
			// - aim to workaround flash-player limitation - details in oload_mod_flv_t
			http_rephd.status_code(200).reason_phrase("OK");
		}
	}

	// notify the http client that this server accept RANGE request
	http_rephd.accept_ranges(true);
	// set the Content-Length
	http_rephd.content_length(rep_range.len() + prefix_header.length());
	// set "Connection: close"
	http_rephd.header_db().update("Connection", "close");
	// add the mimetype of the content - rfc2616.14.17
	std::string	mimetype_str	= mimediag_t::from_file_path(mfile_subfile.mfile_path());
	if( !mimetype_str.empty() )	http_rephd.header_db().update("Content-Type", mimetype_str);

#if 0	// TODO experimentation to avoid caching - NOT SURE if this is a good thing 
	// setup "Cache-Control" field to never cache - rfc2616.14.9
	// - "no-cache" - dont cache the data - rfc2616.14.9.1
	// - "no-store" - dont store the data - rfc2616.14.9.2
	http_rephd.header_db().update("Cache-Control", "no-cache; no-store");
#endif

	// if outter var has "httpo_content_attach" to true, set "content-disposition:attachement"
	// - set "Content-Disposition" to specify the filename of the downloaded file
	// - see rfc2616.19.5.1 Content-Disposition
	std::string content_attach_str	= outter_uri.var().get_first_value("httpo_content_attach", "false");
	if( string_t::convert_to_bool(content_attach_str) ){
		std::string	fname	= mfile_subfile.mfile_path().basename().to_string();
		http_rephd.header_db().update("Content-Disposition", "attachment; filename=\"" + fname + "\"");
	}
	// log to debug
	KLOG_ERR("http_rephd=" << http_rephd);


	// get the slide_curs_maxlen from the profile
	size_t	slide_curs_maxlen	= profile().slide_curs_maxlen();
	// overwrite slide_curs_maxlen with "read_ahead" outter_var IF available
	if( outter_uri.var().contain_key("read_ahead") ){
		const std::string & value_str	= outter_uri.var().get_first_value("read_ahead");
		// update slide_curs_maxlen
		slide_curs_maxlen	= string_t::to_uint32(value_str);
		// if slide_curs_maxlen == 0, return an error
		if(slide_curs_maxlen == 0)	return bt_err_t(bt_err_t::ERROR, "invalid outter_var read_ahead. MUST NOT be 0");
	}
	// if slide_curs_maxlen is > than rep_range.len(), clamp it
	// - thus it wont try to download data which have not been requested
	if( rep_range.len().is_size_t_ok() && slide_curs_maxlen > rep_range.len().to_size_t() )
		slide_curs_maxlen	= rep_range.len().to_size_t();
		
	// compute the current idx_offsetfor the slide_curs
	bt_pselect_slide_curs_arg_t	curs_arg;
	curs_arg.offset		(bt_unit_t::totfile_to_pieceidx(rep_range.beg(), bt_mfile));
	curs_arg.has_circularidx(false);
	// set a bt_pieceprec_arr_t
	size_t			nbpiece_ahead	= ceil_div(slide_curs_maxlen, bt_mfile.piecelen());
	DBG_ASSERT( nbpiece_ahead >= 1 && bt_mfile.nb_piece() >= 1 );
	size_t			idx_end		= std::min(nbpiece_ahead-1, bt_mfile.nb_piece()-1);
	bt_pieceprec_arr_t	pieceprec_arr;
	pieceprec_arr.assign(idx_end+1, bt_pieceprec_t::NOTNEEDED);
	pieceprec_arr.set_range_decrease(0, idx_end, 1000);
	curs_arg.pieceprec_arr	(pieceprec_arr);

	// build the bt_httpo_full_profile_t
	bt_httpo_full_profile_t	httpo_full_profile;
	// handle the "httpo_maxrate" outter_var
	if( outter_uri.var().contain_key("httpo_maxrate") ){
		const std::string & value_str	= outter_uri.var().get_first_value("httpo_maxrate");
		size_t		httpo_maxrate	= string_t::to_uint32(value_str);
		// copy the value in bt_httpo_full_profile_t
		httpo_full_profile.xmit_maxrate(httpo_maxrate);
		// handle the "httpo_maxrate_thres" outter_var -- IFF httpo_maxrate is set
		if( outter_uri.var().contain_key("httpo_maxrate_thres") ){
			const std::string & value_str	= outter_uri.var().get_first_value("httpo_maxrate_thres");
			file_size_t	maxrate_thres	= string_t::to_uint64(value_str);
			// copy the value in bt_httpo_full_profile_t
			httpo_full_profile.xmit_maxrate_thres(maxrate_thres);			
		}else{
			// if "httpo_maxrate_thres" is unspecified, default to 10*maxrate
			// - TODO this should not be hardcoded.. 
			// - TODO why is there a default ? why not 0 ?
			// - seems complex
			httpo_full_profile.xmit_maxrate_thres(httpo_maxrate * 10);
		}
	}
	
	
	// build the bt_httpo_full_arg_t
	bt_httpo_full_arg_t	httpo_full_arg;
	httpo_full_arg.bt_swarm		( bt_swarm	);
	httpo_full_arg.http_rephd	( http_rephd	);
	httpo_full_arg.range_tosend	( rep_range	);
	httpo_full_arg.curs_arg		( curs_arg	);
	httpo_full_arg.header_datum	( prefix_header	);
	httpo_full_arg.piecedel_in_dtor	( false		);
	
	// start this bt_httpo_full_t
	bt_err		= httpo_full->profile(httpo_full_profile)
					.start(httpo_full_arg, this, NULL);
	if( bt_err.failed() )	return bt_err;

		
	// return no error
	return bt_err_t::OK;
}

/** \brief Check the compatibility of the new bt_httpo_full_t with the current swarm
 * 
 * - this is related to the outter_var options of the new bt_httpo_full_t which 
 *   may be incompatible with the original bt_httpo_full_t which originated the
 *   creation of this oload_swarm_t
 */
bt_err_t oload_swarm_t::httpo_full_is_mfile_compatible(bt_httpo_full_t *httpo_full)	throw()
{
	const bt_mfile_t &	bt_mfile	= bt_ezswarm->mfile();
	const http_uri_t &	req_nested_uri	= httpo_full->http_reqhd().uri();
// TODO 
// - check the piecelen
// - check the announce_uri
// - see other too


	/*************** Check for dupuri incompatibility	***************/
	// NOTE: there is a 'limitation' with the dupuri processing 
	// - if the outter_uri from the http_reqhd_t contains from dupuri
	// - they may not be inside the current bt_mfile in the bt_swarm
	// - i can not add them ONLY BECAUSE it is not allowed to change the 
	//   bt_mfile_t DURING the file of a bt_swarm.
	// - log this case and discard the incompatible dupuri
	std::list<http_uri_t>	dupuri_db	= oload_helper_t::extract_dupuri_db(req_nested_uri);
	size_t			subfile_idx	= oload_helper_t::subfile_idx_by_nested_uri(bt_mfile, req_nested_uri);
	const bt_mfile_subfile_t &mfile_subfile	= bt_mfile.subfile_arr()[subfile_idx];
	// go thru the whole dupuri_db
	std::list<http_uri_t>::iterator	iter;
	for( iter = dupuri_db.begin(); iter != dupuri_db.end(); iter++){
		const http_uri_t &	dupuri	= *iter;
		// if the dupuri is already contained by the mfile_subfile, goto the next
		if( mfile_subfile.uri_arr().contain(dupuri) )	continue;		
		// if it is not contained, it will be discarded, so log the event
		KLOG_ERR("for nested_uri " << req_nested_uri << ", the dupuri " << dupuri
				<< " WILL NOT be used because it has not been initialized"
				<< " by the first http connection on this swarm."
				<< " This is a limitation of dupuri processing.");
	}

	// return no error
	return bt_err_t::OK;
}

/** \brief Delete the whole http_full_db returning a http error if needed
 */
void	oload_swarm_t::httpo_full_db_dtor(const std::string &reason)	throw()
{
	// delete the httpo_full_db if needed
	while( !m_httpo_full_db.empty() ){
		bt_httpo_full_t * httpo_full	= m_httpo_full_db.front();
		// remote it from the m_httpo_full_db 
		m_httpo_full_db.pop_front();
		// reply the http error 503 "Service Unavailable" - rfc2616.10.5.4
		// - IIF httpo_full is not started, because the error is in the header
		if( !httpo_full->is_started() ){
			oload_helper_t::reply_err_httpo_full(httpo_full
					, http_status_t::SERVICE_UNAVAILABLE, reason);
		}
		// delete the object itself
		nipmem_zdelete httpo_full;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bt_httpo_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool	oload_swarm_t::neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
					, const bt_httpo_event_t &httpo_event)	throw()
{
	bt_httpo_full_t*httpo_full	= &cb_bt_httpo_full;
	// log to debug
	KLOG_ERR("enter httpo_event=" << httpo_event);
	// sanity check - bt_httpo_event_t MUST be is_full_ok()
	DBG_ASSERT( httpo_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( httpo_event.get_value() ){
	case bt_httpo_event_t::CNX_CLOSED:
			// remove it from the httpo_full_db
			m_httpo_full_db.remove(httpo_full);
			// delete the bt_httpo_full_t object itself
			nipmem_zdelete httpo_full;
			// if httpo_full_db is now empty, start the idle_timeout
			if( m_httpo_full_db.empty() ){
				DBG_ASSERT( !idle_timeout.is_running() );
				idle_timeout.start(profile().idle_timeout(), this, NULL );
			}
			// return dontkeep - as the httpo_full has just been deleted
			return false;
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	oload_swarm_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// sanity check - m_httpo_full_db MUST be empty 
	DBG_ASSERT( m_httpo_full_db.empty() );
	// when the idle_timeout expires, autodelete the oload_swarm_t
	return autodelete("Timed out after " + idle_timeout.get_period().to_string() );
}

NEOIP_NAMESPACE_END;




