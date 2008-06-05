/*! \file
    \brief Class to handle the bt_alloc_t

\par Brief Description
bt_alloc_t handles the files allocation for a bt_mfile_t.

It has 2 bt_alloc_policy_t:
-# bt_alloc_policy_t::HOLE which create the file and just write a single byte (0)
  at the end.
  - it is very fast to allocate but assumes the underlying filesystem support
    sparse-file. see http://en.wikipedia.org/wiki/Sparse_file for details
-# bt_alloc_policy_t::FULL which create the file and fill the whole file with 0.
  - it is much slower than bt_alloc_policy_t::FULL but dont require to 
    have sparse-file support.
  - additionnaly, after the bt_alloc_t, the whole space is allocated on the disk.
    which allow to detect immediatly if the disk has enougth freespace to download
    the whole bt_mfile_t.     

It has 2 important properties:
- it is non destructive (aka no existing file is modified by it)
  - so no file_mode_t::TRUNCATE
- it is atomic. (aka cleans up in case of error) 
  - aka after the completion of bt_alloc_t, either all files are created, or none is
  - all that implemented in the created_paths queue


\par relation with bt_io_vapi_t
The current implementations assumes that the bt_swarm_t will use bt_io_sfile_t.
- TODO sort this one out
  - bt_alloc_t doesnt know which implementation of bt_io_vapi_t is used
  - there are no allocation to be made for bt_io_pfile_t anyway
  - current bt_alloc_t is mainly used thru bt_ezswarm_t... which knows 
    the bt_io_vapi_t via bt_ezswarm_opt_t


*/

/* system include */
/* local include */
#include "neoip_bt_alloc.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_utils.hpp"
#include "neoip_file_aio.hpp"
#include "neoip_file_awrite.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_alloc_t::bt_alloc_t()		throw()
{
	// zero some fields
	file_aio	= NULL;
	file_awrite	= NULL;	
	// set some field
	cur_file_idx	= 0;
	cur_file_off	= 0;
}

/** \brief Destructor
 */
