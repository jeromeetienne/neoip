/*! \file
    \brief Definition of the \ref casti_swarm_t

\par Brief Description
casti_swarm_t hold a bt_ezswarm_t and the bt_scasti_vapi_t to import an http
stream into bt_swarm_t
- the bt_scasti_vapi_t stuff is handled in casti_swarm_scasti_t
- the starpos stuff is handled in casti_swarm_spos_t
- the bt_cast_udata_t xmit stuff is handled in casti_swarm_udata_t

- TODO to refactor the start() function to use a arg list
  - this is a real mess

\par NOTES
- the bt_ezswarm_state_t::STOPPING is not handled.
  - not that it is a good thing, simply it is non trivial to handle and dont know
    the solution for now
  - it implies to have a casti_swarm_t which enter in stopping when it is about
    to be deleted, and it is deleted when STOPPED is entered
  - what happen if a httpo_full_t occurs for this casti_swarm_t while it is in
    stopping ?
  - POSSIBLE SOLUTIONS:
    - bt_ezswarm_t goes on the stopping ? and when STOPPED is reached, it relaunch
      itself instead of deleteing itself ?
    - casti_apps_t detects this case and ungracefully delete the casti_swarm_t ?
      - this one is quite simple to code

*/

/* system include */
/* local include */
#include "neoip_casti_swarm.hpp"
#include "neoip_casti_swarm_arg.hpp"
#include "neoip_casti_apps.hpp"
#include "neoip_casti_inetreach_httpd.hpp"

#include "neoip_casti_swarm_udata.hpp"
#include "neoip_casti_swarm_spos.hpp"
#include "neoip_casti_swarm_scasti.hpp"

#include "neoip_bt_cast_id.hpp"
#include "neoip_bt_cast_helper.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_cast_udata.hpp"
#include "neoip_bt_cast_mdata_dopublish.hpp"
#include "neoip_bt_cast_mdata_unpublish.hpp"
#include "neoip_bt_cast_mdata_server.hpp"

#include "neoip_xmlrpc_listener.hpp"

#include "neoip_bt_scasti_vapi.hpp"
#include "neoip_bt_scasti_mod_vapi.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_event.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of \ref casti_swarm_t constant
const std::string	casti_swarm_t::UTMSG_BCAST_KEY	= "casti";

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
casti_swarm_t::casti_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_bt_ezswarm	= NULL;
	m_swarm_udata	= NULL;
	m_swarm_spos	= NULL;
	m_swarm_scasti	= NULL;
	m_mdata_dopublish=NULL;
	m_mdata_unpublish=NULL;
	pieceq_beg	= 0;
	pieceq_end	= 0;

	// compute the mdata_nonce - MUST be different than 0
	// - it is statically unique for this run of the casti_swarm_t
	// - it is used by casto_swarm_t to detect 'relaunch' from casti_swarm_t
	mdata_nonce	= 1 + (rand() % 255);	// MUST be different than 0
}

/** \brief Destructor
 */
casti_swarm_t::~casti_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_casti_apps_t
	if( casti_apps )	casti_apps->swarm_unlink(this);
	// delete casti_swarm_udata_t if needed
	nipmem_zdelete	m_swarm_udata;
	// delete casti_swarm_spos_t if needed
	nipmem_zdelete	m_swarm_spos;
	// delete casti_swarm_scasti_t if needed
	nipmem_zdelete	m_swarm_scasti;
	// delete bt_cast_mdata_dopublish_t if needed
	nipmem_zdelete	m_mdata_dopublish;
	// delete bt_cast_mdata_unpublish_t if needed
	nipmem_zdelete	m_mdata_unpublish;
	// delete the bt_ezswarm_t if needed
	nipmem_zdelete	m_bt_ezswarm;
}

/** \brief autodelete this object, and return false for tokeep convenience
 */
