/*! \file
    \brief Header of the lib_apps_t
    
*/


#ifndef __NEOIP_LIB_APPS_HPP__ 
#define __NEOIP_LIB_APPS_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_lib_apps_profile.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_apps_type.hpp"
#include "neoip_clineopt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	libsess_err_t;
class	clineopt_arr_t;
class	clineopt_err_t;

/** \brief init the layers and load the config file
 */
class lib_apps_t : NEOIP_COPY_CTOR_DENY {
private:
	lib_apps_profile_t	profile;	//!< the profile for this object
	
	/*************** apps info	***************************************/
	std::string		m_canon_name;	//!< the apps canonical name
	std::string		m_human_name;	//!< the apps human-readable name
	std::string		m_version;	//!< the apps version
	std::string		m_summary;	//!< the short summary of the apps purpose
	std::string		m_longdesc;	//!< the long description of the apps purpose
	apps_type_t		m_apps_type;	//!< the apps_type_t for this apps

	std::string		m_exec_name;	//!< the executable name argv[0]
	strvar_db_t		m_arg_option;	//!< the cmdline arg which are options
	std::vector<std::string> m_arg_remain;	//!< the remaining of the cmdline args
	
	/*************** internal function	*******************************/
	clineopt_arr_t	neutral_clineopt_arr()					throw();
	clineopt_err_t	parse_cmdline(int argc, char **argv, const clineopt_arr_t &app_clineopt_arr) throw();
	clineopt_err_t	parse_cmdline_from_envvar(const clineopt_arr_t &clineopt_arr) 	throw();
	void		display_help(const clineopt_arr_t &clineopt_arr)	throw();
	void		display_version()					throw();
public:
	/*************** ctor/dtor	***************************************/
	lib_apps_t()			throw();
	~lib_apps_t()			throw();

	/*************** setup function	***************************************/
	lib_apps_t &	set_profile(const lib_apps_profile_t &profile)			throw();
	libsess_err_t	start(int argc, char **argv, const clineopt_arr_t &apps_clineopt_arr
				, const std::string &m_canon_name, const std::string &m_human_name
				, const std::string &m_version, const std::string &m_summary
				, const std::string &m_longdesc
				, const apps_type_t &m_apps_type)		throw();
	
	/*************** query function	***************************************/
	const std::string &		canon_name()	const throw()	{ return m_canon_name;		}
	const std::string &		human_name()	const throw()	{ return m_human_name;		}
	const std::string &		version()	const throw()	{ return m_version;		}
	const std::string &		summary()	const throw()	{ return m_summary;		}
	const std::string &		longdesc()	const throw()	{ return m_longdesc;		}
	const apps_type_t &		apps_type()	const throw()	{ return m_apps_type;		}

	const std::string &		exec_name()	const throw()	{ return m_exec_name;		}
	const strvar_db_t &		arg_option()	const throw()	{ return m_arg_option;		}
	const std::vector<std::string> &arg_remain()	const throw()	{ return m_arg_remain;		}
	const lib_apps_profile_t &	get_profile()	const throw()	{ return profile;		}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_APPS_HPP__  */



