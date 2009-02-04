/*! \file
    \brief Declaration of the \ref log_layer_t

*/


#ifndef __NEOIP_LOG_LAYER_HPP__ 
#define __NEOIP_LOG_LAYER_HPP__ 
/* system include */
#include <map>
#include <stdio.h>
/* local include */
#include "neoip_log_level.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	strvar_db_t;
class	log_layer_t;
class	file_path_t;

// function declaration for ctor/get
log_layer_t *	get_global_log_layer()		throw();

/** \brief layer to allocate logory in neoip
 */
class log_layer_t : NEOIP_COPY_CTOR_ALLOW {
public:
	typedef	std::map<std::string, log_level_t>	category_db_t;	
private:
	log_level_t	default_level;		//!< default log_level_t when not found within pattern
	bool		m_output_tty;		//!< true if the log MUST be done on tty
	bool		m_output_syslog;	//!< true if the log MUST be done on syslog
	bool		m_disp_log_src;		//!< true if the source of the log MUST be logged
	bool		m_disp_log_time;	//!< true if the time of the log MUST be logged
	bool		syslog_started;		//!< true if the syslog have been started
	category_db_t	category_pattern;	//!< the list of category pattern 
	category_db_t	category_cache;		//!< the list of category cache
	
	bool		m_output_file;		//!< true if the log MUST be done in a file
	std::string	m_output_file_path;	//!< the file_path_t for the log file
	FILE *		fOut_file;		//!< descriptor of the file containing logs
	
	/*************** internal function	*******************************/
	log_level_t	create_cache_entry(const std::string &category_name)	throw();	
public:
	/*************** ctor/dtor	***************************************/
	log_layer_t(const log_level_t &default_level)	throw();
	~log_layer_t()					throw();

	/*************** query function	***************************************/
	log_level_t	find_category_level(const char *category_name)		throw();	
	bool		output_tty()		const throw()	{ return m_output_tty;		}
	bool		output_syslog()		const throw()	{ return m_output_syslog;	}
	bool		output_file()		const throw()	{ return m_output_file;		}
	bool		disp_log_src()		const throw()	{ return m_disp_log_src;	}
	bool		disp_log_time()		const throw()	{ return m_disp_log_time;	}

	/*************** action function	*******************************/
	void		set_default_level(const log_level_t &default_level)	throw();
	void		set_config_varfile(const strvar_db_t &strvar_db)	throw();
	void		set_output_file_path(const file_path_t &file_path)	throw();
	void		do_log(const char *filename, int lineno, const char *fct_name
				, const char *category_name, log_level_t log_level
				, const std::string &str )			throw();
	void		output_tty(bool new_val)				throw();
	void		output_syslog(bool new_val)				throw();
	void		output_file(bool new_val)				throw();
	void		disp_log_src(bool new_val)				throw();
	void		disp_log_time(bool new_val)				throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LOG_LAYER_HPP__  */



