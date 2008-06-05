/*! \file
    \brief Implementation of the lib_session_t

\par Subdirectory meaning
- conf_rootdir
  - may be shared by several neoip application
  - persistance: irrelevant as it is read-only and provided by external means 
  - goal: contains configuration for neoip application
  - access control: read only
- temp_rootdir
  - may be shared by several applications
  - persistance: it erased between run
  - goal: contains data temporary to a single run of the apps
  - access control: read/write
- cache_rootdir
  - is not shared by several application
  - persistence: may be erased between run but should not under normal circonstance
  - goal: contains data which may be usefull to a next run
  - access control: read/write
- run_rootdir
  - goal: contains 'administrative' data e.g. pidfile
  - shared by several application
  - persistence: erased between run
  - access control: read/write
- log_rootdir
  - shared by several application
  - goal: contains the log file
  - persistence: may be erased between run 
  - access control: write only
  
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_profile.hpp"
#include "neoip_lib_session_exit.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_eloop.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_event_hook.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_nipmem_tracker.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_timeout.hpp"
#include "neoip_clineopt_arr.hpp"

#include "neoip_log_layer.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_strvar_helper.hpp"
#include "neoip_file.hpp"

#include "neoip_crypto_layer_init.hpp"
#include "neoip_compress_layer_init.hpp"
#include "neoip_udp_layer_init.hpp"
#include "neoip_socket_layer_udp_init.hpp"
#include "neoip_socket_layer_ntlay_init.hpp"
#include "neoip_ndiag_watch.hpp"
#include "neoip_ndiag_err.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief contructor of the lib_session_t
 */
lib_session_t::lib_session_t() throw()
{
	// allocate the default profile
	profile		= nipmem_new lib_session_profile_t();
	// allocate the event loop 
	// - NOTE: dont use nipmem_new not to disturb the nipmem tracker
	eloop		= nipmem_new eloop_t();

	// allocate the exit timers after the eloop_t
	exit_zerotimer	= nipmem_new zerotimer_t();
	exit_timeout	= nipmem_new timeout_t();
}

/** \brief destructor of the lib_session_t
 */
lib_session_t::~lib_session_t() throw()
{
#if 1
	// deinit the ndiag_watch_t 
	ndiag_watch_deinit();
#endif

	// close all pending lib_session_exit_t
	// - TODO: totally unsure about this one
	//   - the session_exit_db are lib_session_exit allocated elsewhere...
	//   - why do i delete them here ?
	while( !session_exit_db.empty() )	nipmem_delete session_exit_db.begin()->second;

	// notify that the httpd will be deinig
	event_hook->notify(HOOK_PRE_HTTPD_DEINIT);
	// delete the httpd associated with this session
	if( lib_httpd )	nipmem_delete lib_httpd;


// TODO bug if the lib_session_t hasent been started, layers are deinit without being init
// - to add a _is_init() in the layer_init.cpp would be good
	// deinit the socket layer
	socket_layer_ntlay_deinit();
	socket_layer_udp_deinit();

	// deinit the udp layer
	udp_layer_deinit();
	// deinit the crypto layer
	compress_layer_deinit();
	// deinit the crypto layer
	crypto_layer_deinit();
	
	// delete the exit_zerotimer and exit_timeout
	nipmem_zdelete	exit_zerotimer;
	nipmem_zdelete	exit_timeout;

	
	// delete the session_conf
	nipmem_zdelete	m_session_conf;
	// delete the event_hook
	nipmem_zdelete	event_hook;

	// delete the eloop_t
	nipmem_zdelete	eloop;
	
	// delete the lib_session_profile_t
	nipmem_zdelete	profile;
	
	// display a summary of the nipmem_tracker
	get_global_nipmem_tracker()->display_summary();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			start function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
lib_session_t &	lib_session_t::set_profile(const lib_session_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check().succeed() );	
	// delete the previous profile if needed
	nipmem_zdelete	this->profile;
	// copy the parameter
	this->profile	= nipmem_new lib_session_profile_t(profile);
	// return the object iself
	return *this;
}


/** \brief start the session
 * 
 * @return false if no error occured, true otherwise
 */