bool	casti_swarm_t::autodelete(const std::string &reason)			throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
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
casti_swarm_t &	casti_swarm_t::profile(const casti_swarm_profile_t &m_profile)	throw()
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
bt_err_t casti_swarm_t::start(casti_swarm_arg_t &swarm_arg)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// sanity check - swarm_arg MUST be check().succeed()
	DBG_ASSERT( swarm_arg.check().succeed() );
	// sanity check - casti_swarm_t MUST NOT exist for this cast_name
	DBG_ASSERT( swarm_arg.casti_apps()->swarm_by(swarm_arg.mdata_srv_uri(), swarm_arg.cast_name(), swarm_arg.cast_privtext()) == NULL);

	// copy the parameter
	this->casti_apps	= swarm_arg.casti_apps();
	this->m_mdata_srv_uri	= swarm_arg.mdata_srv_uri();
	this->m_cast_name	= swarm_arg.cast_name();
	this->m_cast_privtext	= swarm_arg.cast_privtext();
	this->m_scasti_uri	= swarm_arg.scasti_uri();
	this->m_scasti_mod	= swarm_arg.scasti_mod();
	this->m_http_peersrc_uri= swarm_arg.http_peersrc_uri();
	this->m_web2srv_str	= swarm_arg.web2srv_str();
	// link this object to the bt_casti_apps_t
	casti_apps->swarm_dolink(this);

	// get a random bt_cast_id_t
	this->m_cast_id		= bt_cast_id_t::build_random();

	// build the bt_mfile_t
	bt_mfile_t	bt_mfile;
	bt_mfile	= bt_cast_helper_t::build_mfile(cast_id(), m_cast_name, http_peersrc_uri());
	if( !bt_mfile.is_fully_init() ){
		std::string reason = "Cant build the bt_mfile_t for "+ scasti_uri().to_string();
		return bt_err_t(bt_err_t::ERROR, reason);
	}

	// start the bt_ezswarm_t
	bt_err_t	bt_err;
	bt_err		= launch_ezswarm(bt_mfile);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the bt_scasti_vapi_t of casti_swarm_t
 */
bt_scasti_vapi_t *	casti_swarm_t::scasti_vapi()	const throw()
{
	DBG_ASSERT( m_swarm_scasti );
	return swarm_scasti()->scasti_vapi();
}

/** \brief Return the bt_ezswarm_state
 */
bt_ezswarm_state_t	casti_swarm_t::state()	const throw()
{
	// if bt_ezswarm() is_stopped but m_mdata_unpublish is not completed, return STOPPING
	// - special case when doing a casti_swarm_t::gracefull_shutdown()
	if( bt_ezswarm()->cur_state().is_stopped() && m_mdata_unpublish )
		return bt_ezswarm_state_t::STOPPING;
	// return the bt_ezswarm()->cur_state() directly
	return bt_ezswarm()->cur_state();
}

/** \brief Return true IIF casti_swarm_t bt_cast_mdata_t has been published
 */
bool	casti_swarm_t::is_published()	const throw()
{
	// if m_data_dopublish doesnt exist, then bt_cast_mdata_t is not published
	if(!m_mdata_dopublish)	return false;
	// return the published state of bt_cast_mdata_dopublish_t
	return m_mdata_dopublish->published();
}

/** \brief Return cast_privhash IIF casti_swarm_t bt_cast_mdata_t has been published
 */
