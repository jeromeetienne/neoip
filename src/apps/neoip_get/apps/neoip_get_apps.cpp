/*! \file
    \brief Definition of the \ref get_apps_t class

\par Extraction of the metadata
- the bt_mfile_t is extracted from the link_type/link_addr via the bt_lnk2mfile_t class.
- then it checks if there is a resumedata file already saved at the destination
  directory.
  - if there is it, it uses it to resume the download where it stopped
- there is a special kludge to be able to resume a download without accessing
  the original metadata. use the -t resumedata
  - this allows to resume a download even when the original metadata are unreachable 

\par About using bt_io_cache_t delayed-write and bt_swarm_resumedata_t 
- when bt_io_cache_t delayed-write is used, a write is declared successfull
  to bt_swarm_t before it is even attempted to be written on disk.
- This can lead to wrong/misleading information in bt_swarm_resumedata_t
  - e.g. the write on disk returns an error (no more space, no permission etc..)
  - e.g. the disk flushing didnt have time to be made due to apps crash before
    clean completion
- This may happen in all applications which makes persistent storage of data
  between reboot. i.e. neoip-get/neoip-btcli
- POSSIBLE SOLUTION:
  - create a special variable in bt_swarm_resumedata_t, cleanly_saved()
    which is true IIF it is bt_io_cache_t is sure to be properly flushed. 
  - bt_swarm_t always return bt_swarm_resumedata_t with cleanly_saved set to false
  - it is up the apps to set it to true when bt_ezswarm_t successfully passed
    stopping.

\par TODO refactor the bt_mfile building
- it has been refactored in bt_lnk2mfile_t but this is still flacky 
  - then improve it 
- neoip-oload and neoip-get doesnt handle the bt_mfile_subfile_t local_path the 
  same way. 
  - neoip-get uses io direct in the local files via bt_io_sfile_t
  - neoip-oload uses io by piece via bt_io_pfile_t
  - TODO how to handle this in the bt_lnk2mfile_t
    - look at how it is implemented and see
- at first this object will be coded directly in the neoip-get directory
  - then ported to the neoip-get once completed
  - then ported to neoip-oload 
- neoip-oload needs to be able to filter files in the bt_mfile_t
  - as it exports the downloaded file via http, it can export only one
    file
  - TODO should this feature be part of bt_lnk2mfile_t ?
  - or another one able to remove file from the bt_mfile ? 
    - this function may be in the bt_mfile_helper_t and should check if the bt_mfile_t
      contains some piecehash. if so, it is not possible to remove file
    - may be simply to remove bt_mfile_subfile_t which are not wanted and 
      to do a complete_init

\par TODO refactor bt_ezswarm_t handling
- all the state handling in bt_ezswarm_t is bogus and way to hardcoded
- i should likely have a destination state, and have the bt_ezswarm to 
  navigate thru the state to reach this one.

*/

/* system include */
#include <cmath>
/* local include */
#include "neoip_get_apps.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_lnk2mfile.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_init.hpp"
#include "neoip_lib_session_exit.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_err.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezsession_opt.hpp"
#include "neoip_bt_mfile_helper.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_bt_swarm_resumedata_helper.hpp"
#include "neoip_ipport_aview_helper.hpp"
#include "neoip_file_utils.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_xml_parse_doc.hpp"
#include "neoip_xml_parse.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
get_apps_t::get_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	lib_session_exit= NULL;
	bt_lnk2mfile	= NULL;
	bt_ezswarm	= NULL;
	bt_ezsession	= NULL;
	// set default value
	resumedata_todelete	= false;
}
	
/** \brief Destructor
 */
get_apps_t::~get_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the pidfile and urlfile if needed
	if( !dest_dirpath.is_null() )	lib_apps_helper_t::urlfile_remove();
	// delete the lib_session_exit_t if needed
	nipmem_zdelete	lib_session_exit;
	// delete the bt_lnk2mfile_t if needed
	nipmem_zdelete	bt_lnk2mfile;
	// delete the bt_ezswarm_t if needed
	nipmem_zdelete	bt_ezswarm;
	// delete the bt_ezsession_t if needed
	nipmem_zdelete	bt_ezsession;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
