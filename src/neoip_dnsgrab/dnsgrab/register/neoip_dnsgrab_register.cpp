/*! \file
    \brief Definition of the \ref dnsgrab_register_t class

- this object is rather simple to be externalized from dnsgrab_t
  - but the logic to determine the file_path_t is complex enougth
    to be externilized

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_dnsgrab_register.hpp"
#include "neoip_file.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of constants
#if 1
	const delay_t	dnsgrab_register_t::CFGFILE_REWRITE_PERIOD = delay_t::from_min(5*60);
#else	// the debug value
	const delay_t	dnsgrab_register_t::CFGFILE_REWRITE_PERIOD = delay_t::from_sec(10);
#endif
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
dnsgrab_register_t::dnsgrab_register_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
}

/** \brief Destructor
 */
dnsgrab_register_t::~dnsgrab_register_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");

	// remove the config file if needed
	if( !cfgfile_path.is_null() )	file_utils_t::remove_file(cfgfile_path);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         start function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 * 
 * @param location	== "user" if the registration is at the user level
 *                      == "system" if the registration is at the system level
 * 
 * @return false if no error occured, true otherwise
 */
dnsgrab_err_t	dnsgrab_register_t::start(const std::string &location, size_t priority
				, const ipport_addr_t &listen_addr, const delay_t &request_ttl
				, const std::string &tag, const std::string &nounce)	throw()
{
	file_path_t		dir_path;
	file_err_t		file_err;
	// sanity check - the location MUST be "user" or "system"
	DBG_ASSERT( location == "user" || location == "system" );
	// sanity check - the priority MUST be between 0 and 999 included
	DBG_ASSERT( priority >= 0 && priority <= 999 );
	
	// compute the dir_path
	if( location == "user" ){
		dir_path	= file_utils_t::get_home_dir() / ".libnss_neoip";
	}else{
		dir_path	= "/etc/libnss_neoip.d";
	}
	// log to debug
	KLOG_DBG("dir_path=" << dir_path );
	
	// check that dir_path exists
	file_stat_t	dir_stat	= file_stat_t(dir_path);
	// if the dir_path doesnt exist, try to create it
	if( dir_stat.is_null() ){
		// if the dir_path doesnt exist and the location is "user", create it
		if( location == "user" ){
			file_err	= file_utils_t::create_directory(dir_path, true);
			if( file_err.failed() )	return dnsgrab_err_from_file( file_err );
		}else{
			// if the dir_path doesnt exist and the location is "system", return an error
			return dnsgrab_err_t(dnsgrab_err_t::ERROR, dir_path.to_string() + " doesnt exists.");
		}
	}else if( !dir_stat.is_dir() ){
		// if the dir_path exists but is not a directory, report the error
		return dnsgrab_err_t(dnsgrab_err_t::ERROR, dir_path.to_string() + " is NOT a directory!");
	}

	// compute the basename
	std::ostringstream	oss;
	oss << "prio" << std::setfill('0') << std::setw(3) << priority << ".";
	if( !tag.empty() )	oss << tag;
	else			oss << "dnsgrab";
	oss	<< ".";
	if( !nounce.empty() )	oss << nounce;
	else			oss << rand();
	oss	<< ".cfg";

	// compute the final file_path with the dir_path and the basename
	cfgfile_path	= dir_path / oss.str();
	// log to debug
	KLOG_DBG("file_path=" << cfgfile_path);

	// compute the configuration string
	std::ostringstream	oss_line;
	oss_line << listen_addr.get_ipaddr()
			<< " " << listen_addr.get_port()
			<< " " << request_ttl.to_sec_32bit();
	config_str	= oss_line.str();
	// log to debug
	KLOG_DBG("config_str=" << config_str);

	// write the config file
	file_err	= write_cfgfile();
	if( file_err.failed() )	return dnsgrab_err_from_file(file_err);
	
	// start the rewrite_timeout
	rewrite_timeout.start(CFGFILE_REWRITE_PERIOD, this, NULL);

	// return no error
	return dnsgrab_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         write config file
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Write the config file
 * 
 * - NOTE: this is using the fopen() layer because im a looser and dont trust my own file layer
 */
file_err_t	dnsgrab_register_t::write_cfgfile()	throw()
{
	file_perm_t	file_perm = file_perm_t::USR_RW_ |file_perm_t::GRP_READ  | file_perm_t::OTH_READ;
	file_err_t	file_err;
	// write the whole file
	file_err	= file_sio_t::writeall(cfgfile_path, datum_t(config_str), file_perm);
	if( file_err.failed() )	return file_err;
	// return no error
	return file_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         rewrite_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	dnsgrab_register_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// rewrite the config file
	file_err_t	file_err	= write_cfgfile();
	// if the write failed, log the event
	if( file_err.failed() )	KLOG_ERR("Cant write the config file due to " << file_err);
	// return 'tokeep'
	return true;
}

NEOIP_NAMESPACE_END









