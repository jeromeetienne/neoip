/*! \file
    \brief Header of the lib_session_t
    
*/


#ifndef __NEOIP_LIB_SESSION_HPP__ 
#define __NEOIP_LIB_SESSION_HPP__ 

/* system include */
#include <string>
#include <list>
#include <map>
/* local include */
#include "neoip_lib_session_init.hpp"
#include "neoip_file_path.hpp"
#include "neoip_zerotimer_cb.hpp"
#include "neoip_timeout_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	lib_apps_t;
class	libsess_err_t;
class	lib_session_exit_t;
class	strvar_db_t;
class	eloop_t;
class	event_hook_t;
class	event_hook_cb_t;
class	lib_httpd_t;
class	clineopt_arr_t;
class	lib_session_profile_t;

/** \brief init the layers and load the config file
 */
class lib_session_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t, public timeout_cb_t {
public:	/////////////////////// constant declaration ///////////////////////////
	enum hook_level_t {
		HOOK_NONE,
		HOOK_POST_HTTPD_INIT,
		HOOK_PRE_HTTPD_DEINIT,
		HOOK_MAX,
	};	
private:
	lib_apps_t *	m_lib_apps;		//!< backpointer on the lib_apps_t
	eloop_t *	eloop;			//!< the eloop_t on which this session run
	strvar_db_t *	m_session_conf;		//!< store the configuration of the session
	lib_session_profile_t *profile;		//!< the profile for this object

	event_hook_t *	event_hook;		//!< handle the lib_session hooks
						//!< (see neoip_lib_session::hook_level_t)	
	lib_httpd_t *	lib_httpd;		//!< the httpd associated with this session


	/*************** directory stuff	*******************************/
	file_path_t	m_conf_rootdir;	//!< rootdir for common library config - readonly directory
	file_path_t	m_temp_rootdir;		//!< rootdir for dynfile file deleted between reboot
	file_path_t	m_cache_rootdir;	//!< rootdir for dynfile file kept between reboot
	file_path_t	m_run_rootdir;		//!< rootdir for dynfile files which are info about
						//!<  the running system since last boot 
	file_path_t	m_log_rootdir;		//!< rootdir for log files 
	libsess_err_t	rootdir_init()		throw();
	static libsess_err_t	rootdir_create_if_needed(const file_path_t &dir_path)	throw();

	/*************** exit_zerotimer	***************************************/
	zerotimer_t *	exit_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** exit_timeout stuff	*******************************/
	timeout_t *	exit_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** lib_session_exit_t	*******************************/
	typedef	std::multimap<size_t, lib_session_exit_t *>	session_exit_db_t;
	session_exit_db_t	session_exit_db;
	void 		exit_dolink(lib_session_exit_t *session_exit) 	throw();
	void 		exit_unlink(lib_session_exit_t *session_exit)	throw();
	void		launch_next_exit_order()			throw();
public:
	/*************** ctor/dtor	***************************************/
	lib_session_t()		throw();
	~lib_session_t()	throw();

	/*************** setup function	***************************************/
	lib_session_t &	set_profile(const lib_session_profile_t &profile)	throw();
	libsess_err_t 	start(lib_apps_t *lib_apps)				throw();
	void	event_hook_append(int level_no, event_hook_cb_t *callback, void *userptr) 	throw();
	void	event_hook_remove(int level_no, event_hook_cb_t *callback, void *userptr) 	throw();

	/*************** query function	***************************************/
	const file_path_t &	conf_rootdir()		const throw()	{ return m_conf_rootdir;	}
	const file_path_t &	temp_rootdir()		const throw()	{ return m_temp_rootdir;	}
	const file_path_t &	cache_rootdir()		const throw()	{ return m_cache_rootdir;	}
	const file_path_t &	run_rootdir()		const throw()	{ return m_run_rootdir;		}
	const file_path_t &	log_rootdir()		const throw()	{ return m_log_rootdir;		}

	const strvar_db_t &	session_conf()		const throw()	{ return *m_session_conf;	}
	const lib_session_profile_t &get_profile()	const throw()	{ return *profile;		}
	lib_httpd_t *		get_httpd()		throw()		{ return lib_httpd;		}
	lib_apps_t *		lib_apps()		throw()		{ return m_lib_apps;		}
	static clineopt_arr_t	clineopt_arr()		throw();
	bool			loop_stopping()		const throw();

	/*************** action function	*******************************/
	void			loop_run()		throw();
	void			loop_stop_now()		throw();
	void			loop_stop_asap()	throw();
	
	/*************** list of friend class	*******************************/
	friend class	lib_session_exit_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_SESSION_HPP__  */