get_apps_t &	get_apps_t::set_profile(const get_profile_t &profile)		throw()
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
bt_err_t	get_apps_t::start()	throw()
{
	lib_session_t *			lib_session	= lib_session_get();
	lib_apps_t *			lib_apps	= lib_session->lib_apps();
	const strvar_db_t &		arg_option	= lib_apps->arg_option();
	const std::vector<std::string> &arg_remain	= lib_apps->arg_remain();

	// check that there is some remaining arg, as it must contain the link_addr
	if( arg_remain.size() < 1 )
		return bt_err_t(bt_err_t::ERROR, "No metadata_link in the command line. see --help");

	// get the link_addr from the command line remaining
	link_addr	= arg_remain[0];
	
	// set the dest_dirpath to the default value - aka the current directory
	dest_dirpath	= file_utils_t::get_current_dir();
	// get the dest_dirpath from the command line option if specified
	if( arg_option.contain_key("dest-dir") )
		dest_dirpath	= arg_option.get_first_value("dest-dir");

	// get the link_type from the command line option if specified
	if( arg_option.contain_key("type") ){
		link_type	= arg_option.get_first_value("type");
		// check that the link_type parameter is a valid one
		if(link_type != "resumedata" && bt_lnk2mfile_type_t(link_type).is_null())
			return bt_err_t(bt_err_t::ERROR, "unknown link type in --type");
	}
	// if the link_type is null, guess it
	if( link_type.empty() ){
		file_path_t	link_path(link_addr); 
		// do a special case for "resumedata"
		if( !link_path.is_null() && link_path.basename_ext() == "resumedata" ){
			link_type	= "resumedata";
		}else{	// use bt_lnk2mfile_t guessing function
			link_type	= bt_lnk2mfile_type_from_addr(link_addr).to_string();
		}
	}

	// display the param - as they are understood by the program
	disp_param();
	
	// start the lib_session_exit_t
	lib_session_exit	= nipmem_new lib_session_exit_t();
	lib_session_exit->start(lib_session_get(), lib_session_exit_t::EXIT_ORDER_BT_SWARM
								, this, NULL);

	// create the urlfile - usefull to get the debug http of this apps
	lib_apps_helper_t::urlfile_create();

	// if link_type == "resumedata", read it and do the launch_ezsession/ezswarm
	if( link_type == "resumedata" )	return load_resumedata_link_addr();

	// init the bt_lnk2mfile_profile_t
	// - update the dest_local_dir with the one passed in parameters
	bt_lnk2mfile_profile_t	lnk2mfile_profile	= profile.lnk2mfile();
	lnk2mfile_profile.dest_local_dir(dest_dirpath.to_string());
	// start the bt_lnk2mfile_t
	bt_err_t	bt_err;
	bt_lnk2mfile	= nipmem_new bt_lnk2mfile_t();
	bt_err		= bt_lnk2mfile->profile(lnk2mfile_profile)
					.start(link_type, link_addr, this, NULL);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Helper on top of fatal error with only a reason
 */
bool	get_apps_t::fatal_error(const std::string &reason)	throw()
{
	return fatal_error(bt_err_t(bt_err_t::ERROR, reason));
}

/** \brief notify a fatal error and leave the event loop
 */
bool	get_apps_t::fatal_error(const bt_err_t &bt_err)	throw()
{
	//log to debug
	KLOG_STDOUT("fatal error " << bt_err);
	// as the bt_swarm_t got in error, we dont unregister it
	nipmem_zdelete	lib_session_exit;
	// stop the lib_session_t asap - to get the lib_session_exit_t from the rest of the code
	lib_session_get()->loop_stop_asap();
	// return true for tokeep convenience - object it still valid
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_lnk2mfile_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_lnk2mfile_t to provide event
 */
bool	get_apps_t::neoip_bt_lnk2mfile_cb(void *cb_userptr, bt_lnk2mfile_t &cb_bt_lnk2mfile
				, const bt_err_t &cb_bt_err, const bt_lnk2mfile_type_t &lnk2mfile_type
				, const bt_mfile_t &cb_bt_mfile)		throw() 
{
	bt_err_t	bt_err	= cb_bt_err;	
	// log to debug
	KLOG_WARN("enter bt_err=" << cb_bt_err << " lnk2mfile_type=" << lnk2mfile_type);

	// handle the error
	if( bt_err.failed() )	return fatal_error(bt_err);

	// copy the resulting link_type
	link_type		= lnk2mfile_type.to_string();
	// build the bt_swarm_resumedata_t from the resulting bt_mfile_t
	m_swarm_resumedata	= bt_swarm_resumedata_t::from_mfile(cb_bt_mfile);

	// delete the bt_lnk2mfile_t
	nipmem_zdelete bt_lnk2mfile;

	// load the resumedata load if availablae
	bt_err		= resumedata_load_if_avail();
	if( bt_err.failed() )	return fatal_error(bt_err);

	// launch the bt_ezsession_t
	bt_err		= launch_ezsession();
	if( bt_err.failed() )	return fatal_error(bt_err);
	
	// launch the bt_ezswarm_t 
	bt_err		= launch_ezswarm();
	if( bt_err.failed() )	return fatal_error(bt_err);
	
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			resumedata resuming 'kludge'
// - here the point is to be able to resume a download with only the .resumedata
// - this is usefull if the original metadata are nomore reachable  
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief generate the bt_mfile_t from the metadata_datum containing a .torrent
 * 
 * - leave the bt_swarm_resumedata_t untouched
 */
bt_err_t	get_apps_t::load_resumedata_link_addr()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// load the bt_swarm_resumedata_t from the link_addr
	m_swarm_resumedata	= bt_swarm_resumedata_helper_t::from_file(link_addr);
	if( !swarm_resumedata().check().succeed() )
		return bt_err_t(bt_err_t::ERROR, "unable to parse the link as a .resumedata");

	// launch the bt_ezsession_t
	bt_err_t	bt_err;
	bt_err		= launch_ezsession();
	if( bt_err.failed() )	return bt_err;
	
	// launch the bt_ezswarm_t 
	bt_err		= launch_ezswarm();
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_ezswarm_t handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_ezsession_t
 */
bt_err_t	get_apps_t::launch_ezsession()	throw()
{
	const strvar_db_t &	session_conf	= lib_session_get()->session_conf();
	lib_apps_t *		lib_apps	= lib_session_get()->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	bt_err_t		bt_err;

	// get the tcp_listen_aview from the config
	ipport_aview_t	tcp_listen_aview;
	tcp_listen_aview= ipport_aview_helper_t::tcp_listen_aview_from_conf();
	// sanity check - the tcp_listen_aview local view MUST NOT be null
	DBG_ASSERT( !tcp_listen_aview.is_null() );

	// get the udp_listen_aview from the config
	ipport_aview_t	udp_listen_aview;
	udp_listen_aview= ipport_aview_helper_t::udp_listen_aview_from_conf();
	// sanity check - the udp_listen_aview local view MUST NOT be null
	DBG_ASSERT( !udp_listen_aview.is_null() );
	// log to debug
	KLOG_ERR("tcp_listen_aview=" << tcp_listen_aview << " udp_listen_aview=" << udp_listen_aview);

	// get the nslan_addr from the config
	ipport_addr_t	nslan_addr	= session_conf.get_first_value("nslan_ipport", "255.255.255.255:7777");

	// set the bt_ezswarm_opt_t
	bt_ezsession_opt_t	ezsession_opt;
	ezsession_opt	|= bt_ezsession_opt_t::NSLAN_PEER;
	ezsession_opt	|= bt_ezsession_opt_t::KAD_PEER;

	// build the bt_ezsession_profile_t
	bt_ezsession_profile_t	ezsession_profile;
	ezsession_profile.nslan_addr	(nslan_addr);
	ezsession_profile.nslan_realmid	("nslan_realm_neoip_bt");
	ezsession_profile.kad_realmid	("kad_realm_neoip_bt");

	// if arg_option contain xmit-maxrate, set it up
	if( arg_option.contain_key("xmit-maxrate") ){
		size_t maxrate	= string_t::to_uint32(arg_option.get_first_value("xmit-maxrate"));
		ezsession_opt	|= bt_ezsession_opt_t::XMIT_RSCHED;
		ezsession_profile.xmit_maxrate( maxrate );
	}
	// if arg_option contain recv-maxrate, set it up
	if( arg_option.contain_key("recv-maxrate") ){
		size_t maxrate	= string_t::to_uint32(arg_option.get_first_value("recv-maxrate"));
		ezsession_opt	|= bt_ezsession_opt_t::RECV_RSCHED;
		ezsession_profile.recv_maxrate( maxrate );
	}

	// start the bt_ezsession_t;
	bt_ezsession	= nipmem_new bt_ezsession_t();
	bt_err		= bt_ezsession->profile(ezsession_profile).start(ezsession_opt, tcp_listen_aview
							, udp_listen_aview, kad_peerid_t::build_random()
							, bt_id_t::build_peerid("azureus", "1.2.3.4"));
	if( bt_err.failed() )	return bt_err;


	// return no error
	return bt_err_t::OK;	
}

/** \brief Launch the bt_ezswarm_t
 */
bt_err_t	get_apps_t::launch_ezswarm()	throw()
{
	bt_err_t	bt_err;

	// sanity check - the bt_swarm_resumedata_t MUST check().succeed()
	DBG_ASSERT( swarm_resumedata().check().succeed() );	
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
	ezswarm_profile.peersrc_kad_peer(bt_ezsession->kad_peer());

	// set the bt_ezswarm_opt_t
	bt_ezswarm_opt_t	ezswarm_opt;
	ezswarm_opt	|= bt_ezswarm_opt_t::IO_SFILE;
	ezswarm_opt	|= bt_ezswarm_opt_t::ECNX_HTTP;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_KAD;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_NSLAN;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_UTPEX;
	// if the bt_mfile_t has a announce_uri(), use the bt_peersrc_http_t too
	if( !bt_mfile().announce_uri().is_null() )	ezswarm_opt |= bt_ezswarm_opt_t::PEERSRC_HTTP;

	// start a bt_swarm_t depending on the availability of a bt_swarm_resumedata_t
	bt_ezswarm	= nipmem_new bt_ezswarm_t();
	bt_ezswarm->set_profile(ezswarm_profile);
	bt_err		= bt_ezswarm->start(swarm_resumedata(), ezswarm_opt, bt_ezsession, this, NULL);
	if( bt_err.failed() )	return bt_err;
	// make bt_ezswarm_t to gracefully goto SHARE
	bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::SHARE);

	// display the metadata
	disp_mfile();
	// start the display_timeout
	display_timeout.start(profile.display_period(), this, NULL);
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
bool 	get_apps_t::neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw()
{
	lib_apps_t *		lib_apps	= lib_session_get()->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	// log to debug
	KLOG_WARN("enter ezevent=" << ezswarm_event);
	
	// if it is a fatal bt_ezswarm_t, notify a error
	if( bt_ezswarm->cur_state().is_error() ){
		// goto the a new line
		KLOG_STDOUT("\n");
		return fatal_error("Received a fatal event " + bt_ezswarm->cur_state().reason());
	}
	
	// handle the bt_ezswarm_event_t depending of its type
	switch(ezswarm_event.get_value()){
	case bt_ezswarm_event_t::LEAVE_STATE_PRE:
			// display the current status of the get_apps_t
			disp_ezswarm();
			// if about to leave bt_ezswarm_state_t::SHARE, do a special display
			if( bt_ezswarm->in_share() )	disp_leave_share();
			// if about to leave bt_ezswarm_state_t::SHARE, save the resumedata
			// - thus it is always the most uptodate
			if( bt_ezswarm->in_share() )	resumedata_save();
			// if entered in bt_ezswarm_state_t::SHARE, stop the resumedata_timeout
			if( bt_ezswarm->in_share() )	resumedata_timeout.stop();
			// if leaving share and bt_swarm_t is seed and keep-resumedata is not set
			// set resumedata_todelete - it will delete the resumedata in stopped
			if( bt_ezswarm->in_share() ){
				bt_swarm_t * bt_swarm	= bt_ezswarm->share()->bt_swarm();
				if( bt_swarm->is_seed() && !arg_option.contain_key("keep-resumedata") )
					resumedata_todelete	= true;
			}
			break;
	case bt_ezswarm_event_t::ENTER_STATE_POST:
			// display the current status of the get_apps_t
			disp_ezswarm();
			// if entered in bt_ezswarm_state_t::SHARE, set some variables to display stats
			if( bt_ezswarm->in_share() ){
				bt_swarm_t * bt_swarm	= bt_ezswarm->share()->bt_swarm();
				enter_share_date	= date_t::present();
				enter_share_anyavail	= bt_swarm->totfile_anyavail();
			}
			// if entered in bt_ezswarm_state_t::SHARE, start the resumedata_timeout
			if( bt_ezswarm->in_share() )
				resumedata_timeout.start(profile.resumedata_autosave_period(), this, NULL);
			// if entered bt_ezswarm_state_t::SHARE, save the resumedata
			// - thus it is saved as soon as possible when starting a new one
			if( bt_ezswarm->in_share() )				resumedata_save();
			// if entered in bt_ezswarm_state_t::STOPPED and resumedata_todelete, delete it
			if( bt_ezswarm->in_stopped() && resumedata_todelete )	file_utils_t::remove_file( resumedata_filename() );
			// if entered in bt_ezswarm_state_t::STOPPED, leave the programm
			if( bt_ezswarm->in_stopped() ){
				// erase the line to remove last event display
				KLOG_STDOUT("\r");
				// delete the lib_session_exit_t as the STOPPING is already done
				nipmem_zdelete	lib_session_exit;
				// stop the lib_session_t as soon as possible
				// - required to make the apps leave automatically when becoming seed
				lib_session_get()->loop_stop_asap();
			}
			break;
	case bt_ezswarm_event_t::SWARM_EVENT:{
			DBG_ASSERT( bt_ezswarm->in_share() );
			bt_swarm_t * bt_swarm	= bt_ezswarm->share()->bt_swarm();
			// if the bt_swarm_t is a seed, gracefully goto stopped
			if( bt_swarm->is_seed() && !arg_option.contain_key("keep-seeding") )
				bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::STOPPED);
			break;}
	default:	break;
	}

	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Display the current status when being in bt_ezswarm_state_t::SHARE
 */
void	get_apps_t::disp_param()			throw()
{
	// sanity check - the get_apps_t MUST NOT be in bt_ezswarm_t
	DBG_ASSERT( !bt_ezswarm );

	KLOG_STDOUT("getting a [" << link_addr << "] as [" << link_type << "]\n");
	KLOG_STDOUT("\tDestination directory: " << dest_dirpath << "\n");
}

/** \brief Display some data about the bt_mfile
 */
void	get_apps_t::disp_mfile()			throw()
{
	// sanity check - the get_apps_t MUST be in bt_ezswarm_t
	DBG_ASSERT( bt_ezswarm );

	file_size_t	totfile_size	= bt_ezswarm->mfile().totfile_size();
	KLOG_STDOUT("\tLength: " << string_t::size_sepa(totfile_size.to_uint64()) << "-byte"
				<< " (" << string_t::size_string(totfile_size.to_uint64()) << ")"
				<< "\n");
}

/** \brief Display some stuff when a bt_swarm_resumedata_t has been found
 */
void	get_apps_t::disp_found_resumedata()			throw()
{
	KLOG_STDOUT("\tResumedata found in [" << resumedata_filename() << "]");
	KLOG_STDOUT("\n");
}


/** \brief Display some stuff when leaving the bt_ezswarm_state_t::SHARE
 */
void	get_apps_t::disp_leave_share()			throw()
{
	lib_apps_t *		lib_apps	= lib_session_get()->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	// sanity check - the get_apps_t MUST be in the proper state
	DBG_ASSERT( bt_ezswarm->in_share() );

	bt_swarm_t * 	bt_swarm	= bt_ezswarm->share()->bt_swarm();
	file_size_t	totfile_anyavail= bt_swarm->totfile_anyavail();
	file_size_t	dloaded_size	= 0;
	
	if( totfile_anyavail > enter_share_anyavail )
		dloaded_size	= totfile_anyavail - enter_share_anyavail;

	delay_t		share_delay	= date_t::present() - enter_share_date;
	double		avg_rate	= dloaded_size.to_double() / share_delay.to_sec_double();
	KLOG_STDOUT("\n\t");
	if( bt_swarm->is_seed() && !arg_option.contain_key("keep-seeding"))	KLOG_STDOUT("Completed.");
	else									KLOG_STDOUT("Interrupted.");
	KLOG_STDOUT("\tdownloaded "<< string_t::size_string(dloaded_size.to_uint64()) 
			<< " in " << string_t::strip(string_t::delay_string(share_delay))
			<< " ("<< string_t::strip(string_t::size_string(uint64_t(avg_rate))) << "/s)\n");
}

/** \brief callback called when the timeout_t expire
 */
bool get_apps_t::display_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// sanity check - the timeout_t MUST be the proper one
	DBG_ASSERT( &cb_timeout == &display_timeout );
	// display the status
	disp_ezswarm();
	// return tokeep
	return true;
}

/** \brief Display the current bt_ezswarm_t status
 */
void	get_apps_t::disp_ezswarm()			throw()
{
	std::string	status_str;
	// sanity check - the get_apps_t MUST be in bt_ezswarm
	DBG_ASSERT( bt_ezswarm );

	switch( bt_ezswarm->cur_state().get_value() ){
	case bt_ezswarm_state_t::ALLOC:		status_str	= status_str_alloc();	break;
	case bt_ezswarm_state_t::CHECK:		status_str	= status_str_check();	break;
	case bt_ezswarm_state_t::SHARE:		status_str	= status_str_share();	break;
	case bt_ezswarm_state_t::STOPPING:	status_str	= status_str_stopping();break;
	case bt_ezswarm_state_t::STOPPED:	status_str	= status_str_stopped();	break;
	default:	DBG_ASSERT( 0 );
	}
	
	// display the status_str
	// - TODO this 78 to get the widht of the screen is lame
	KLOG_STDOUT("\r" << status_str);
	if( status_str.size() < 78 )	KLOG_STDOUT(std::string(78 - status_str.size(), ' '));
}

/** \brief Return the status string when being in bt_ezswarm_state_t::ALLOC
 */
std::string	get_apps_t::status_str_alloc()			throw()
{
	std::ostringstream	oss;
	// sanity check - the get_apps_t MUST be in the proper state
	DBG_ASSERT( bt_ezswarm->in_alloc() );
	// build the string
	oss << "[ALLOCATING]";
	// return the just built string
	return oss.str();
}

/** \brief Return the status string when being in bt_ezswarm_state_t::CHECK
 */
std::string	get_apps_t::status_str_check()			throw()
{
	std::ostringstream	oss;
	// sanity check - the get_apps_t MUST be in bt_ezswarm_state_t::CHECK
	DBG_ASSERT( bt_ezswarm->in_check() );
	// build the string
	oss << "[CHECKING]";
	// return the just built string
	return oss.str();
}

/** \brief Return the status string when being in bt_ezswarm_state_t::SHARE
 */
std::string	get_apps_t::status_str_share()			throw()
{
	std::ostringstream	oss;
	// sanity check - the get_apps_t MUST be in the proper state
	DBG_ASSERT( bt_ezswarm->in_share() );
	// compute local variable
	bt_swarm_t * 	bt_swarm	= bt_ezswarm->share()->bt_swarm();
	const bt_mfile_t & bt_mfile	= bt_swarm->get_mfile();
	file_size_t	totfile_avail	= bt_swarm->totfile_anyavail();
	file_size_t	totfile_size	= bt_mfile.totfile_size();
	file_size_t	totfile_remain	= totfile_size - totfile_avail;
	double		curavail_ratio	= totfile_avail.to_double() / totfile_size.to_double();
	double		oldavail_ratio	= enter_share_anyavail.to_double() / totfile_size.to_double();
	double		recv_rate	= bt_swarm->recv_rate();
	// compute the estimated time of arrival
	delay_t		eta;
	if( recv_rate != 0 )	eta	= delay_t::from_msec(uint64_t(1000.0*(totfile_remain.to_double() / recv_rate)));
	else			eta	= delay_t::INFINITE;

	// clamp the oldavail_ratio with the curavail_ratio
	// - NOTE: this is a special as the curavail_ratio may be decreased in case of 
	//   failed piecehash checking.
	oldavail_ratio	= std::min(curavail_ratio, oldavail_ratio);

	// build the string
	oss << string_t::percent_string(curavail_ratio)
			<< " " << string_t::progress_bar_str(curavail_ratio, oldavail_ratio, 30)
			<< " " << string_t::size_sepa(totfile_avail.to_uint64())
			<< "   " << string_t::size_string(uint64_t(recv_rate))<< "/s"
			<< "   ETA " << string_t::delay_string(eta);
	// return the just built string
	return oss.str();
}

/** \brief Return the status string when being in bt_ezswarm_state_t::STOPPING
 */
std::string	get_apps_t::status_str_stopping()			throw()
{
	std::ostringstream	oss;
	// sanity check - the get_apps_t MUST be in the proper state
	DBG_ASSERT( bt_ezswarm->in_stopping() );
	// build the string
	oss << "[STOPPING]";
	// return the just built string
	return oss.str();
}

/** \brief Return the status string when being in bt_ezswarm_state_t::STOPPED
 */
std::string	get_apps_t::status_str_stopped()			throw()
{
	std::ostringstream	oss;
	// sanity check - the get_apps_t MUST be in the proper state
	DBG_ASSERT( bt_ezswarm->in_stopped() );
	// build the string
	oss << "[STOPPED]";
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_swarm_resumedata_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the resumedata filename
 */
file_path_t	get_apps_t::resumedata_filename()	const throw()
{
	// sanity check - the bt_swarm_resumedata_t MUST check().succeed()
	DBG_ASSERT( swarm_resumedata().check().succeed() );
	// sanity check - dest_dirpath MUST be init
	DBG_ASSERT( !dest_dirpath.is_null() );

	// if the link_type is resumedata, the resumedata filename is the link_addr
	if( link_type == "resumedata" )	return link_addr;

	// in all other cases, the filename is dest_dir / bt_mfile().name() + ".resumedata"
	return dest_dirpath / (bt_mfile().name().to_string() + ".resumedata");
}

/** \brief Load the resumedata if it is available
 */
bt_err_t	get_apps_t::resumedata_load_if_avail()	throw()
{
	// get the filename
	file_path_t	resumedata_path	= resumedata_filename();
	// if the link_type is already a resumedata, no need to reload ot
	if( link_type == "resumedata" )			return bt_err_t::OK;
	// if the file doesnt exist, return now
	if( file_stat_t(resumedata_path).is_null() )	return bt_err_t::OK;

	// load the bt_swarm_resumedata_t from the link_addr
	m_swarm_resumedata	= bt_swarm_resumedata_helper_t::from_file(resumedata_path);
	if( !swarm_resumedata().check().succeed() )
		return bt_err_t(bt_err_t::ERROR, "unable to parse " + resumedata_path.to_string() + " as a .resumedata");
	
	// display the event
	disp_found_resumedata();
	// return no error
	return bt_err_t::OK;
}	

/** \brief Load the resumedata if it is available
 */
bt_err_t	get_apps_t::resumedata_save()	throw()
{
	// sanity check - the bt_ezswarm_state_t MUST be SHARE
	DBG_ASSERT( bt_ezswarm->in_share() );
	// log to dbeug
	KLOG_DBG("save resumedata");
	// save the bt_swarm_resume_data_t
	bt_swarm_t *	bt_swarm	= bt_ezswarm->share()->bt_swarm();
	bt_err_t	bt_err;
	bt_err	= bt_swarm_resumedata_helper_t::to_file(bt_swarm->get_current_resumedata(), resumedata_filename());
	if( bt_err.failed() ){
		KLOG_ERR("Cant write the bt_swarm_resumedata_t in " << resumedata_filename() << " due to " << bt_err);
		return bt_err;
	}
	// return no error
	return bt_err_t::OK;
}


/** \brief callback called when the timeout_t expire
 */
bool get_apps_t::resumedata_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// sanity check - the timeout_t MUST be the proper one
	DBG_ASSERT( &cb_timeout == &resumedata_timeout );
	// sanity check - the bt_ezswarm MUST be in bt_ezswarm_state_t::SHARE
	DBG_ASSERT( bt_ezswarm->in_share() );
	// save the resumedata
	resumedata_save();
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
 * - NOTE: this function is just a fork toward the other callbacks
 */
bool get_apps_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	if( &cb_timeout == &display_timeout )		return display_timeout_cb(userptr, cb_timeout);
	if( &cb_timeout == &resumedata_timeout )	return resumedata_timeout_cb(userptr, cb_timeout);
	DBG_ASSERT( 0 );
	return false;	// only to avoid a compiler warning
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			lib_session_exit_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref lib_session_exit_t when to notify an event
 */
bool	get_apps_t::neoip_lib_session_exit_cb(void *cb_userptr, lib_session_exit_t &session_exit) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// if the bt_ezswarm_t is init and is in bt_ezswarm_state_t, gracefully goto stopped
	if( bt_ezswarm->in_share() ){
		bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::STOPPED);
		return true;
	}

	// else delete the exit procedure
	nipmem_zdelete	lib_session_exit;
	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	get_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --type cmdline option
	clineopt	= clineopt_t("type", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Determine the type of the file to download"
						"\n\t\tThe parameter may be one of {torrent|metalink|static|resumedata}"
						"\n\t\tIf this option is not provided, it attempts to deduce it from the filename extension");
	clineopt.alias_name_db().append("t");
	clineopt_arr	+= clineopt;
	// add the --dest-dir cmdline option
	clineopt	= clineopt_t("dest-dir", clineopt_mode_t::REQUIRED)
						.option_mode(clineopt_mode_t::OPTIONAL)
						.help_string("Specify the destination directory"
							"\n\t\tIf this option is not specified, it defaults to the current directory");
	clineopt.alias_name_db().append("o");
	clineopt_arr	+= clineopt;
	// add the --xmit-maxrate cmdline option
	clineopt	= clineopt_t("xmit-maxrate", clineopt_mode_t::REQUIRED)
						.option_mode(clineopt_mode_t::OPTIONAL)
						.help_string("Specify the maximum rate in xmit"
							"\n\t\tIf this option is not specified, there are no limit.");
	clineopt.alias_name_db().append("x");
	clineopt_arr	+= clineopt;
	// add the --recv-maxrate cmdline option
	clineopt	= clineopt_t("recv-maxrate", clineopt_mode_t::REQUIRED)
						.option_mode(clineopt_mode_t::OPTIONAL)
						.help_string("Specify the maximum rate in recv"
							"\n\t\tIf this option is not specified, there are no limit.");
	clineopt.alias_name_db().append("r");
	clineopt_arr	+= clineopt;
	// add the --keep-resumedata cmdline option
	clineopt	= clineopt_t("keep-resumedata", clineopt_mode_t::NOTHING)
						.option_mode(clineopt_mode_t::OPTIONAL)
						.help_string("Dont delete the resumedata file when the download is completed"
							"\n\t\tIf this option is not specified, it will be deleted.");
	clineopt.alias_name_db().append("k");
	clineopt_arr	+= clineopt;
	// add the --keep-seeding cmdline option
	clineopt	= clineopt_t("keep-seeding", clineopt_mode_t::NOTHING)
						.option_mode(clineopt_mode_t::OPTIONAL)
						.help_string("When the files are fully downloaded, keep seeding on them"
							"\n\t\tIf this option is not specified, the programm will exit immediatly.");
	clineopt.alias_name_db().append("s");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END

