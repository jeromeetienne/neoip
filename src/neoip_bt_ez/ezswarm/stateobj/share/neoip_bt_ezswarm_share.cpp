/*! \file
    \brief Class to handle the bt_ezswarm_share_t
    
\par Brief Description
\ref bt_ezswarm_share_t handle the bt_ezswarm_state_t::SHARE for bt_ezswarm_t

*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_sched_full.hpp"
#include "neoip_bt_swarm_peersrc.hpp"
#include "neoip_bt_swarm_utmsg.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_io_sfile.hpp"
#include "neoip_bt_peerpick_mode.hpp"
#include "neoip_bt_peerpick_helper.hpp"
#include "neoip_bt_peersrc_http.hpp"
#include "neoip_bt_peersrc_kad.hpp"
#include "neoip_bt_peersrc_nslan.hpp"
#include "neoip_bt_peersrc_utpex.hpp"
#include "neoip_bt_utmsg_byteacct.hpp"
#include "neoip_bt_utmsg_bcast.hpp"
#include "neoip_bt_utmsg_fstart.hpp"
#include "neoip_bt_utmsg_piecewish.hpp"
#include "neoip_bt_utmsg_punavail.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_kad.hpp"
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
bt_ezswarm_share_t::bt_ezswarm_share_t()		throw()
{
	// zero some field
	bt_ezswarm	= NULL;
	// zero the satelite class pointers
	m_http_ecnx_pool= NULL;
	m_peerpick_vapi	= NULL;
	m_peersrc_http	= NULL;
	m_peersrc_kad	= NULL;
	m_peersrc_nslan	= NULL;
	m_peersrc_utpex	= NULL;
	m_utmsg_byteacct= NULL;
	m_utmsg_bcast	= NULL;
	m_utmsg_fstart	= NULL;
	m_utmsg_piecewish= NULL;
	m_utmsg_punavail= NULL;
	// zero the bt_swarm_t
	m_bt_swarm	= NULL;
}

/** \brief Destructor
 */