bt_alloc_t::~bt_alloc_t()		throw()
{
	// delete the file_awrite_t if needed
	nipmem_zdelete	file_awrite;
	// delete the file_aio_t if needed
	nipmem_zdelete	file_aio;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_alloc_t &	bt_alloc_t::set_profile(const bt_alloc_profile_t &profile)	throw()
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
bt_err_t	bt_alloc_t::start(const bt_mfile_t &bt_mfile, const bt_alloc_policy_t &alloc_policy
				, bt_alloc_cb_t *callback, void *userptr)	throw()
{
	bt_err_t	bt_err;
	// sanity check - the bt_mfile_t MUST be fully_init
	DBG_ASSERT( bt_mfile.is_fully_init() );

	// copy the parameter
	this->bt_mfile		= bt_mfile;
	this->alloc_policy	= alloc_policy;
	this->callback		= callback;
	this->userptr		= userptr;

	// start the zerotimer_t to create/overwrite the local files of bt_mfile_t asyncronously
	zerotimer.append(this, NULL);
	
	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_alloc_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// TODO here handle the case of bt_io_pfile_t
	return launch_next_file();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Launch the processing of the next file
 * 
 * @return a tokeep for the bt_alloc_t
 */
bool	bt_alloc_t::launch_next_file()	throw()
{
	bt_err_t	bt_err;
	// if the cur_file_idx is equal to bt_mfile.subfile_arr.size(), notify a success
	if( cur_file_idx == bt_mfile.subfile_arr().size() )
		return notify_callback_succeed();

	// delete the file_aio_t if needed
	nipmem_zdelete	file_aio;

	// init some alias
	const bt_mfile_subfile_t &mfile_subfile	= bt_mfile.subfile_arr()[cur_file_idx];
	const file_path_t &	file_path	= mfile_subfile.local_path();

	// create the directory if needed
	bt_err	= create_dir_if_needed(file_path.dirname());
	if( bt_err.failed() )	return notify_callback_failed(bt_err);

	// log to debug
	KLOG_ERR("Start creating(not overwriting) file " << file_path << " in mode " << alloc_policy);

	// test if the the file_path already exists
	if( !file_stat_t(file_path).is_null() )
		return notify_callback_failed("path "+file_path.to_string()+" already exists.");

	// start a file_aio_t in WRITE|CREATE
	// - NOTE: do not file_mode_t::TRUNCATE to stay non destructive
	file_mode_t	file_mode = file_mode_t::WRITE | file_mode_t::CREATE;
	file_err_t	file_err;
	file_aio	= nipmem_new file_aio_t();
	file_err	= file_aio->start(file_path, file_mode);
	if( file_err.failed() )	return notify_callback_failed(bt_err_from_file(file_err));
	
	//  queue the file_path as it has been successfully created
	created_paths_add(file_path);

	// compute the first cur_file_off for this file, based on the bt_alloc_policy_t
	if( alloc_policy == bt_alloc_policy_t::FULL ){
		// if it is in bt_alloc_policy_t::FULL, write the file from the begining
		cur_file_off	= 0;
	}else{
		// if it is in bt_alloc_policy_t::HOLE, write only the last byte of the file
		cur_file_off	= mfile_subfile.len() - 1;
		// if mfile_subfile.len() is 0, it is a special case, cur_file_off is set to 0
		// - NOTE: launch_next_write() will handle it and wont write anything
		if( mfile_subfile.len() == 0 )	cur_file_off	= 0;
	}

	// launch the next write
	bool	tokeep	= launch_next_write();
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

/** \brief Launch the processing of the next write
 * 
 * @return a tokeep for the bt_alloc_t
 */
bool	bt_alloc_t::launch_next_write()	throw()
{
	const bt_mfile_subfile_t &mfile_subfile	= bt_mfile.subfile_arr()[cur_file_idx];
	file_err_t		file_err;
	// if the current file offset is >= than the current, launch the next file
	// - NOTE: works ok with files of size 0
	if( cur_file_off >= mfile_subfile.len() ){
		// increment the cur_file_idx
		cur_file_idx++;
		// launch the next file
		return launch_next_file();
	}

	// sanity check - file_awrite MUST be NULL
	DBG_ASSERT( file_awrite == NULL );

	// build the data2write based on filelength and profile
	file_size_t	datalen		= std::min(mfile_subfile.len() - cur_file_off, profile.write_chunk_maxlen());
	datum_t		data2write	= datum_t(datalen.to_size_t());
	// fill the data2write with 0 - may be removed as no code expect it to be 0
	// - NOTE: it consume a bit of CPU but provide 'cleaner' files thus helps detecting stuff
	memset( (char*)data2write.get_ptr(), 0, data2write.size());

	// log to debug
	KLOG_DBG("launch on " << mfile_subfile.local_path() << " a awrite of " << data2write.size()
				<< " at the offset " << cur_file_off);

	// launch the bt_awrite_t for the next chunk
	file_awrite	= nipmem_new file_awrite_t(file_aio);
	file_err	= file_awrite->start(cur_file_off, data2write, this, NULL);
	if( file_err.failed() )	return notify_callback_failed(bt_err_from_file(file_err));
	
	// update the cur_file_off
	cur_file_off	+= data2write.size();
	
	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        file_awrite_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_awrite_t when to notify the result of the operation
 */
bool	bt_alloc_t::neoip_file_awrite_cb(void *cb_userptr, file_awrite_t &cb_file_awrite
				, const file_err_t &file_err)	throw()
{
	// log to debug
	KLOG_DBG("enter file_err=" << file_err);

	// if the file_awrite_t failed, notify the error to the caller
	if( file_err.failed() )	return notify_callback_failed(bt_err_from_file(file_err));	

	// delete the file_awrite_t
	nipmem_zdelete	file_awrite;

	// relaunch for the next write
	bool	tokeep	= launch_next_write();
	if( !tokeep )	return false;

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          utility function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Create the directories if needed (this is reccursive)
 */
bt_err_t	bt_alloc_t::create_dir_if_needed(const file_path_t &dir_path)	throw()
{
	file_stat_t	file_stat;
	file_err_t	file_err;
	// go thru each subpath of the dir_path
	// - NOTE: do a custom recursive create_directory to queue all created_paths
	for(size_t i = 0; i < dir_path.size(); i++){
		file_path_t 	subpath	= dir_path.subpath(i);
		file_stat_t	file_stat(subpath);
		// if this level doesnt exist, try to create it
		if( file_stat.is_null() ){
			// create this directory
			// - NOTE: with no recursion to be sure all created_paths are queued
			file_err = file_utils_t::create_directory(subpath, file_utils_t::NO_RECURSION);
			if( file_err.failed() )	return bt_err_from_file(file_err);
			//  queue the file_path as it has been successfully created
			created_paths_add(subpath);
		}else if( !file_stat.is_dir() ){
			// if this level already exists, but it IS NOT a dir, report an error
			return bt_err_t(bt_err_t::ERROR, subpath.to_string() + " already exists and it not a directory");
		}
	}
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			created_paths queue management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a file_path_t to the created_paths queue
 * 
 * - allow it to be removed cleaned up if an error occurs during bt_alloc_t
 * - it contains all the successfully created file_path_t (regular files and directory)
 */
void	bt_alloc_t::created_paths_add(const file_path_t &file_path)	throw()
{
	created_paths.push_back(file_path);
}

/** \brief delete all file_path_t in the created_paths queue
 * 
 * - goes on as much as possible even in case of error
 *   - as it happen in a destructor, one can not report an error
 *   - error are logged tho
 */
void	bt_alloc_t::created_paths_cleanup()				throw()
{
	file_stat_t	file_stat;
	file_err_t	file_err;
	// go thru the whole created_paths
	while( !created_paths.empty() ){
		// get the last created_paths
		file_path_t file_path	= created_paths.back();
		// remove it from the queue
		created_paths.pop_back();
		// do a file_stat_t to determine if it is a directory or a regular file
		file_err	= file_stat.open(file_path);
		if( file_err.failed() ){
			KLOG_ERR("Cant file_stat_t("<< file_path <<") due to " << file_err);
			continue;
		}
		// log to debug
		KLOG_DBG("cleanup " << file_path << " as " << (file_stat.is_dir() ? "directory" : "regular file"));
		// remove the file_path_t (different operation if directory or file)
		if(file_stat.is_dir())	file_err = file_utils_t::remove_directory(file_path);
		else			file_err = file_utils_t::remove_file(file_path);
		// log the error if it occured
		if( file_err.failed() )	KLOG_ERR("Cant remove(" << file_path <<") due to " << file_err); 
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller when bt_alloc_t succeed
 */
bool	bt_alloc_t::notify_callback_succeed()				throw()
{
	return notify_callback(bt_err_t::OK);
}

/** \brief notify the caller when bt_alloc_t failed with a std::string
 */
bool	bt_alloc_t::notify_callback_failed(const std::string &reason)	throw()
{
	// NOTE: call notify_callback_failed to have the created_paths_cleanup
	return notify_callback_failed(bt_err_t(bt_err_t::ERROR, reason) );
}

/** \brief notify the caller when bt_alloc_t failed with a bt_err_t 
 */
bool	bt_alloc_t::notify_callback_failed(const bt_err_t &bt_err)	throw()
{
	// cleanup all the created_paths
	created_paths_cleanup();
	// notify the error to the caller
	return notify_callback(bt_err);
}


/** \brief notify the callback with the tcp_event
 */
bool bt_alloc_t::notify_callback(const bt_err_t &bt_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_alloc_cb(userptr, *this, bt_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END





