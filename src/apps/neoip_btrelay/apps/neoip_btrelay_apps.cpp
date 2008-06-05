/*! \file
    \brief Definition of the \ref btrelay_apps_t

\par About multiple bt_swarm_t and --type option parsing
- it is possible to relay multiple bt_swarm_t at the same time
- simply append the multiple link_addr to the command line options
- If the link_type of those link may not be guessed, it is possible to specify
  the link_type of each individual link_addr
  - It requires to set multiple --type options in the command line
  - the first link_type is used for the first link_addr, the second link_type
    for the second link_addr, and so on
  - if it a link_type is not provided. it will be guessed  
  - TODO: BUT rather dirty because all the --type have to be defined *before*
    the first link_addr. so the link_type and the matching link_addr are
    decorrelated in the command line. which is dirty 

\par Possible Improvement
- TODO it would be good not to delete the available piece when rebooting the apps
  - it would avoid to uselessly take from the swarm when rebooting
  - meaning a bt_io_pfile_t which doesnt delete the data
  - meaning a bt_swarm_resumedata_t which store the available data from one 
    boot to another
  - where to store those data ? in a ~/.neoip-btrelay ?
    - seems premature to code it now

\par Possible use case
- put it in the given box on a lan, and let it seed but only when the lan is 
  not used by others.
  - require a bandwidth watcher

*/

/* system include */
/* local include */
#include "neoip_btrelay_apps.hpp"
#include "neoip_btrelay_itor.hpp"
#include "neoip_btrelay_swarm.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezsession_opt.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_relay.hpp"

#include "neoip_bt_lnk2mfile.hpp"

#include "neoip_bt_cast_mdata.hpp"

#include "neoip_ipport_addr.hpp"
#include "neoip_ipport_aview_helper.hpp"

#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_strvar_helper.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_file.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
btrelay_apps_t::btrelay_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_bt_ezsession	= NULL;
	m_bt_relay	= NULL;
}

/** \brief Destructor
 */
btrelay_apps_t::~btrelay_apps_t()	throw()
{
	bool	was_started	= m_bt_ezsession ? true : false;
	// log to debug
	KLOG_DBG("enter");
	// delete all the itor_db if needed
	while( !itor_db.empty() )	nipmem_delete itor_db.front();	
	// delete all the swarm_db if needed
	while( !swarm_db.empty() )	nipmem_delete swarm_db.front();
	// delete the bt_relay_t if needed
	nipmem_zdelete	m_bt_relay;
	// delete the bt_ezsession_t if needed
	nipmem_zdelete	m_bt_ezsession;
	// delete the pidfile and urlfile if needed
	if( was_started ){
		lib_apps_helper_t::urlfile_remove();
		file_utils_t::remove_directory(io_pfile_dirpath(), file_utils_t::DO_RECURSION);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	btrelay_apps_t::start()						throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	file_path_t		config_path	= lib_session->conf_rootdir() / "btrelay";
	bt_err_t		bt_err;
	// log to debug
	KLOG_WARN("enter");

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/btrelay_apps_" + OSTREAMSTR(this));
	
	// load the configuration file for the router
	strvar_db_t btrelay_conf	= strvar_helper_t::from_file(config_path / "neoip_btrelay.conf");
	
	// start the bt_ezsession_t
	bt_err		= launch_ezsession();
	if( bt_err.failed() )		return bt_err;
	
	// start the bt_relay_t
	m_bt_relay	= nipmem_new bt_relay_t();
	bt_err		= bt_relay()->profile(m_profile.bt_relay()).start();
	if( bt_err.failed() )	return bt_err;

	// create the urlfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::urlfile_create();

	// set the io_pfile_dirpath name
	m_io_pfile_dirpath	= lib_session->temp_rootdir() / (lib_apps->canon_name()+"_pfile.");
	m_io_pfile_dirpath	= file_utils_t::get_temp_path(m_io_pfile_dirpath);
	// create the temporary directory io_pfile_dirpath()
	// - TODO this should be created by bt_alloc_t?
	// - all this stuff about bt_alloc_t and bt_io_pfile_t is unclear
	// - there is the deletion of this directory in the btrelay_apps_t dtor
	file_err_t	file_err;
	file_err	= file_utils_t::create_directory(io_pfile_dirpath(), file_utils_t::NO_RECURSION);
	if( file_err.failed() )	return bt_err_from_file(file_err);

	/*
	 * parse the command line to get the link_type/addr
	 */	
	const std::vector<std::string> &arg_remain	= lib_apps->arg_remain();
	// check that there is some remaining arg, as it must contain the link_addr
	if( arg_remain.size() < 1 )
		return bt_err_t(bt_err_t::ERROR, "No metadata_link in the command line. see --help");
	// goes thru all the link_addr in the arg_remains
	size_t	type_optidx	= 0;
	for(size_t i = 0; i < arg_remain.size(); i++){
		std::string	link_addr	= arg_remain[i];
		std::string	link_type;
		// determine the link_type for this particular link_addr
		if( arg_option.next_key_idx("type", type_optidx) != arg_option.INDEX_NONE ){
			type_optidx	= arg_option.next_key_idx("type", type_optidx);
			link_type	= arg_option[type_optidx].val(); 
		}else{
			link_type	= std::string();
		}
		// launch the btrelay_itor_t
		bt_err		= launch_itor(link_type, link_addr);
		if( bt_err.failed() )	return bt_err;
	}

	// return no error
	return bt_err_t::OK;
}

/** \brief Launch the bt_ezsession_t
 */
bt_err_t	btrelay_apps_t::launch_ezsession()	throw()
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
	ipport_addr_t	nslan_addr = session_conf.get_first_value("nslan_ipport", "255.255.255.255:7777");

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
	m_bt_ezsession	= nipmem_new bt_ezsession_t();
	bt_err		= m_bt_ezsession->profile(ezsession_profile).start(ezsession_opt
					, tcp_listen_aview, udp_listen_aview
					, kad_peerid_t::build_random()
					, bt_id_t::build_peerid("azureus", "0.0.4.2"));
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;	
}

