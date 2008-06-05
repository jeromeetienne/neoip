/*! \file
    \brief Definition of the \ref btcli_swarm_t class

- TODO this is quite chaotic
  - to clean up
- TODO make a .deb for this

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

*/

/* system include */
/* local include */
#include "neoip_btcli_swarm.hpp"
#include "neoip_btcli_apps.hpp"
#include "neoip_btcli_profile.hpp"
#include "neoip_bt_err.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezswarm_share.hpp"

#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_bt_swarm_resumedata_helper.hpp"
#include "neoip_bt_mfile_helper.hpp"

#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit.hpp"
#include "neoip_file.hpp"
#include "neoip_file_path_arr.hpp"
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
btcli_swarm_t::btcli_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	btcli_apps	= NULL;
	bt_ezswarm	= NULL;
	lib_session_exit= NULL;
	// put some default value
	is_full		= false;
}
	
/** \brief Destructor
 */
btcli_swarm_t::~btcli_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from btcli_apps_t if needed
	if( btcli_apps )	btcli_apps->swarm_unlink(this);
	// delete the bt_ezswarm_t
	nipmem_zdelete	bt_ezswarm;
	// delete the lib_session_exit_t if needed
	nipmem_zdelete	lib_session_exit;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	btcli_swarm_t::start(btcli_apps_t *btcli_apps
				, const file_path_t &m_metadata_basename)	throw()
{
	const btcli_profile_t &	profile	= btcli_apps->profile();
	bt_err_t		bt_err;
	// copy the parameter
	this->btcli_apps		= btcli_apps;
	this->m_metadata_basename	= m_metadata_basename;
	// link this object to the btcli_apps_t
	btcli_apps->swarm_dolink(this);
	
	// start the lib_session_exit_t
	lib_session_exit	= nipmem_new lib_session_exit_t();
	lib_session_exit->start(lib_session_get(), lib_session_exit_t::EXIT_ORDER_BT_SWARM
					, this, NULL);

	// start the resumedata_timeout
	resumedata_timeout.start(profile.resumedata_autosave_period(), this, NULL);
	
	// launch the bt_ezswarm_t
	bt_err	= launch_ezswarm();
	if( bt_err.failed() )	return bt_err;
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_ezswarm_t
 */
bt_err_t	btcli_swarm_t::launch_ezswarm()	throw()
{
	bt_ezsession_t *	bt_ezsession	= btcli_apps->bt_ezsession();
	bt_err_t		bt_err;
	bt_swarm_resumedata_t	swarm_resumedata;

	// load the bt_swarm_resumedata if available, else load the metadata_basename
	file_path_t resumedata_path	= resumedata_path_from_metadata(metadata_basename());
	if( !file_stat_t(resumedata_path).is_null() ){
		swarm_resumedata= bt_swarm_resumedata_helper_t::from_file(resumedata_path);
		DBG_ASSERT( !swarm_resumedata.is_null() );	// TODO poor error management	
	}else{
		file_path_t	metadata_fullpath = btcli_apps->partmeta_dir() / metadata_basename();
		bt_mfile_t	bt_mfile;
		bt_mfile	= bt_mfile_helper_t::from_torrent_file(metadata_fullpath);
		DBG_ASSERT( !bt_mfile.is_null() );	// TODO poor error management
		// add the local_path in the bt_mfile.subfile_arr()
		bt_mfile.set_file_local_dir(btcli_apps->partdata_dir());

		// complete the initialization fo the bt_mfile
		bt_mfile.complete_init();
		// build the swarm_resumedata from the bt_mfile_t
		swarm_resumedata	= bt_swarm_resumedata_t::from_mfile(bt_mfile);		
	}
	
	// sanity check - swarm_resumedata MUST be check().succeed()
	DBG_ASSERT( swarm_resumedata.check().succeed() );

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
	if( !swarm_resumedata.bt_mfile().announce_uri().is_null() )
		ezswarm_opt |= bt_ezswarm_opt_t::PEERSRC_HTTP;

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

/** \brief Return the infohash for this btcli_swarm_t
 */
const bt_id_t &	btcli_swarm_t::infohash()		const throw()
{
	const bt_mfile_t & bt_mfile	= bt_ezswarm->swarm_resumedata().bt_mfile();
	return bt_mfile.infohash();
}

/** \brief Execute a command from the GUI
 */
void	btcli_swarm_t::do_command(const std::string &command)	throw()
{
	// log to debug
	KLOG_ERR("command=" << command);
	
	// TODO halfbacked stat automata
	if( command == "stop" ){
		if( bt_ezswarm->in_share() ){
			bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::STOPPED);
		}else{
		}
	}else if( command == "start" ){
		if( bt_ezswarm->in_stopped() ){
			bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::SHARE);
		}else{
		}
	}else{
		KLOG_ERR("received unknown command " << command);
		DBG_ASSERT( 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Move the files from the partdata_dir/partmeta_dir to the fulldata_dir/fullmeta_dir
 * 
 * - this copy the metadata_basename and all the data files from the dir_part to the dir_full
 * - this functions return an error immediatly - aka no support for error
 * - NOTE: it avoid any race condition aka it remove the source files IIF the destination files
 *   have been successfully copied
 * - TODO currently the copy is done only by hardlink aka it requires to have the 
 *   part_dir on the same partition as the dir_part
 *   - additionnaly it is only on some filesystem and OS
 *   - later support actual copy with a asyncronous function
 * - TODO sometime the copy do not happen for unknown reason
 *   - investiage the matter
 *   - likely would be nicer to put this file copy into the neoip-_file
 *   - and have a nunit for it
 *   - something similar to the ruby FileUtils
 */
bt_err_t	btcli_swarm_t::move_part2full_dir()	throw()
{
	const file_path_t &	partmeta_dir	= btcli_apps->partmeta_dir();
	const file_path_t &	partdata_dir	= btcli_apps->partdata_dir();
	const file_path_t &	fullmeta_dir	= btcli_apps->fullmeta_dir();
	const file_path_t &	fulldata_dir	= btcli_apps->fulldata_dir();
	file_path_arr_t		subpath_arr;
	file_err_t		file_err;
	// if bt_ezswarm current state is NOT share, return now
	DBG_ASSERT( bt_ezswarm->in_stopped() );

/*
 * STEP 0: get all the data file subpath in subpath_arr
 */
	// reload the bt_mfile_t from metadata_basename() to populate the subpath_arr
	file_path_t	metadata_fullpath	= partmeta_dir / metadata_basename();
	bt_mfile_t	bt_mfile		= bt_mfile_helper_t::from_torrent_file(metadata_fullpath);
	DBG_ASSERT( !bt_mfile.is_null() );	
	// go thru the whole bt_mfile_t.subfile_arr()
	// - TODO should be in bt_mfile_t itself
	//   - something similar is in bt_alloc_t
	for(size_t i = 0; i < bt_mfile.subfile_arr().size(); i++){
		bt_mfile_subfile_t &mfile_subfile	= bt_mfile.subfile_arr()[i];
		file_path_t	file_path;
		// if this is a multi-file bt_mfile_t, the name() is the top most directory name
		if( bt_mfile.subfile_arr().size() > 1 )	file_path	/= bt_mfile.name();
		// add the mfile_path
		file_path	/= mfile_subfile.mfile_path();
		// add this file_path into the subpath_arr
		subpath_arr	+= file_path;
	}

/*
 * STEP 1: create all the directory in the destination if needed
 * - it is needed IIF the bt_mfile_t has several file
 */
	if( bt_mfile.subfile_arr().size() > 1 ){
		// create all the destination directories in the fulldata_dir
		for(size_t i = 0; i < subpath_arr.size(); i++){
			file_path_t	dirname	= fulldata_dir / subpath_arr[i].dirname();
			file_stat_t	file_stat( dirname );
			// if the dirname already exist and is a directory, goto the next
			if( !file_stat.is_null() && file_stat.is_dir() )	continue;
			// try to reccursively create the directory
			file_err	= file_utils_t::create_directory(dirname, file_utils_t::DO_RECURSION);
			if( file_err.failed() )	return bt_err_from_file(file_err);
		}
	}
	

/*
 * STEP 2: copy the files from the source to the destination, meta and data
 */
	// copy the metadata_basename
	file_err	= file_utils_t::create_hardlink(partmeta_dir / metadata_basename(), fullmeta_dir / metadata_basename());
	if( file_err.failed() )	return bt_err_from_file(file_err);

	// copy the file themselves
	for(size_t i = 0; i < subpath_arr.size(); i++){
		file_path_t src_path	= partdata_dir / subpath_arr[i];
		file_path_t dst_path	= fulldata_dir / subpath_arr[i];
		// copy this file
		file_err	= file_utils_t::create_hardlink(src_path, dst_path);
		if( file_err.failed() )	return bt_err_from_file(file_err);
	}


/*
 * STEP 3: remove all the src files
 * - at this point, all the files have been successfully linked to the destination directories
 */
	// delete the metadata basename from the partmeta_dir
	file_err = file_utils_t::remove_file(partmeta_dir / metadata_basename());
	if( file_err.failed() )	return bt_err_from_file(file_err);
	// delete the resumedata from the partmeta_dir
	file_err = file_utils_t::remove_file(resumedata_path_from_metadata(metadata_basename()));
	if( file_err.failed() )	return bt_err_from_file(file_err);
	// delete all the data files in the partdata_dir
	if( bt_mfile.subfile_arr().size() > 1 ){
		file_err = file_utils_t::remove_directory(partdata_dir / bt_mfile.name(), file_utils_t::DO_RECURSION);
	}else{
		file_err = file_utils_t::remove_file(partdata_dir / bt_mfile.name());
	}
	if( file_err.failed() )	return bt_err_from_file(file_err);
 
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_swarm_resumedata_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the resumedata_path from a metadata_path
 */
file_path_t	btcli_swarm_t::resumedata_path_from_metadata(const file_path_t &metadata_path)	throw()
{
	// sanity check - the metadata_path MUST be a torrent
	// - NOTE: the function work anyway with any extension
	DBG_ASSERT( metadata_path.basename_ext() == "torrent" );
	// get the torrent_path basename and replace its extension with .resumedata
	std::string	basename	= metadata_path.basename_noext() + ".resumedata";
	// resumedata file are always stored in partmeta_dir
	file_path_t	resumedata_path	= btcli_apps->partmeta_dir() / basename;
	// return the just built resumedata_path
	return resumedata_path;	
}

/** \brief Load the resumedata if it is available
 */
bt_err_t	btcli_swarm_t::resumedata_save()	throw()
{
	// log to debug
	KLOG_DBG("save resumedata");
	// sanity check - the bt_ezswarm_state_t MUST be SHARE
	DBG_ASSERT( bt_ezswarm->in_share() );
	// save the bt_swarm_resume_data_t
	file_path_t	resumedata_path	= resumedata_path_from_metadata(metadata_basename());
	bt_swarm_t *	bt_swarm	= bt_ezswarm->share()->bt_swarm();
	bt_err_t	bt_err;
	bt_err	= bt_swarm_resumedata_helper_t::to_file(bt_swarm->get_current_resumedata(), resumedata_path);
	if( bt_err.failed() ){
		KLOG_ERR("Cant write the bt_swarm_resumedata_t in " << resumedata_path << " due to " << bt_err);
		return bt_err;
	}
	// return no error
	return bt_err_t::OK;
}

/** \brief callback called when the timeout_t expire
 */
bool btcli_swarm_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// if bt_ezswarm current state is bt_ezswarm_state_t::SHARE, save the bt_swarm_resumedata_t
	if( bt_ezswarm->in_share() )	resumedata_save();
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_ezswarm_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_ezswarm_t when to notify an event
 */
bool 	btcli_swarm_t::neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw()
{
	// log to debug
	KLOG_ERR("enter ezevent=" << ezswarm_event);

	// handle the bt_ezswarm_event_t depending of its type
	switch(ezswarm_event.get_value()){
	case bt_ezswarm_event_t::LEAVE_STATE_PRE:
			// if just leaving bt_ezswarm_state_t::SHARE, save the bt_swarm_resumedata_t
			if( bt_ezswarm->in_share() )			resumedata_save();
			break;
	case bt_ezswarm_event_t::ENTER_STATE_POST:
			// TODO what to do if it enter in_error() ?
	
			// if entered bt_ezswarm_state_t::SHARE, save the bt_swarm_resumedata_t
			if( bt_ezswarm->in_share() )			resumedata_save();
			// if entered in bt_ezswarm_state_t::STOPPED and is_full, move the files to datafull_dir/metafull_dir
			if( bt_ezswarm->in_stopped() && is_full ){
				// move the files from part to full
				bt_err_t bt_err	= move_part2full_dir();
				if( bt_err.failed() )	KLOG_ERR("Cant move file from part to full due to " << bt_err);
				// autodelete
				nipmem_delete	this;
				return false;
			}
			// if entered in bt_ezswarm_state_t::STOPPED, remove the lib_session_exit_t
			// - it is no more needed and will allow to leave the program if STOPPED has
			//   been triggered by lib_session_exit_t
			if( bt_ezswarm->in_stopped() )			nipmem_zdelete lib_session_exit;
			break;
	case bt_ezswarm_event_t::SWARM_EVENT:{
			DBG_ASSERT( bt_ezswarm->in_share() );
			bt_swarm_t * bt_swarm	= bt_ezswarm->share()->bt_swarm();
			// if the bt_swarm_t is a seed, goto STOPPING
			if( bt_swarm->is_seed() ){
				// mark this btcli_swarm_t as full
				is_full	= true;
				// make bt_ezswarm_t to gracefully goto STOPPED
				bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::STOPPED);
			}
			break;}
	default:	break;
	}

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			lib_session_exit_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref lib_session_exit_t when to notify an event
 */
bool	btcli_swarm_t::neoip_lib_session_exit_cb(void *cb_userptr, lib_session_exit_t &session_exit) throw()
{
	// log to debug
	KLOG_ERR("enter");
	
	// if bt_ezswarm_t is init and is in bt_ezswarm_state_t::SHARE, gracefully goto STOPPED
	if( bt_ezswarm && bt_ezswarm->in_share() ){
		bt_ezswarm->graceful_change_state(bt_ezswarm_state_t::STOPPED);
		return true;
	}

	// else delete the exit procedure
	nipmem_zdelete	lib_session_exit;
	// return dontkeep
	return false;
}
NEOIP_NAMESPACE_END

