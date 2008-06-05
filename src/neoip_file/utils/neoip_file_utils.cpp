/*! \file
    \brief Implementation of \ref file_utils_t to read directories
 
*/

/* system include */
#include <sys/types.h>
#include <sys/stat.h>
/* local include */
#include "neoip_file_utils.hpp"
#include "neoip_file_oswarp.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_dir.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN;

// definition of \ref file_utils_t constant
const bool	file_utils_t::DO_RECURSION	= true;
const bool	file_utils_t::NO_RECURSION	= false;
// end of constants definition


/** \brief Return the current directory or a null file_path_t if case of error
 * 
 * @return a file_path_t of the result. it may be null in case of error
 */
file_path_t	file_utils_t::get_current_dir()	throw()
{
	char		buf[10*1024];
	// get the current working directory
	// - in case of faillure return a null file_path_t
	if( !getcwd(buf, sizeof(buf)) )	return file_path_t();
	// else return the file_path
	return file_path_t(buf);
}

/** \brief Return the home directory of the current user or a null file_path_t if case of error
 * 
 * @return a file_path_t of the result. it may be null in case of error
 */
file_path_t	file_utils_t::get_home_dir()	throw()
{
	// just forward to file_oswarp_t
	return file_oswarp_t::home_dir();
}

/** \brief Return a temporary path matching the path_pattern
 * 
 * - WARNING: this is based mktemp(3), so it suffer from the same problems
 * 
 * @return the resulting file_path_t
 */
