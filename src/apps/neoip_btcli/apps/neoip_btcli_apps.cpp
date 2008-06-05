/*! \file
    \brief Definition of the \ref btcli_apps_t class

- TODO to clean up
  - the order of the function are unclear
  - most are not commented
  - not so bad except this

\par About lookmeta_dir handling
- lookmeta_dir_arr is a database of file_path_t directory which gonna be periodically
  scanned for new metadata files
- if one is found, it is copied to partmeta_dir, then deleted from the lookmeta_dir
  when it has been found.
- POSSIBLE ALTERNATIVE: not to delete file in external directories
  - i dont like to delete file automatically, especially in external directories
  - why not doing : dont delete the original file and copy the files to partmeta_dir
    IIF it is not in the partmeta_dir *AND* in the fullmeta_dir ?
  - it allows not to delete the file but create a weird case:
    1. the user put a metafile in the lookmeta_dir, it got copied to partmeta_dir
    2. the datafile got downloaded and put in fulldata_dir
    3. the user take the datafile and put it in a more personnal directory
    4. here is the bug, the lookmeta_dir get scanned and find the original 
       metafile and so loop again...
       - in this case, it is up to the user to delete himself the metafile from
         the lookmeta_dir
  - this is a not a bad policy but is it better than the previous one ? unclear
    - currently i dont have the brain to decide or even think about it
    - TODO to decide later
       
*/

/* system include */
/* local include */
#include "neoip_btcli_apps.hpp"
#include "neoip_btcli_swarm.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_btcli_resp.hpp"

#include "neoip_lib_session.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_ipport_aview_helper.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_strvar_helper.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_file.hpp"
#include "neoip_bytearray.hpp"
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
btcli_apps_t::btcli_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	m_bt_ezsession	= NULL;
}
	
/** \brief Destructor
 */
btcli_apps_t::~btcli_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the btcli_resp_t if needed
	nipmem_zdelete	btcli_resp;
	// delete all pending btcli_swarm_t
	while( !swarm_db.empty() )	nipmem_delete swarm_db.front();
	// delete the bt_ezsession_t if needed
	nipmem_zdelete	m_bt_ezsession;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
