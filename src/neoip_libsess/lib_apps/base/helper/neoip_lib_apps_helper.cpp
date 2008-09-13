/*! \file
    \brief Definition of the \ref lib_apps_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_lib_apps_helper.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_file.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			misc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief deamonize the application
 * 
 * - NOTE: it changes the current pid 
 *   - so if the pidfile_write/remove functions have to be called, it MUST be done
 *     AFTER the daemonize()
 * 
 * @return true if an error occurs, false otherwise
 */
libsess_err_t	lib_apps_helper_t::daemonize()	throw()
{
	// pass in daemon but with nochdir
	// - with nochdir to keep all the file_path_t valid
	// - without noclose to release fd on stdin/stdout/stderr
#ifndef _WIN32
	if( daemon(1, 0) )	return libsess_err_t(libsess_err_t::ERROR, "Cant demonize due to " + neoip_strerror(errno));
#else
	EXP_ASSERT(0);
#endif
	// make the logging happen in syslog instead of tty
	// - TODO unsure it is the proper place to do those changes
	get_global_log_layer()->output_tty	( false	);
	get_global_log_layer()->output_syslog	( true	);
	// return noerror
	return libsess_err_t::OK;
}

/** \brief Return the current pid
 */
size_t	lib_apps_helper_t::current_pid()	throw()
{
	pid_t	pid	= getpid();
	return pid;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			pidfile create/remove
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a file_path_t to store the pidfile
 */
file_path_t	lib_apps_helper_t::pidfile_path()	throw()
{
	lib_session_t *	lib_session	= lib_session_get();
	lib_apps_t *	lib_apps	= lib_session->lib_apps();
	// sanity check - only apps_type_t::*_BOOT are allowed to save pidfile
	DBG_ASSERT( lib_apps->apps_type().is_boot() );
	// return the file_path_t
	return lib_session->run_rootdir() / (lib_apps->canon_name() + ".pid");
}

/** \brief write a pidfile
 */
void	lib_apps_helper_t::pidfile_create()		throw()
{
	// file the file_path_t for the pidfile
	file_path_t	file_path	= pidfile_path();
	// sanity check - file_path MUST NOT be null
	DBG_ASSERT( !file_path.is_null() );
	// log to debug
	KLOG_ERR("Write pidfile into " << file_path);
	// save the file
	file_err_t	file_err;
	file_err	= file_sio_t::writeall(file_path, datum_t(OSTREAMSTR(current_pid())));
	if( file_err.failed() )
		KLOG_ERR("Failed to write the pidfile at " << file_path	<< " due to " << file_err);
}

/** \brief remove a pidfile
 */
void	lib_apps_helper_t::pidfile_remove()		throw()
{
	// file the file_path_t for the pidfile
	file_path_t	file_path	= pidfile_path();
	// sanity check - file_path MUST NOT be null
	DBG_ASSERT( !file_path.is_null() );
	// remove the file
	file_err_t	file_err;
	file_err	= file_utils_t::remove_file(file_path);
	if( file_err.failed() )
		KLOG_ERR("Failed to remove the pidfile at " << file_path << " due to " << file_err);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			url file
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the file_path_t for the url
 */
file_path_t	lib_apps_helper_t::urlfile_path()				throw()
{
	std::ostringstream	oss;
	// build the file_path
	oss << lib_session_get()->temp_rootdir().to_string();
	oss << "/.neoip_debug_url." << current_pid() << ".http_url";
	// return the just built file_path_t
	return oss.str();
}

/** \brief Create the url file
 */
libsess_err_t	lib_apps_helper_t::urlfile_create()				throw()
{
	lib_httpd_t *	httpd		= lib_session_get()->get_httpd();
	std::string	url_str		= "http://" + httpd->get_listen_addr().to_string();
	file_perm_t	file_perm	= file_perm_t::USR_RW_ | file_perm_t::GRP_RW_ | file_perm_t::OTH_RW_;
	// write the file
	file_err_t	file_err;
	file_err	= file_sio_t::writeall(urlfile_path(), datum_t(url_str), file_perm);
	if( file_err.failed() )	return libsess_err_from_file(file_err);
	// return no error
	return libsess_err_t::OK;	
}

/** \brief remote the url file
 */
libsess_err_t	lib_apps_helper_t::urlfile_remove()				throw()
{
	file_err_t	file_err;
	file_err	= file_utils_t::remove_file(urlfile_path());
	if( file_err.failed() ){
		KLOG_ERR("Cant remove the config url file called " << urlfile_path());
		return libsess_err_from_file(file_err);
	}
	// return no error
	return libsess_err_t::OK;
}



NEOIP_NAMESPACE_END