libsess_err_t lib_session_t::start(lib_apps_t *p_lib_apps)	throw()
{	
	libsess_err_t	libsess_err;
	// copy the parameter
	this->m_lib_apps	= p_lib_apps;
	
	// init all the rootdir
	libsess_err	= rootdir_init();
	if( libsess_err.failed() )	return libsess_err;
	
	// load the session conf
	m_session_conf	= nipmem_new strvar_db_t();
	*m_session_conf	= strvar_helper_t::from_file(conf_rootdir() / "neoip_session.conf");

	// init the event hook
	event_hook	= nipmem_new event_hook_t(HOOK_MAX);

	// set the output_filepath in the log_layer_t
	std::string	logfile_basename	= lib_apps()->canon_name() + ".log";
	get_global_log_layer()->set_output_file_path(log_rootdir() / logfile_basename );

	// set the config_varfile within the global log_layer_t
	strvar_db_t	log_conf;
	log_conf	= strvar_helper_t::from_file(conf_rootdir() / "neoip_log.conf");
	get_global_log_layer()->set_config_varfile(log_conf);
	
	// init the crypto layer
	crypto_layer_init();
	// init the compress layer
	compress_layer_init();
	// init the udp layer
	udp_layer_init();
	// init the socket layer
	socket_layer_udp_init();
	socket_layer_ntlay_init();

	// init httpd associated with this session
	lib_httpd	= nipmem_new lib_httpd_t(this);
	bool failed	= lib_httpd->start();
	if( failed )	return libsess_err_t(libsess_err_t::ERROR, "Cant start lib_httpd_t");
	event_hook->notify(HOOK_POST_HTTPD_INIT);

#if 1
	// init the ndiag_watch_t - MUST be after the lib_httpd_t to register the wikidbg
	// - TODO is this requirement on wikidbg true ?
	ndiag_watch_init();
	// build the ndiag_watch_profile_t
	ndiag_watch_profile_t	watch_profile	= get_profile().ndiag_watch();
	// if session_conf contains a use_upnp set to false,
	// - NOTE: trick rather kludgy to disable upnp from the lib_session.conf 
	std::string	use_upnp_str	= session_conf().get_first_value("use_upnp" ,"yes");
	bool		use_upnp	= string_t::convert_to_bool(use_upnp_str);
	watch_profile.enable_upnp_watch(use_upnp);
	// start the ndiag_watch_t
	ndiag_err_t	ndiag_err;
	ndiag_err	= ndiag_watch_get()->set_profile(watch_profile).start();
	if( ndiag_err.failed() )	return libsess_err_t(libsess_err_t::ERROR, ndiag_err.to_string());
#endif
	// return no error
	return libsess_err_t::OK;
}

/** \brief Initialize all the rootdir
 */