/** \brief Try to launch a btrelay_itor_t with the link_type/link_addr
 */
bt_err_t	btrelay_apps_t::launch_itor(const std::string &orig_link_type
						, const std::string &link_addr)	throw()
{
	std::string	link_type	= orig_link_type;
	bt_err_t	bt_err;
	// log to debug
	KLOG_ERR("enter link_type=" << link_type << " link_addr=" << link_addr);
	// check that the link_type parameter is a valid one
	if( link_type != "cast" && !link_type.empty() ){
		if( bt_lnk2mfile_type_t(link_type).is_null() )
			return bt_err_t(bt_err_t::ERROR, "unknown link type in --type");
	}

	// if the link_type is null, try to guess it with bt_link_mfile_type_t stuff
	if( link_type.empty() )	link_type = bt_lnk2mfile_type_from_addr(link_addr).to_string();

	// launch the btrelay_itor_t
	btrelay_itor_t*	btrelay_itor;
	btrelay_itor	= nipmem_new btrelay_itor_t();
	bt_err		= btrelay_itor->start(this, link_type, link_addr, this, NULL);
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
bool	btrelay_apps_t::fatal_error(const std::string &reason)	throw()
{
	return fatal_error(bt_err_t(bt_err_t::ERROR, reason));
}

/** \brief notify a fatal error and leave the event loop
 */
bool	btrelay_apps_t::fatal_error(const bt_err_t &bt_err)	throw()
{
	//log to debug
	KLOG_ERR("fatal error " << bt_err);
	// stop the lib_session_t loop now
	lib_session_get()->loop_stop_now();
	// return true for tokeep convenience - object it still valid
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			btrelay_itor_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref btrelay_itor_t when it is completed
 */
bool btrelay_apps_t::neoip_btrelay_itor_cb(void *cb_userptr, btrelay_itor_t &cb_btrelay_itor
			, const bt_err_t &cb_bt_err, const bt_mfile_t &bt_mfile
			, const bt_cast_mdata_t &cast_mdata)	throw()
{
	btrelay_itor_t*	btrelay_itor	= &cb_btrelay_itor;
	bt_err_t	bt_err		= cb_bt_err;
	btrelay_swarm_t*btrelay_swarm;
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err << " bt_mfile.name()=" << bt_mfile.name() );

	// if bt_err.failed(), just delete btrelay_itor and return
	if( bt_err.failed() )	return fatal_error(bt_err);

	// sanity check - at this point, bt_err.succeed() MUST be true
	DBG_ASSERT( bt_err.succeed() );

	// create btrelay_swarm_t
	btrelay_swarm	= nipmem_new btrelay_swarm_t();
	bt_err		= btrelay_swarm->start(this, bt_mfile, cast_mdata);
	if( bt_err.failed() )	return fatal_error(bt_err);
	
	// delete the btrelay_itor
	nipmem_zdelete btrelay_itor;
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
clineopt_arr_t	btrelay_apps_t::clineopt_arr()	throw()
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
	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END;