std::string	casti_swarm_t::cast_privhash()	const throw()
{
	// if m_data_dopublish doesnt exist, then return an empty string
	if(!m_mdata_dopublish)			return std::string();
	// if m_mdata_dopublish->published() is failed, then return an empty string
	if(!m_mdata_dopublish->published() )	return std::string();
	// return cast_privhash of bt_cast_mdata_dopublish_t
	return m_mdata_dopublish->cast_privhash();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief update web2srv_str
 *
 * - if the value changes, a bt_cast_mdata_t publication is immediatly triggered
*/
void	casti_swarm_t::web2srv_str(const std::string &new_value)		throw()
{
	// if the value stays the same, return now
	if( m_web2srv_str == new_value )	return;
	// update the value
	m_web2srv_str	= new_value;
	// if m_data_dopublish is not init, return now
	if( !m_mdata_dopublish )		return;
	m_mdata_dopublish->web2srv_str(m_web2srv_str);
}

/** \brief do a gracefull_shutdown of this casti_swarm_t
 *
 * @return tokeep for the casti_swarm_t
 */
bool	casti_swarm_t::gracefull_shutdown(const std::string &reason)		throw()
{
	// if bt_ezswarm() is not in_share(), autodelete right now
	if( !bt_ezswarm()->in_share() )	return autodelete(reason);
	// graceful_change_state bt_ezswarm_state_t::STOPPED
	bt_ezswarm_state_t	new_state(bt_ezswarm_state_t::STOPPED, reason);
	bt_ezswarm()->graceful_change_state(new_state);
	// return tokeep
	return true;
}

/** \brief notify the casti_swarm_t that it may start publish bt_cast_mdata_t
 */
void	casti_swarm_t::start_publishing()		throw()
{
	// sanity check - MUST NOT be called if ::is_published() is true
	DBG_ASSERT( !is_published() );
	// sanity check - bt_ezswarm_t MUST be bt_ezswarm_state_t::STOPPED
	DBG_ASSERT( bt_ezswarm()->in_share() );

	// if m_data_dopublish is already running, return now
	if( m_mdata_dopublish )	return;

	// start the bt_cast_mdata_dopublish_t
	bt_err_t	bt_err;
	m_mdata_dopublish= nipmem_new bt_cast_mdata_dopublish_t();
	bt_err		= m_mdata_dopublish->start(mdata_srv_uri(), cast_privtext()
						, m_web2srv_str, this, NULL);
	DBG_ASSERT( bt_err.succeed() );
}

/** \brief notify the casti_swarm_t that republishing is required
 *
 * - this is needed because the bt_cast_mdata_dopublish_t may be in "pull" mode
 *   and so may relies on the network position of neoip-casti
 *   - this function is notified when the network position is changed
 */
void	casti_swarm_t::notify_republish_required()	throw()
{
	// if it is not published, do nothing
	if( !m_mdata_dopublish )	return;

	// stop the current m_mdata_dopublish
	nipmem_zdelete	m_mdata_dopublish;

	// start the bt_cast_mdata_dopublish_t
	bt_err_t	bt_err;
	m_mdata_dopublish= nipmem_new bt_cast_mdata_dopublish_t();
	bt_err		= m_mdata_dopublish->start(mdata_srv_uri(), cast_privtext()
						, m_web2srv_str, this, NULL);
	DBG_ASSERT( bt_err.succeed() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Building of current bt_cast_mdata_t/bt_cast_udata_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a bt_cast_mdata_t for this casti_swarm_t
 *
 * - NOTE: it may return a bt_cast_mdata_t
 */
bt_cast_mdata_t		casti_swarm_t::current_mdata()	const throw()
{
	bt_cast_mdata_t	cast_mdata;
	// sanity check - bt_ezswarm MUST be non null and in_share
	DBG_ASSERT( bt_ezswarm() );
	DBG_ASSERT( bt_ezswarm()->in_share() );
	// some variable alias to ease readability
	const bt_cast_spos_arr_t &cast_spos_arr	= swarm_spos()->cast_spos_arr();
	bt_scasti_mod_vapi_t * mod_vapi	= scasti_vapi()->mod_vapi();
	bt_swarm_t *	bt_swarm	= bt_ezswarm()->share()->bt_swarm();
	const bt_mfile_t &bt_mfile	= bt_swarm->get_mfile();
	// populate the bt_cast_mdata_t
	cast_mdata.cast_id		( cast_id()			);
	cast_mdata.cast_name		( cast_name()			);
	cast_mdata.http_peersrc_uri	( http_peersrc_uri()		);
	cast_mdata.pieceq_beg		( pieceq_beg			);
	cast_mdata.pieceq_end		( pieceq_end			);
	cast_mdata.pieceq_maxlen	( profile().pieceq_maxlen()	);
	cast_mdata.boot_nonce		( mdata_nonce			);
	cast_mdata.casti_date		( date_t::present()		);
	cast_mdata.recv_udata_maxdelay	( swarm_udata()->profile().recv_udata_maxdelay() );
	cast_mdata.prefix_header	( mod_vapi->prefix_header()	);
	cast_mdata.cast_spos_arr	( cast_spos_arr.within_pieceq(pieceq_beg, pieceq_end, bt_mfile)	);
	// TODO this value should not be hardcorded
	cast_mdata.slide_curs_nbpiece_hint( 20				);

	// return the just built bt_cast_mdata_t
	return	cast_mdata;
}


/** \brief Return a bt_cast_udata_t for this casti_swarm_t
 */
bt_cast_udata_t		casti_swarm_t::current_udata()				const throw()
{
	bt_cast_udata_t	cast_udata;
	// sanity check - bt_ezswarm MUST be non null and in_share
	DBG_ASSERT( bt_ezswarm() );
	DBG_ASSERT( bt_ezswarm()->in_share() );
	// some variable alias to ease readability
	bt_swarm_t *	bt_swarm	= bt_ezswarm()->share()->bt_swarm();
	const bt_mfile_t &bt_mfile	= bt_swarm->get_mfile();
	// populate the bt_cast_udata_t
	cast_udata.pieceq_beg	( pieceq_beg		);
	cast_udata.pieceq_end	( pieceq_end		);
	cast_udata.boot_nonce	( mdata_nonce		);
	cast_udata.casti_date	( date_t::present()	);
	// populate the cast_udata.cast_spos_arr
	// - only if pieceq_end has changed since the last bt_cast_udata_t xmit
	if( swarm_udata()->last_pieceq_end() != pieceq_end ){
		const bt_cast_spos_arr_t &cast_spos_arr	= swarm_spos()->cast_spos_arr();
		size_t	range_beg	= (swarm_udata()->last_pieceq_end()+1) % bt_mfile.nb_piece();
		size_t	range_end	= pieceq_end;
		cast_udata.cast_spos_arr( cast_spos_arr.within_pieceq(range_beg, range_end, bt_mfile)	);
	}

	// sanity check - the generated bt_cast_udata_t MUST NOT be null
	DBG_ASSERT( !cast_udata.is_null() );
	// return the just built bt_cast_udata_t
	return	cast_udata;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			launch bt_ezswarm_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_ezswarm_t
 */
bt_err_t	casti_swarm_t::launch_ezswarm(const bt_mfile_t &bt_mfile)	throw()
{
	bt_ezsession_t *	bt_ezsession	= casti_apps->bt_ezsession();
	bt_err_t		bt_err;

	// build the bt_swarm_resumedata_t from the resulting bt_mfile_t
	bt_swarm_resumedata_t	swarm_resumedata;
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
	if( bt_ezsession->xmit_rsched() )	ezswarm_profile.swarm().xmit_limit_arg().rate_sched(bt_ezsession->xmit_rsched()).rate_prec(rate_prec_t(50));
	if( bt_ezsession->recv_rsched() )	ezswarm_profile.swarm().recv_limit_arg().rate_sched(bt_ezsession->recv_rsched()).rate_prec(rate_prec_t(50));
	// copy the kad_peer_t pointer from the bt_ezsession_t to the bt_ezswarm_profile_t
	ezswarm_profile.peersrc_kad_peer		(bt_ezsession->kad_peer());
	// copy the io_pfile_dirpath for the bt_io_vapi_t
	ezswarm_profile.io_pfile().dest_dirpath		(casti_apps->io_pfile_dirpath());
	ezswarm_profile.io_pfile().has_circularidx	(true);
	// set the bt_pselect_policy_t::SLIDE
	ezswarm_profile.swarm().pselect_policy		(bt_pselect_policy_t::SLIDE );
	// set a low bt_swarm_profile_t::itor_blacklist_delay as it is live content
	// - this mean it is normal behaviour to enter a cast, leave it and then come back
	ezswarm_profile.swarm().itor_blacklist_delay	( delay_t::from_sec(60) );
#if 1
	// set bt_swarm_profile_t::itor_jamrc4_type to support ONLY bt_jamrc4_type_t::DOJAM
	ezswarm_profile.swarm().itor_jamrc4_type	( bt_jamrc4_type_t::DOJAM );
#endif
	// set the bt_peerpick_mode_t::CASTI
	ezswarm_profile.peerpick_mode			(bt_peerpick_mode_t::CASTI);
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
	m_bt_ezswarm	= nipmem_new bt_ezswarm_t();
	bt_err		= m_bt_ezswarm->set_profile(ezswarm_profile)
				.start(swarm_resumedata, ezswarm_opt, bt_ezsession, this, NULL);
	if( bt_err.failed() )	return bt_err;
	// make bt_ezswarm_t to gracefully goto SHARE
	bt_ezswarm()->graceful_change_state(bt_ezswarm_state_t::SHARE);

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
bool 	casti_swarm_t::neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw()
{
	// log to debug
	KLOG_ERR("enter ezevent=" << ezswarm_event);

	// if it is a fatal bt_ezswarm_t, notify a error
	if( bt_ezswarm()->cur_state().is_error() )
		return autodelete("bt_ezswarm report error " + bt_ezswarm()->cur_state().reason());

	// handle the bt_ezswarm_event_t depending of its type
	switch(ezswarm_event.get_value()){
	case bt_ezswarm_event_t::ENTER_STATE_POST:
			// if entered in bt_ezswarm_state_t::SHARE, start bt_scasti_vapi_t and xmit_udata
			if( bt_ezswarm()->in_share() ){
				bt_err_t bt_err	= bt_ezswarm_enter_share();
				if( bt_err.failed() )	return autodelete(bt_err);
			}
			// if entered in bt_ezswarm_state_t::STOPPING, start bt_cast_mdata_unpublish_t
			if( bt_ezswarm()->in_stopping() ){
				// sanity check - at this point, m_mdata_unpublish MUST be NULL
				DBG_ASSERT(m_mdata_unpublish == NULL);
				// start the bt_cast_mdata_unpublish_t
				m_mdata_unpublish	= nipmem_new bt_cast_mdata_unpublish_t();
				bt_err_t bt_err		= m_mdata_unpublish->start(mdata_srv_uri()
								, cast_name(), cast_privtext()
								, this, NULL);
				if( bt_err.failed() )	return autodelete(bt_err);
			}
			// if entered in STOPPED and unpublish done, now autodelete
			if( bt_ezswarm()->in_stopped() && !m_mdata_unpublish ){
				return autodelete(bt_ezswarm()->cur_state().reason());
			}
			break;
	case bt_ezswarm_event_t::LEAVE_STATE_PRE:
			// if about to leave bt_ezswarm_state_t::SHARE, stop the bt_scasti_vapi_t
			if( bt_ezswarm()->in_share() )	bt_ezswarm_leave_share();
			break;
	default:	break;
	}
	// return tokeep
	return true;
}

/** \brief Some initialization to do when bt_ezswarm_t after entered bt_ezswarm_state_t::SHARE
 */
bt_err_t	casti_swarm_t::bt_ezswarm_enter_share()				throw()
{
	bt_err_t	bt_err;

	// start the casti_swarm_scasti_t
	DBG_ASSERT( !m_swarm_scasti );
	m_swarm_scasti	= nipmem_new casti_swarm_scasti_t();
	bt_err		= m_swarm_scasti->start(this);
	if( bt_err.failed() )	return bt_err;

	// start the casti_swarm_udata_t
	DBG_ASSERT( !m_swarm_udata );
	m_swarm_udata	= nipmem_new casti_swarm_udata_t();
	bt_err		= m_swarm_udata->start(this);
	if( bt_err.failed() )	return bt_err;

	// start the casti_swarm_spos_t
	DBG_ASSERT( !m_swarm_spos );
	m_swarm_spos	= nipmem_new casti_swarm_spos_t();
	bt_err		= m_swarm_spos->start(this);
	if( bt_err.failed() )	return bt_err;

	// NOTE: bt_cast_mdata_dopublish_t is started later. when some bt_cast_spos_t are avail

	// return noerror
	return bt_err_t::OK;
}

/** \brief Some initialization to do when bt_ezswarm_t before leave bt_ezswarm_state_t::SHARE
 */
void	casti_swarm_t::bt_ezswarm_leave_share()				throw()
{
	// delete the casti_swarm_spos_t
	nipmem_zdelete	m_swarm_spos;
	// delete the casti_swarm_udata_t
	nipmem_zdelete	m_swarm_udata;
	// delete the casti_swarm_scasti_t
	nipmem_zdelete	m_swarm_scasti;
	// delete the bt_cast_mdata_dopublish_t
	nipmem_zdelete	m_mdata_dopublish;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_cast_mdata_dopublish_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_cast_mdata_dopublish_t to provide event
 */
bool	casti_swarm_t::neoip_bt_cast_mdata_dopublish_cb(void *cb_userptr
					, bt_cast_mdata_dopublish_t &cb_mdata_dopublish
					, bt_cast_mdata_t *cast_mdata_out
					, uint16_t *port_lview_out, uint16_t *port_pview_out
					, std::string *casti_uri_pathquery_out)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// copy the casti_swarm_t bt_cast_mdata_t
	if( cast_mdata_out )	*cast_mdata_out	= current_mdata();

	// get the port_lview_out/port_pview_out from the casti_inetreach_httpd_t
	casti_inetreach_httpd_t*inetreach_httpd	= casti_apps->inetreach_httpd();
	if( port_lview_out )	*port_lview_out	= inetreach_httpd->listen_ipport_lview().port();
	if( port_pview_out )	*port_pview_out	= inetreach_httpd->listen_ipport_pview().port();

	// get the casti_uri_pathquery_out from the bt_cast_mdata_server_t
	// - NOTE: a lot of alias to get the casti_uri_pathquery
	bt_cast_mdata_server_t*	mdata_server	= casti_apps->mdata_server();
	xmlrpc_listener_t *	xmlrpc_listener	= mdata_server->xmlrpc_listener();
	const http_uri_t &	xmlrpc_uri	= xmlrpc_listener->xmlrpc_uri();
	if( casti_uri_pathquery_out )	*casti_uri_pathquery_out	= xmlrpc_uri.pathquery_str();

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_cast_mdata_unpublish_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_cast_mdata_unpublish_t
 */
bool	casti_swarm_t::neoip_bt_cast_mdata_unpublish_cb(void *cb_userptr
					, bt_cast_mdata_unpublish_t &cb_swarm_unpublish
					, const bt_err_t &bt_err)	throw()
{
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err);

	// delete the m_mdata_unpublish
	nipmem_zdelete	m_mdata_unpublish;

	// if bt_ezswarm() is already stopped, autodelete now
	if(bt_ezswarm()->in_stopped())	return autodelete(bt_ezswarm()->cur_state().reason());

	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END;