libsess_err_t	lib_session_t::rootdir_init()		throw()
{
	const strvar_db_t &	apps_option	= lib_apps()->arg_option();

/* NOTE ABOUT directory revamping
 * - if -c is explicitly set, you do all inside this directory
 *   - put all the rootdir within this custom rootdir, thus all the files
 *     are localized within this custom_dir which is the desired behaviour
 *     for the caller.
 * 
 * - e.g. neoip-oload which is a USR_BOOT apps
 *   - ~/.config/neoip-oload for the config dir itself, in read-only
 *   - ~/.cache/neoip-oload for the m_cache_rootdir
 *   - what about the temp and run directory ?
 *     - both should be writable by user because the apps run as user
 *     - run_dir should be erased at boot/login time for self-healing
 *     - it should not conflict if 2 users uses it at the same time
 *   - the run_dir should be constant in order to be found by external programms
 *     - typically to get the pidfile 
 *     - ~/.var/run/neoip-oload.pid ?

 * - look at http://standards.freedesktop.org/basedir-spec/basedir-spec-0.6.html
 *   - should it respect the XDG env var ?
 * 
 * TODO:
 * - should i check/create directory in this?
 *   - either that or i rely on the installer to do it for me
 *   - what is the best solution
 *   - all the directory which are not mandatory to contains files should do that
 *     - e.g. do it for all but conf_rootdir
 *     - test if it exists and is_dir with file_stat_t, do nothing
 *     - if not, try to create them recursivly, if creation failed, return error
 *  
 * - clean up this mess 
 *   - remove the default for libconf_roodir
 *   - is this prgconf_rootdir usefull ? is it just used ? i bet not
 *     - if not remove it. i check this is not used
 * - rename conf_rootdir in conf_rootdir ?
 * - currently i write the log in /tmp directory
 *   - i should not :)
 *   - have a log_rootdir
 *   - e.g. neoip-router should write in /var/log/neoip_router.log
 */

	// if conf_rootdir() is explicitly set, put all stuff below it 
	if( apps_option.contain_key("config-dir") ){
		m_conf_rootdir	= apps_option.get_first_value("config-dir");
		m_temp_rootdir	= conf_rootdir() / "tmp";
		m_cache_rootdir	= conf_rootdir() / "cache";
		m_run_rootdir	= conf_rootdir() / "tmp";
		m_log_rootdir	= conf_rootdir() / "tmp";
	}else if( lib_apps()->apps_type().is_user() ){
		// if the lib_apps_t::appstype() is "user"
		file_path_t  home_dir	= file_utils_t::get_home_dir();
		m_conf_rootdir	= home_dir / ".config" / lib_apps()->canon_name();
		m_temp_rootdir	= "/tmp";
		m_cache_rootdir	= home_dir / ".cache" / lib_apps()->canon_name();
		m_run_rootdir	= home_dir / ".var" / "run";
		m_log_rootdir	= home_dir / ".var" / "log";
		// if this user has NO user-specific config directory, try the system one
		if( file_stat_t(conf_rootdir() / "neoip_session.conf").is_null() )
			m_conf_rootdir	= file_path_t("/etc") / lib_apps()->canon_name();		
	}else if( lib_apps()->apps_type().is_system() ){
		// if the lib_apps_t::appstype() is "system"
		m_conf_rootdir	= file_path_t("/etc") / lib_apps()->canon_name();
		m_temp_rootdir	= "/tmp";
		m_cache_rootdir	= file_path_t("/var/cache") / lib_apps()->canon_name();
		m_run_rootdir	= "/var/run";
		m_log_rootdir	= "/var/log";
	}else {
		DBG_ASSERT(0);
	}
	
	// if conf_rootdir() / "neoip_session.conf" doesnt exist, return an error
	file_path_t	sessconf_path	= conf_rootdir() / "neoip_session.conf";
	if( file_stat_t(sessconf_path).is_null() )
		return libsess_err_t(libsess_err_t::ERROR, sessconf_path.to_os_path_string() + " is not present");
	
	// create all dir with are not mandatorily containing file
	// - aka all but conf_rootdir
	libsess_err_t	libsess_err;
	libsess_err	= rootdir_create_if_needed(temp_rootdir());
	if( libsess_err.failed() )	return libsess_err;
	libsess_err	= rootdir_create_if_needed(cache_rootdir());
	if( libsess_err.failed() )	return libsess_err;
	libsess_err	= rootdir_create_if_needed(run_rootdir());
	if( libsess_err.failed() )	return libsess_err;
	libsess_err	= rootdir_create_if_needed(log_rootdir());
	if( libsess_err.failed() )	return libsess_err;
	
	// return no error
	return libsess_err_t::OK;
}

/** \brief create the dir_path if needed
 */
libsess_err_t	lib_session_t::rootdir_create_if_needed(const file_path_t &dir_path)	throw()
{	
	// test if the dir_path exists and is a directory, return now
	file_stat_t	file_stat	= file_stat_t(dir_path);
	if( !file_stat.is_null() ){
	 	if( file_stat.is_dir() )	return libsess_err_t::OK;
		return libsess_err_t(libsess_err_t::ERROR, dir_path.to_os_path_string() + " exists BUT is not a directory.");	 	
	}

	// try to create the dir_path
	file_err_t	file_err;
	file_err	= file_utils_t::create_directory(dir_path, file_utils_t::DO_RECURSION);
	if( file_err.failed() )	return libsess_err_from_file(file_err);

	// return no error
	return libsess_err_t::OK;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	lib_session_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --config-dir cmdline option
	clineopt	= clineopt_t("config-dir", clineopt_mode_t::REQUIRED)
						.option_mode(clineopt_mode_t::OPTIONAL)
						.help_string("Specify the directory for the library configuration files");
	clineopt.alias_name_db().append("c");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////..////////////////////////////


/** \brief run the loop 
 */
void	lib_session_t::loop_run()	throw()
{
	eloop->loop_run();
}

/** \brief stop the loop on the next iteration of the event loop
 */
void	lib_session_t::loop_stop_now()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// stop the eloop loop 'immediatly' - after this iteration
	eloop->loop_stop();
}

/** \brief Return true if the loop is currently stopping, false otherwise
 */
bool	lib_session_t::loop_stopping()						const throw()
{
	// if the exit_timeout is running, the loop is considered as 'stopping'
	if( !exit_zerotimer->empty() || exit_timeout->is_running() )	return true;
	// else return false
	return false;
}

/** \brief Function to be called when the user whish to interrupt
 * 
 * - there is a zerotimer_t before triggering the stopping
 * - so it should not be expected to stop on the next iteration of the event loop
 *   - if it is what is desired, use loop_stop_now()
 */
