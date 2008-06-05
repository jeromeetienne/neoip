/*! \file
    \brief Definition of the \ref file_oswarp_t class
    
*/

/* system include */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef	_WIN32
#	include <windows.h>
#	undef ERROR
#	undef INFINITE
#	undef OPTIONAL
#else
#	include <pwd.h>
#endif
/* local include */
#include "neoip_file_oswarp.hpp"
#include "neoip_file_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_mode.hpp"
#include "neoip_file_perm.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			mkdir
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Do a mkdir
 * 
 * - NOTE: required because window doesnt support file_perm_t in mkdir
 */
file_err_t	file_oswarp_t::mkdir(const file_path_t &dir_path, const file_perm_t &file_perm)
										throw()
{
#ifndef	_WIN32
	int	err = ::mkdir(dir_path.to_os_path_string().c_str(), file_perm.to_octal());
#else
	int	err = ::mkdir(dir_path.to_os_path_string().c_str());
#endif
	if( err )	return file_err_from_errno("Cant mkdir " + dir_path.to_os_path_string());
	// return no error
	return	file_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			mkdir
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Do a open() for a file
 * 
 * - NOTE: required because window doesnt support file_perm_t in open()
 * - WARNING: this is up to the caller to detect the error if any
 */
int	file_oswarp_t::open(const file_path_t &file_path, const file_mode_t &file_mode
				, const file_perm_t &file_perm)		throw()
{
	int	file_fd;
#ifndef	_WIN32
	file_fd	= ::open(file_path.to_os_path_string().c_str(), file_mode.to_open_flags()
					, file_perm.to_octal());
#else
	// under _WIN32 - explictly specify a O_BINARY
	file_fd	= ::open(file_path.to_os_path_string().c_str(), file_mode.to_open_flags()|O_BINARY
					, file_perm.to_octal());
#endif
	// return the resulting fd
	return file_fd;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the HOME directory
 */
file_path_t	file_oswarp_t::home_dir()					throw()
{
#ifndef _WIN32
	struct passwd	pw_stt;
	struct passwd *	pwbufp;
	char		buf[10*1024];
	// get the passwd struct for the current uid
	if( getpwuid_r(getuid(), &pw_stt, buf, sizeof(buf), &pwbufp) )	return file_path_t();
	// return the file_path_t of the home dir of the current user
	return file_path_t(pw_stt.pw_dir);
#else
	char *	tmp;
	// test HOMEPATH environment variable
	tmp	= getenv("HOMEPATH");
	if( tmp )	return tmp;
	// test HOME environment variable
	tmp	= getenv("HOME");
	if( tmp )	return tmp;
	// use directly the window syscall to get the system directory
	char	win_home[MAX_PATH];
	if( GetSystemDirectory(win_home, sizeof(win_home)) )
		return win_home;
	// log the error	
	KLOG_ERR("Failed to find any home directory");
	// return a null 
	return file_path_t();
#endif
}

NEOIP_NAMESPACE_END