btcli_apps_t &	btcli_apps_t::set_profile(const btcli_profile_t &m_profile)	throw()
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
bt_err_t	btcli_apps_t::start()	throw()
{
	lib_apps_t *		lib_apps	= lib_session_get()->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	bt_err_t		bt_err;
	
	// read the configuration file
	bt_err	= read_fileconf();
	if( bt_err.failed() )	return bt_err;

	// launch the bt_ezsession_t
	bt_err	= launch_ezsession();
	if( bt_err.failed() )	return bt_err;

	// launch the btcli_resp_t 
	// - to handle the bt_ezsession_t via RPC
	btcli_resp	= nipmem_new btcli_resp_t();
	bt_err		= btcli_resp->start(this);
	if( bt_err.failed() )	return bt_err;

	// do a initial scane_dir_look()
	bt_err	= lookmeta_dir_scan();
	if( bt_err.failed() )	return bt_err;

	// start the lookmeta_timeout to launch_swarm_look() periodically
	lookmeta_timeout.start(profile().look_dir_period(), this, NULL);
	
	// do the launch_from_partmeta_dir()
	bt_err	= launch_from_partmeta_dir();
	if( bt_err.failed() )	return bt_err;

	// if the arg_option DOES NOT contains the "nodaemon" key, pass daemon
	// - NOTE: be carefull this change the pid
	if( !arg_option.contain_key("nodaemon") && lib_apps_helper_t::daemonize().failed() )
		return bt_err_t(bt_err_t::ERROR, "Can't daemon()");

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			initialization function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Read the configuration files
 */
bt_err_t	btcli_apps_t::read_fileconf()		throw()
{
	file_path_t	conf_rootdir	= lib_session_get()->conf_rootdir();
	file_path_t	config_path	= conf_rootdir / "btcli" / "neoip_btcli.conf";
	strvar_db_t	btcli_conf	= strvar_helper_t::from_file(config_path);

	// log to debug
	KLOG_DBG("strvar btcli.conf=" << btcli_conf);

/*
 * Get the full/part meta/data dir variables from the configuration file
 * - if there are not present, there is default in home dir which is created 
 */
	// read the fulldata_dir variable
	if( btcli_conf.contain_key("fulldata_dir") ){
		m_fulldata_dir	= btcli_conf.get_first_value("fulldata_dir");		
	}else{	// if not present, set it to a default in home dir and create it if needed
		m_fulldata_dir	= file_utils_t::get_home_dir() / ".neoip-btcli" / "completed";
		file_utils_t::create_directory(fulldata_dir(), file_utils_t::DO_RECURSION);
	}
	// read the fullmeta_dir variable
	if( btcli_conf.contain_key("fullmeta_dir") ){
		m_fullmeta_dir	= btcli_conf.get_first_value("fullmeta_dir");		
	}else{	// if not present, set it to a default in home dir and create it if needed
		m_fullmeta_dir	= file_utils_t::get_home_dir() / ".neoip-btcli" / "completed";
		file_utils_t::create_directory(fullmeta_dir(), file_utils_t::DO_RECURSION);
	}
	// read the partdata_dir variable
	if( btcli_conf.contain_key("partdata_dir") ){
		m_partdata_dir	= btcli_conf.get_first_value("partdata_dir");		
	}else{	// if not present, set it to a default in home dir and create it if needed
		m_partdata_dir	= file_utils_t::get_home_dir() / ".neoip-btcli" / "incoming";
		file_utils_t::create_directory(partdata_dir(), file_utils_t::DO_RECURSION);
	}
	// read the partmeta_dir variable
	if( btcli_conf.contain_key("partmeta_dir") ){
		m_partmeta_dir	= btcli_conf.get_first_value("partmeta_dir");		
	}else{	// if not present, set it to a default in home dir and create it if needed
		m_partmeta_dir	= file_utils_t::get_home_dir() / ".neoip-btcli" / "incoming";
		file_utils_t::create_directory(partmeta_dir(), file_utils_t::DO_RECURSION);
	}
	

/*
 * Check the existance of the full/part meta/data dir
 */
	if( file_stat_t(fulldata_dir()).is_null() )	return bt_err_t(bt_err_t::ERROR, fulldata_dir().to_string() + " doesnt exist.");
	if( file_stat_t(fullmeta_dir()).is_null() )	return bt_err_t(bt_err_t::ERROR, fullmeta_dir().to_string() + " doesnt exist.");
	if( file_stat_t(partdata_dir()).is_null() )	return bt_err_t(bt_err_t::ERROR, partdata_dir().to_string() + " doesnt exist.");
	if( file_stat_t(partmeta_dir()).is_null() )	return bt_err_t(bt_err_t::ERROR, partmeta_dir().to_string() + " doesnt exist.");

/*
 * Read all the lookmeta_dir variables
 */	
	// read all the "lookmeta_dir" key from the btcli_conf
	size_t	keyidx	= strvar_db_t::INDEX_NONE;
	while(true){
		// get the next "lookmeta_dir" key
		keyidx	= btcli_conf.next_key_idx("lookmeta_dir", keyidx);
		// if none is remaining, leave the loop
		if( keyidx == strvar_db_t::INDEX_NONE )	break;
		// convert the value into a file_path_t
		file_path_t	dirname	= btcli_conf[keyidx].val();
		// if the convertion failed, return an error
		if( dirname.is_null() ){
			std::string	reason = "can not convert \"" + OSTREAMSTR(btcli_conf[keyidx]) + "\" lookmeta_dir in a file_path_t";
			return bt_err_t(bt_err_t::ERROR, reason);
		}
		// log to debug
		KLOG_DBG("add " << dirname << " as lookmeta_dir");
		// put the value into the lookmeta_dir_arr
		lookmeta_dir_arr	+= dirname;
	}


/*
 * Check the existance of all the lookmeta_dir
 */
	// check that there is at least one lookmeta_dir
	if( lookmeta_dir_arr.size() == 0 ){
		std::string	reason = "No lookmeta_dir in " + config_path.to_string() + "! At least one MUST be specified.";
		return bt_err_t(bt_err_t::ERROR, reason);
	}
	// go thru all the lookmeta_dir file_path_t to checkk their existance
	for(size_t i = 0; i < lookmeta_dir_arr.size(); i++){
		file_path_t &	lookmeta_dir	= lookmeta_dir_arr[i];
		if( file_stat_t(lookmeta_dir).is_null() )
			return bt_err_t(bt_err_t::ERROR, lookmeta_dir.to_string() + " doesnt exist.");
	}
	

	// return no error
	return bt_err_t::OK;
}

/** \brief Launch the bt_ezsession_t
 */
bt_err_t	btcli_apps_t::launch_ezsession()	throw()
{
	const strvar_db_t &	session_conf	= lib_session_get()->session_conf();
	file_path_t		config_path	= lib_session_get()->conf_rootdir() / "btcli";
	strvar_db_t		btcli_conf	= strvar_helper_t::from_file(config_path / "neoip_btcli.conf");
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

// TODO the gui assume that both are ALWAYS setup
// - so if the parameter is not set in the configuration file, use infinite
//   - the maxrate infinit is not properly handled in the rate_sched_t 
//   - TODO to review
// - moreover this is a changing dynamically, so should not be in a static configuration file
// - a dynamic configuration file should be handled in this neoip-btcli
// - it has been coded fast more as an experiement, now (mar07) it is possible
//   to have a gui, do something more real
	// if btcli_conf contain xmit_maxrate, set it up
	if( btcli_conf.contain_key("xmit_maxrate") ){
		size_t maxrate	= string_t::to_uint32(btcli_conf.get_first_value("xmit_maxrate"));
		ezsession_opt	|= bt_ezsession_opt_t::XMIT_RSCHED;
		ezsession_profile.xmit_maxrate( maxrate );
	}
	// if btcli_conf contain recv_maxrate, set it up
	if( btcli_conf.contain_key("recv_maxrate") ){
		size_t maxrate	= string_t::to_uint32(btcli_conf.get_first_value("recv_maxrate"));
		ezsession_opt	|= bt_ezsession_opt_t::RECV_RSCHED;
		ezsession_profile.recv_maxrate( maxrate );
	}

	// start the bt_ezsession_t;
	m_bt_ezsession	= nipmem_new bt_ezsession_t();
	bt_err		= m_bt_ezsession->profile(ezsession_profile).start(ezsession_opt, tcp_listen_aview
							, udp_listen_aview, kad_peerid_t::build_random()
							, bt_id_t::build_peerid("azureus", "1.2.3.4"));
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;	
}


/** \brief Scan the partmeta_dir and launch the btcli_swarm_t with them if not already launched
 */
bt_err_t	btcli_apps_t::launch_from_partmeta_dir()	throw()
{
	file_dir_t	file_dir;
	file_err_t	file_err;
	bt_err_t	bt_err;
	// open the lookmeta_dir file_dir_t
	file_err	= file_dir.open(partmeta_dir());
	// if it failed, log the event and return an error
	if( file_err.failed() ){
		KLOG_ERR("Can't open directory " << partmeta_dir() << " due to " << file_err);
		return bt_err_from_file(file_err);
	}
	// filter the file_dir_t to keep only the *.resumedata
	file_dir.filter(file_dir_t::filter_glob_nomatch("*.torrent"));
	// launch one btcli_swarm_t with each of the resulting resumedata
	for( size_t i = 0; i < file_dir.size(); i++ ){
		file_path_t	metadata_basename	= file_dir.basename(i);
		// if there is already a btcli_swarm_t for this metadata_basename, goto the name
		if( swarm_by_basename(metadata_basename) )	continue;
		// launch the btcli_swarm_t for this file_path_t
		bt_err	= launch_swarm(metadata_basename);
		if( bt_err.failed() )	return bt_err;
	}
	// return no error
	return bt_err_t::OK;
}


/** \brief Launch a btcli_swarm_t with a metadata_basename
 */
bt_err_t	btcli_apps_t::launch_swarm(const file_path_t &metadata_basename)	throw()
{
	btcli_swarm_t *	btcli_swarm;
	bt_err_t	bt_err;
	// log to debug
	KLOG_ERR("enter");
	// sanity check - no other btcli_swarm_t MUST have this metadata_basename
	DBG_ASSERT( swarm_by_basename(metadata_basename) == NULL );

	// to launch the btcli_swarm_t with this bt_mfile now
	btcli_swarm	= nipmem_new btcli_swarm_t();
	bt_err		= btcli_swarm->start(this, metadata_basename);
	if( bt_err.failed() ){
		nipmem_delete	btcli_swarm;
		return bt_err;
	}
	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	btcli_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --nodaemon cmdline option
	clineopt	= clineopt_t("nodaemon", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("To keep the daemon in front");
	clineopt.alias_name_db().append("d");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}

/** \brief Return a pointer on the btcli_swarm_t which matches metadata_basename
 * 
 * - if none matches, return NULL
 */
btcli_swarm_t *	btcli_apps_t::swarm_by_basename(const file_path_t &metadata_basename)	const throw()
{
	std::list<btcli_swarm_t *>::const_iterator	iter;
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		btcli_swarm_t *	btcli_swarm	= *iter;
		// if the metadata_basename of this btcli_swarm_t doesnt match, goto the next
		if( btcli_swarm->metadata_basename() != metadata_basename )	continue;
		// if it matches, return its pointer
		return btcli_swarm;
	}
	// if this point is reached, no btcli_swarm_t matches, so return NULL
	return NULL;
}

/** \brief Return a pointer on the btcli_swarm_t which matches infohash
 * 
 * - if none matches, return NULL
 */
btcli_swarm_t *	btcli_apps_t::swarm_by_infohash(const bt_id_t &infohash)	const throw()
{
	std::list<btcli_swarm_t *>::const_iterator	iter;
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		btcli_swarm_t *	btcli_swarm	= *iter;
		// if the infohash of this btcli_swarm_t doesnt match, goto the next
		if( btcli_swarm->infohash() != infohash )	continue;
		// if it matches, return its pointer
		return btcli_swarm;
	}
	// if this point is reached, no btcli_swarm_t matches, so return NULL
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			lookmeta_dir stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Scan all the lookmeta_dir and copy any metadata file into the partmeta_dir
 */
bt_err_t	btcli_apps_t::lookmeta_dir_scan()	throw()
{
	file_dir_t				file_dir;
	file_err_t				file_err;
	datum_t					torrent_datum;
	// go thru all the lookmeta_dir file_path_t
	for(size_t i = 0; i < lookmeta_dir_arr.size(); i++){
		file_path_t &	lookmeta_dir	= lookmeta_dir_arr[i];
		// open the lookmeta_dir file_dir_t
		file_err	= file_dir.open(lookmeta_dir);
		// if it failed, log the event and goto the next
		if( file_err.failed() ){
			KLOG_ERR("Can't open directory " << lookmeta_dir << " due to " << file_err);
			continue;
		}
		// filter the file_dir_t to keep only the *.torrent
		file_dir.filter(file_dir_t::filter_glob_nomatch("*.torrent"));
		
		// loop on all the remaining elements
		for(size_t i = 0; i < file_dir.size(); i++){
			// determine the file_path_t
			file_path_t	src_path	= file_dir.fullpath(i);
			file_path_t	dst_path	= partmeta_dir() / src_path.basename();
			// read the content pointed by src_path
			file_err	= file_sio_t::readall(src_path, torrent_datum);
			if( file_err.failed() )	return bt_err_from_file(file_err);
			// copy the src_path into the dst_path
			file_err	= file_sio_t::writeall(dst_path, torrent_datum);
			if( file_err.failed() )	return bt_err_from_file(file_err);
			// remove the src_path
			file_err	= file_utils_t::remove_file(src_path);
			if( file_err.failed() )	return bt_err_from_file(file_err);
		}
	}
	// return no error
	return bt_err_t::OK;
}

/** \brief callback called when the timeout_t expire
 */
bool btcli_apps_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_WARN("enter");
	// do a initial lookmeta_dir_scan()
	bt_err	= lookmeta_dir_scan();
	if( bt_err.failed() ){
		KLOG_ERR("Periodic lookmeta_dir_scan() returned error due to " << bt_err);
		return true;
	}		
	// do the launch_from_partmeta_dir()
	bt_err	= launch_from_partmeta_dir();
	if( bt_err.failed() ){
		KLOG_ERR("Periodic launch_swarm_look() returned error due to " << bt_err);
		return true;
	}
			
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END