file_path_t	file_utils_t::get_temp_path(const file_path_t &base_path)	throw()
{
	// build the pattern which match mktemp
	std::string	pattern = base_path.to_os_path_string() + "XXXXXX";
	char *		tmp	= (char *)nipmem_alloca(pattern.size() + 1);
	strcpy( tmp, pattern.c_str() );
	// log to debug
	KLOG_DBG("original pattern=" << pattern);
	// create the temporary name	
	char *		succeed	= mktemp(tmp);
	// sanity check - mktemp SHOULD never fails
	DBG_ASSERT( succeed );
	// log to debug
	KLOG_DBG("Returned result is=" << file_path_t(tmp).to_os_path_string());
	// return the result
	return file_path_t(tmp);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            create/remove directory
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Create a directory
 * 
 * @param dir_path	the directory path to create
 * @param recursive	true if the whole path MUST be created (similar to /bin/mkdir -p).
 * 			false if only the last name MUST be created (similar to plain /bin/mkdir)
 * @return a file_err_t of the result
 */
file_err_t	file_utils_t::create_directory(const file_path_t &dir_path, bool recursive
						, const file_perm_t &file_perm)	throw()
{
	file_err_t	file_err;
	// try to create the 'last name'
	file_err	= file_oswarp_t::mkdir(dir_path, file_perm);
	// if the creation succeed, return no error now
	if( file_err.succeed() )	return file_err_t::OK;
	// if the creation failed and it is not a recursive, report the error
	if( !recursive )		return file_err;
	
	// go thru all the level of dir_path
	for( size_t i = 0; i < dir_path.size(); i++ ){
		file_path_t	subpath	= dir_path.subpath(i);
		file_stat_t	file_stat(subpath);
		// if this level doesnt exist, try to create it
		if( file_stat.is_null() ){
			file_err	= file_oswarp_t::mkdir(subpath, file_perm);
			if( file_err.failed() )	return file_err;
		}else if( !file_stat.is_dir() ){
			// if this level already exists, but it IS NOT a dir, report an error
			return file_err_t(file_err_t::ERROR, "Cant mkdir " + dir_path.to_os_path_string() 
					+ " as " + subpath.to_os_path_string() + "is not a directory.");
		}
	}
	// sanity check - if this point is reached, the dir_path MUST be a valid directory
	DBG_ASSERT( file_stat_t(dir_path).is_dir() );
	// return no error	
	return file_err_t::OK;
}



/** \brief remove a directory
 * 
 * @param dir_path	the directory path to remove
 * @param recursive	true if all the files contained by the directory MUST be removed as well
 * @return a file_err_t of the result
 */
file_err_t	file_utils_t::remove_directory(const file_path_t &dir_path, bool recursive)	throw()
{
	// log to debug
	KLOG_DBG("enter dir_path=" << dir_path);
	// handle the recursive case
	if( recursive ){
		file_dir_t	file_dir;
		file_err_t	file_err;
		// open the directory dir_path
		file_err	= file_dir.open(dir_path);
		if( file_err.failed() )	return file_err;

		// remove all non directory files from the dir_path directory
		for(size_t i = 0; i < file_dir.size(); i++){
			// if it is a directory, skip it
			if( file_dir.get_file_stat(i).is_dir() )	continue;
			// log to debug
			KLOG_DBG("Try to remove file = " << file_dir[i]);
			// if it is not a directory, try to remove it
			file_err	= remove_file(file_dir[i]);
			if( file_err.failed() )	return file_err;		
		}
		// recurse in all subdirectory
		for(size_t i = 0; i < file_dir.size(); i++){
			// if it is NOT a directory, skip it
			if( !file_dir.get_file_stat(i).is_dir() )	continue;
			// if it is not a directory, try to remove it
			file_err	= remove_directory(file_dir[i], recursive);
			if( file_err.failed() )	return file_err;		
		}
	}
	// log to debug
	KLOG_DBG("Try to remove dir = " << dir_path);
	// remove the dir_path
	int	error 	= rmdir(dir_path.to_os_path_string().c_str());
	// if the removal failed, return error now
	if( error )	return file_err_from_errno("Cant mkdir " + dir_path.to_os_path_string());
	// return no error	
	return file_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			remove_file
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Remove a file
 *
 * - NOTE: on win32, the file MUST NOT be opened to be removed
 *  
 * @param file_path	the file path of the file to remove
 * @return		a file_err_t of the result
 */
file_err_t	file_utils_t::remove_file(const file_path_t &file_path)	throw()
{
	// log to debug
	KLOG_DBG("enter file_path=" << file_path.to_os_path_string());
	// remove the file
	int	err	= unlink(file_path.to_os_path_string().c_str());
	if( err )	return file_err_from_errno("Cant unlink() " + file_path.to_os_path_string());
	// return no error	
	return file_err_t::OK;
}

/** \brief Create a hardlink
 */
file_err_t	file_utils_t::create_hardlink(const file_path_t &src_path, const file_path_t &dst_path)	throw()
{
#ifndef	_WIN32
	int	err	= link(src_path.to_os_path_string().c_str(), dst_path.to_os_path_string().c_str());
	if( err )	return file_err_from_errno("Cant link() from " + src_path.to_os_path_string() + " to " + dst_path.to_os_path_string());
#else
	// TODO im not event sure hardlink exist on window :) and btw i dont think i use this :)
	EXP_ASSERT(0);
#endif
	// return no error	
	return file_err_t::OK;
}

/** \brief Create a hardlink
 */
file_err_t	file_utils_t::create_symlink(const file_path_t &src_path, const file_path_t &dst_path)	throw()
{
	// log to debug
	KLOG_DBG("try to do a symlink from " << src_path << " to " << dst_path);
#ifndef	_WIN32
	int	err	= symlink(src_path.to_os_path_string().c_str(), dst_path.to_os_path_string().c_str());
	if( err )	return file_err_from_errno("Cant symlink() from " + src_path.to_os_path_string() + " to " + dst_path.to_os_path_string());
#else
	// TODO im not event sure symlink exist on window :) and btw i dont think i use this :)
	EXP_ASSERT(0);
#endif
	// return no error	
	return file_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			get/set umask
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the current umask
 */
file_perm_t	file_utils_t::umask()						throw()
{
	// get the current umask
	mode_t	old_mode	= ::umask(0);
	// restore the current umask
	// - NOTE: this is needed because i failed to find any function able to read the umask
	//   only this one which set it
	::umask(old_mode);
	// return the file_perm_t for it
	return file_perm_t::from_octal(old_mode);
}
/** \brief set the umask to the file_perm_t
 */
file_err_t	file_utils_t::umask(const file_perm_t &file_perm)		throw()
{
	// set the umask
	::umask(file_perm.to_octal());
	// return no error
	return file_err_t::OK;
}

NEOIP_NAMESPACE_END