void	lib_session_t::loop_stop_asap()	throw()
{
	// if already in stopping, do nothing
	if( loop_stopping() )	return;
	// launch exit_zerotimer to notify all the lib_session_exit_t
	exit_zerotimer->append(this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timer callback for lib_session_exit_t stuff 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	lib_session_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_ERR("enter session_exit_db.size()=" << session_exit_db.size());
	KLOG_ERR("Start stopping");

	// sanity check - the exit_timeout MUST NOT be running
	DBG_ASSERT( !exit_timeout->is_running() );
	// start the exit_timeout_t
	exit_timeout->start(profile->exit_expire_delay(), this, NULL);

	// launch all the next batch of lib_session_exit_t	
	launch_next_exit_order();

	// return tokeep
	return true;
}

/** \brief callback called when the timeout_t expire
 */
bool	lib_session_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// log the event
	KLOG_ERR("Lib session stopping timed out. panic time :)");
	// stop the exit_timeout
	exit_timeout->stop();
	// stop the event loop
	eloop->loop_stop();
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			launch_next_exit_order
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the next patch of lib_session_exit_t with the lowest exit_order
 * 
 * - assume that no lib_session_exit_t in session_exit_db are in_exiting()
 * - NOTE: this function may call eloop->loop_stop();
 */
void	lib_session_t::launch_next_exit_order()			throw()
{
	// log to debug
	KLOG_DBG("enter session_exit_db.size()=" << session_exit_db.size());
	// if there are no lib_session_exit, interrupt the loop now
	if( session_exit_db.empty() ){
		KLOG_ERR("stopping ended gracefully");
		eloop->loop_stop();
		return;
	}

	// get the lowest exit_order from the first lib_session_exit_t in session_exit_db
	size_t	first_exit_order	= session_exit_db.begin()->second->exit_order();
	// log to debug
	KLOG_DBG("first_exit_order="<< first_exit_order);
	
	// notify all the lib_session_exit_t
	session_exit_db_t::iterator	iter;
	for(iter = session_exit_db.begin(); iter != session_exit_db.end(); iter++ ){
		lib_session_exit_t *	session_exit	= iter->second;
		// if this session_exit order is different from the first one in database
		if(session_exit->exit_order() != first_exit_order)	break;
		// sanity check - session_exit MUST NOT be in_exiting()
		DBG_ASSERT( session_exit->in_exiting() == false );
		// log to debug
		KLOG_DBG("begin_exiting with exit_order=" << session_exit->exit_order());
		// notify this lib_session_exit_t *async* after a zerotimer_t
		session_exit->begin_exiting();
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			lib_session_exit dolink/unlink
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief dolink a lib_session_exit_t to this lib_session_t
 */
void	lib_session_t::exit_dolink(lib_session_exit_t *session_exit) 	throw()
{
	// add it in the session_exit_db
	session_exit_db.insert(std::make_pair(session_exit->exit_order(), session_exit));
}

/** \brief unlink a lib_session_exit_t from this lib_session_t
 */
void	lib_session_t::exit_unlink(lib_session_exit_t *session_exit)	throw()
{
	size_t	session_exit_order	= session_exit->exit_order();
	// log to debug
	KLOG_DBG("enter exit_order=" << session_exit_order << " session_exit_db.size()=" << session_exit_db.size());
	// find the lib_session_exit_t in the session_exit_db
	session_exit_db_t::iterator	iter;
	for(iter = session_exit_db.begin(); iter->second != session_exit && iter != session_exit_db.end(); iter++);
	// sanity check - session_exit MUST have been found
	DBG_ASSERT( iter != session_exit_db.end() );
	// remove this session_exit from session_exit_db
	session_exit_db.erase(iter);

	// if exit_timeout is not running, return now
	if( !exit_timeout->is_running() )	return;
	// if there is still a lib_session_exit_t in_exiting in the database, return now
	// - aka wait until it is completed before launching the next batch
	if( !session_exit_db.empty() && session_exit_db.begin()->second->in_exiting() ){
		// sanity check - the exit_order of the current and the db one MUST be equal
		DBG_ASSERT(session_exit_order == session_exit_db.begin()->second->exit_order());
		return;
	}
	// launch all the next batch of lib_session_exit_t	
	launch_next_exit_order();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			hook event
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief hook an event in \ref lib_session_t
 */
void lib_session_t::event_hook_append(int level_no, event_hook_cb_t *callback, void *userptr) throw()
{
	event_hook->append( level_no, callback, userptr );
}

/** \brief unhook an event in \ref lib_session_t
 */
void lib_session_t::event_hook_remove(int level_no, event_hook_cb_t *callback, void *userptr) throw()
{
	event_hook->remove( level_no, callback, userptr );
}


NEOIP_NAMESPACE_END


