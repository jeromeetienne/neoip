/*! \file
    \brief definition of the \ref log_layer_t

*/

/* system include */
#include <iostream>
#ifndef _WIN32
#	include <syslog.h>
#endif
/* local include */
#include "neoip_log_layer.hpp"
#include "neoip_date.hpp"
#include "neoip_date_helper.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                global stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//! pointer on the global log_layer_t - note that it is never freed
static log_layer_t *	global_log_layer_ptr		= NULL;

/** \brief return a point on the global log_layer_t
 */
log_layer_t *get_global_log_layer()	throw()
{
	// if the global_log_layer_ptr is not yet created, create it now
	if( !global_log_layer_ptr ){
		global_log_layer_ptr	= new log_layer_t(log_level_t::INFO);
		DBG_ASSERT( global_log_layer_ptr );
	}
	// return the global_log_layer_ptr
	return global_log_layer_ptr;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
log_layer_t::log_layer_t(const log_level_t &default_level)		throw()
{
	// zero some variables
	this->fOut_file		= NULL;
	// copy the parameter
	this->default_level	= default_level;
	// zero some variable
	this->syslog_started	= false;
	// set from default parameter
	output_tty	( true 	);
	output_syslog	( false	);
	output_file	( false	);
	disp_log_src	( true	);
	disp_log_time	( false	);
}

/** \brief Destructor
 */
log_layer_t::~log_layer_t()		throw()
{
#ifndef _WIN32
	// if syslog has been started, close it now
	if( syslog_started )	closelog();
#endif
	// the output_file is open, close it now
	if( fOut_file )		fclose(fOut_file);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           find_category_level
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the log level of a category
 */
log_level_t log_layer_t::find_category_level(const char *category_name)		throw()
{
	category_db_t::iterator	iter;
	log_level_t		log_level;

	// try to find thie category within the category_cache
	iter	= category_cache.find(category_name);
	
	// get the log_level_t from the cache if found, else create it
	if( iter != category_cache.end() ){
		// if the category has been found in the cache, get its log_level_t
		log_level = iter->second;
	}else{
		// if the category has NOT been found in the cache, create a cache entry for it
		log_level = create_cache_entry( category_name );
	}
	// return the category level
	return log_level;
}

/** \brief return the log level of a category
 */
log_level_t log_layer_t::create_cache_entry(const std::string &category_name)	throw()
{
	log_level_t			log_level = default_level;
	category_db_t::const_iterator	iter;
	// sanity check - the category_name MUST NOT be in the cache
	DBG_ASSERT( category_cache.find(category_name) == category_cache.end() );

	// try to find a matching pattern within the category_pattern
	for(iter = category_pattern.begin(); iter != category_pattern.end(); iter++){
		const std::string &	pattern	= iter->first;
		// if this pattern DOES match the category_name, leave the loop
		if( !string_t::glob_match(pattern, category_name) )	break;
	}
	// if a pattern has been found, set the log_level
	if( iter != category_pattern.end() )	log_level	= iter->second;

	// insert the new category within the category_cache
	bool	succeed	= category_cache.insert(std::make_pair(category_name, log_level)).second;
	DBG_ASSERT( succeed );

	// return the level
	return log_level;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  UTILITY function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief load a file containing property with log_level_t
 */
void	log_layer_t::set_config_varfile(const strvar_db_t &strvar_db)		throw()
{
	// reset the category_pattern
	category_pattern= category_db_t();
	// invalidate the cache - in case some entry no more match the config_varfile
	category_cache	= category_db_t();
	
	// go thru the whole strvar_db_t
	for(size_t i = 0; i < strvar_db.size(); i++ ){
		const std::string &	key	= strvar_db[i].key();
		const std::string &	val	= strvar_db[i].val();
		// if key = "output_tty", get the output_tty accordingly
		if( key == "output_tty" ){
			output_tty	( string_t::convert_to_bool(val) );
			continue;
		}
		// if key = "output_syslog", get the output_syslog accordingly
		if( key == "output_syslog" ){
			output_syslog	( string_t::convert_to_bool(val) );
			continue;
		}
		// if key = "output_file", get the output_file accordingly
		if( key == "output_file" ){
			output_file	( string_t::convert_to_bool(val) );
			continue;
		}
		// if key = "disp_log_src", get the disp_log_src accordingly
		if( key == "disp_log_src" ){
			disp_log_src	( string_t::convert_to_bool(val) );
			continue;
		}
		// if key = "disp_log_time", get the disp_log_src accordingly
		if( key == "disp_log_time" ){
			disp_log_time	( string_t::convert_to_bool(val) );
			continue;
		}
		// get the log_level_t from the val
		log_level_t		level	= log_level_t::from_string_nocase(val);
		// sanity check - the log_level_t MUST NOT be null
		DBG_ASSERT( !level.is_null() );
		// if key = "default_level", get the default_level accordingly
		if( key == "default_level" ){
			default_level	= level;
			continue;
		}
		// insert the new category within the category_pattern
		bool	succeed	= category_pattern.insert(std::make_pair(key, level)).second;
		DBG_ASSERT( succeed );
	}
}

/** \brief Set the default log_level_t
 */
void	log_layer_t::set_default_level(const log_level_t &default_level)	throw()
{
	// copy the parameter
	this->default_level	= default_level;
}

/** \brief Set the file_path_t of the log file 
 */
void	log_layer_t::set_output_file_path(const file_path_t &file_path)	throw()
{
	// sanity check - this MUST be done before setting on output_file()
	DBG_ASSERT( output_file() == false );
	// copy the parameter
	m_output_file_path	= file_path.to_os_path_string();
}
	
/** \brief Set the output_tty variable
 */
void	log_layer_t::output_tty(bool new_val)				throw()
{
	// copy the value
	m_output_tty	= new_val;
}

/** \brief Set the output_syslog variable
 */
void	log_layer_t::output_syslog(bool new_val)			throw()
{
	// copy the value
	m_output_syslog	= new_val;
#ifndef _WIN32
	// start the syslog if needed
	if( output_syslog() && !syslog_started ){
		lib_apps_t * lib_apps	= lib_session_get()->lib_apps();
		openlog(lib_apps->exec_name().c_str(), LOG_PID | LOG_CONS, LOG_DAEMON);
		syslog_started	= true;
	}
	// close the syslog if no more needed
	if( !output_syslog() && syslog_started ){
		closelog();
		syslog_started	= false;
	}
#endif
}

/** \brief Set the output_file variable
 */
void	log_layer_t::output_file(bool new_val)			throw()
{
	// copy the value
	m_output_file	= new_val;

	// start the fOut_file if needed
	if( output_file() && !fOut_file ){
		// open the file
		fOut_file	= fopen(m_output_file_path.c_str(), "a+");
		// make it unbuffered to be sure to be uptodate
		if( fOut_file )	{
			setbuf(fOut_file, NULL);
		}else{
			KLOG_STDERR("cant open " + m_output_file_path + " due to " + neoip_strerror(errno));
			m_output_file	= false;
		}
	}
	// close the fOut_file if no more needed
	if( !output_file() && fOut_file ){
		fclose( fOut_file );
		fOut_file	= NULL;
	}
}

/** \brief Set the disp_log_src vairable
 */
void	log_layer_t::disp_log_src(bool new_val)				throw()
{
	m_disp_log_src	= new_val;
}

/** \brief Set the disp_log_time vairable
 */
void	log_layer_t::disp_log_time(bool new_val)				throw()
{
	m_disp_log_time	= new_val;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			log function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief log a message 
 */
void 	log_layer_t::do_log(const char *filename, int lineno, const char *fct_name
				, const char *category_name, log_level_t log_level
				, const std::string &str)			throw()
{
	std::ostringstream	oss;
	// the category level is supposed to be checked in the macro for CPU efficiency purpose
	DBG_ASSERT( log_level <= find_category_level(category_name) );

	if( disp_log_time() ){
		// get the date for the 'present'
		date_t		date	= date_t::present();
		// convert it to a canonical_string
		oss << date_helper_t::to_canonical_string(date) << "|";
	}

	// if disp_log_src is set, prepend the log source
	if( disp_log_src() ){
		// extract the basename
		std::vector<std::string>	words		= string_t::split(filename, "/");
		std::string			basename	= words[words.size()-1];
		// prepend the date
		//oss << date_t::present().to_uint64() << "-";
		// prepend the log source
		oss << basename << ":" << lineno << ":" << fct_name << "() ";
	}
	// add the logged string itself
	oss << str;
	
	// if output_tty is set, display it
	if( output_tty() )	KLOG_STDERR(oss.str() << "\n");
#ifndef _WIN32
	// if output_syslog is set, send it to syslog
	if( output_syslog() )	syslog(log_level.to_syslog(), "%s", oss.str().c_str());
#endif
	// if output_file is set, append it to it
	if( output_file() )	fprintf(fOut_file, "%s\n", oss.str().c_str());
}


NEOIP_NAMESPACE_END