bt_ezswarm_share_t::~bt_ezswarm_share_t()		throw()
{
	// delete the bt_http_ecnx_pool_t if needed
	nipmem_zdelete	m_http_ecnx_pool;
	// delete the bt_peersrc_http_t if needed
	nipmem_zdelete	m_peersrc_http;
	// delete the peersrc_kad if needed
	nipmem_zdelete	m_peersrc_kad;
	// delete the peersrc_nslan if needed
	nipmem_zdelete	m_peersrc_nslan;
	// delete the bt_peersrc_utpex_t if needed
	nipmem_zdelete	m_peersrc_utpex;
	// delete the bt_utmsg_byteacct_t if needed
	nipmem_zdelete	m_utmsg_byteacct;
	// delete the bt_utmsg_bcast_t if needed
	nipmem_zdelete	m_utmsg_bcast;
	// delete the bt_utmsg_fstart_t if needed
	nipmem_zdelete	m_utmsg_fstart;
	// delete the bt_utmsg_piecewish_t if needed
	nipmem_zdelete	m_utmsg_piecewish;
	// delete the bt_utmsg_punavail_t if needed
	nipmem_zdelete	m_utmsg_punavail;
	// delete the bt_swarm_t if needed
	nipmem_zdelete	m_bt_swarm;
	// delete the bt_peerpick_vapi_t if needed
	nipmem_zdelete	m_peerpick_vapi;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_ezswarm_share_t::start(bt_ezswarm_t *bt_ezswarm)		throw()
{
	bt_session_t *		bt_session	= bt_ezswarm->ezsession()->bt_session();
	const bt_mfile_t &	bt_mfile	= bt_ezswarm->mfile();
	const bt_ezswarm_profile_t &profile	= bt_ezswarm->profile();
	bt_err_t		bt_err;
	// sanity check - the bt_mfile_t MUST be fully init
	DBG_ASSERT( bt_mfile.is_fully_init() );
	// copy the parameter
	this->bt_ezswarm= bt_ezswarm;

	// start a bt_swarm_t depending on the availability of a bt_swarm_resumedata_t
	m_bt_swarm	= nipmem_new bt_swarm_t();
	m_bt_swarm->set_profile(profile.swarm());
	// if ezswarm has a resumedata, start bt_swarm from it else start from bt_mfile_t
	if( bt_ezswarm->swarm_resumedata().is_null() == false ){
		bt_err	= m_bt_swarm->start(bt_session, bt_ezswarm->swarm_resumedata()
						, bt_ezswarm->io_vapi(), this, NULL);
	}else{
		// TODO to remote - the start() with bt_mfile_t is obsolete
		bt_err	= m_bt_swarm->start(bt_session, bt_mfile, bt_ezswarm->io_vapi()
						, this, NULL);
	}
	if( bt_err.failed() )	return bt_err;

	// build the bt_peerpick_vapi_t
	m_peerpick_vapi	= bt_peerpick_helper_t::vapi_ctor(profile.peerpick_mode(), bt_swarm()
					, profile.peerpick(), bt_err);
	if( bt_err.failed() )	return bt_err;
	// update the bt_swarm_sched_full_t with this bt_peerpick_vapi_t
	// - TODO im not happy with the way it is updated
	// - it seems to go thru the whole stuff, kinda kludgy
	bt_swarm()->swarm_sched()->sched_full()->peerpick_vapi(peerpick_vapi());	
	
	// init the bt_http_ecnx_pool_t if configured
	if( bt_ezswarm->opt().is_ecnx_http() ){
		m_http_ecnx_pool= nipmem_new bt_http_ecnx_pool_t();
		bt_err		= http_ecnx_pool()->start(bt_swarm());
		if( bt_err.failed() )		return bt_err;
	}
	
	// init the bt_peersrc_http_t if configured
	if( bt_ezswarm->opt().is_peersrc_http() ){
		m_peersrc_http	= nipmem_new bt_peersrc_http_t();
		bt_err		= peersrc_http()->start(bt_swarm(), bt_swarm()->swarm_peersrc(), NULL);
		if( bt_err.failed() )	return bt_err;
	}

	// init the bt_peersrc_kad_t if configured
	if( bt_ezswarm->opt().is_peersrc_kad() && !bt_mfile.no_external_peersrc() ){
		DBG_ASSERT(profile.peersrc_kad_peer());
		m_peersrc_kad	= nipmem_new bt_peersrc_kad_t();
		bt_err		= peersrc_kad()->start(bt_swarm(), profile.peersrc_kad_peer()
							, bt_swarm()->swarm_peersrc(), NULL);
		if( bt_err.failed() )	return bt_err;
	}

	// init the bt_peersrc_nslan_t if configured
	if( bt_ezswarm->opt().is_peersrc_nslan() && !bt_mfile.no_external_peersrc() ){
		nslan_peer_t *	nslan_peer = bt_ezswarm->ezsession()->nslan_peer();
		// get the nslan_peer_t from the bt_ezsession_t
		m_peersrc_nslan	= nipmem_new bt_peersrc_nslan_t();
		bt_err		= peersrc_nslan()->start(bt_swarm(), nslan_peer, bt_swarm()->swarm_peersrc(), NULL);
		if( bt_err.failed() )	return bt_err;
	}

	// init the bt_peersrc_utpex_t if configured
	if( bt_ezswarm->opt().is_peersrc_utpex() && !bt_mfile.no_external_peersrc() ){
		m_peersrc_utpex	= nipmem_new bt_peersrc_utpex_t();
		bt_err		= peersrc_utpex()->start(bt_swarm(), bt_swarm()->swarm_peersrc()
							, bt_swarm()->swarm_utmsg(), NULL);
		if( bt_err.failed() )	return bt_err;
	}

// TODO make all those utmsg extension tunable
// - like opt.is_utmsg_byteacct with true as default
// - currently impossible to set default value in opt
// - well do the modification, it is not a bad thing to be able to put default value

	// start the bt_utmsg_byteacct_t extension
	m_utmsg_byteacct= nipmem_new bt_utmsg_byteacct_t();
	bt_err		= utmsg_byteacct()->start(bt_swarm(), bt_swarm()->swarm_utmsg(), NULL);
	if( bt_err.failed() )	return bt_err;

	// start the bt_utmsg_bcast_t extension
	m_utmsg_bcast= nipmem_new bt_utmsg_bcast_t();
	bt_err		= utmsg_bcast()->start(bt_swarm(), bt_swarm()->swarm_utmsg(), NULL);
	if( bt_err.failed() )	return bt_err;

	// start the bt_utmsg_fstart_t extension
	m_utmsg_fstart= nipmem_new bt_utmsg_fstart_t();
	bt_err		= utmsg_fstart()->start(bt_swarm(), bt_swarm()->swarm_utmsg(), NULL);
	if( bt_err.failed() )	return bt_err;

	// start the bt_utmsg_piecewish_t extension
	m_utmsg_piecewish= nipmem_new bt_utmsg_piecewish_t();
	bt_err		= utmsg_piecewish()->start(bt_swarm(), bt_swarm()->swarm_utmsg(), NULL);
	if( bt_err.failed() )	return bt_err;

	// start the bt_utmsg_punavail_t extension
	m_utmsg_punavail= nipmem_new bt_utmsg_punavail_t();
	bt_err		= utmsg_punavail()->start(bt_swarm(), bt_swarm()->swarm_utmsg(), NULL);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_swarm_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_swarm_t when to notify an event
 */
bool 	bt_ezswarm_share_t::neoip_bt_swarm_cb(void *cb_userptr, bt_swarm_t &cb_bt_swarm
					, const bt_swarm_event_t &swarm_event)	throw()
{
	// log to debug
	KLOG_WARN("enter event=" << swarm_event);
	
	// if the ezswarm_event is fatal, notify the bt_ezswarm_t of the failure
	if( swarm_event.is_fatal() ){
		bt_err_t bt_err(bt_err_t::ERROR, swarm_event.to_string());
		return bt_ezswarm->stateobj_notify_failure(bt_err);
	}

	// forward the bt_swarm_event_t to the bt_ezswarm_t caller IIF it is not fatal
	bt_ezswarm_event_t	ezswarm_event;
	ezswarm_event	= bt_ezswarm_event_t::build_swarm_event(&swarm_event);
	bool	tokeep	= bt_ezswarm->notify_callback(ezswarm_event);
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END





