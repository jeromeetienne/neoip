/*! \file
    \brief Header of the \ref get_apps_t class

*/


#ifndef __NEOIP_GET_APPS_HPP__ 
#define __NEOIP_GET_APPS_HPP__ 
/* system include */
/* local include */
#include "neoip_get_profile.hpp"
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_bt_lnk2mfile_cb.hpp"
#include "neoip_bt_lnk2mfile_type.hpp"
#include "neoip_datum.hpp"
#include "neoip_timeout.hpp"
#include "neoip_lib_session_exit_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_err_t;
class	bt_ezsession_t;
class	file_path_t;
class	clineopt_arr_t;

/** \brief The object to run the neoip_get apps
 */
class get_apps_t : NEOIP_COPY_CTOR_DENY, private bt_lnk2mfile_cb_t, private bt_ezswarm_cb_t
				, private timeout_cb_t, private lib_session_exit_cb_t  {
private:
	std::string	link_type;
	std::string	link_addr;
	file_path_t	dest_dirpath;
	get_profile_t	profile;

	/*************** bt_lnk2mfile_t	*******************************/
	bt_lnk2mfile_t *	bt_lnk2mfile;
	bool 			neoip_bt_lnk2mfile_cb(void *cb_userptr, bt_lnk2mfile_t &cb_bt_lnk2mfile
					, const bt_err_t &bt_err
					, const bt_lnk2mfile_type_t &lnk2mfile_type
					, const bt_mfile_t &bt_mfile)		throw();

	bt_err_t		load_resumedata_link_addr()	throw();
	/*************** display function	*******************************/
	void			disp_param()			throw();
	void			disp_mfile()			throw();
	void			disp_found_resumedata()		throw();
	void			disp_leave_share()		throw();
	void			disp_ezswarm()			throw();
	std::string		status_str_alloc()		throw();
	std::string		status_str_check()		throw();
	std::string		status_str_share()		throw();
	std::string		status_str_stopping()		throw();
	std::string		status_str_stopped()		throw();
	
	/*************** fatal error notification	***********************/
	bool			fatal_error(const std::string &reason)	throw();
	bool			fatal_error(const bt_err_t &bt_err)	throw();

	/*************** bt_ezswarm_t stuff	*******************************/
	bt_swarm_resumedata_t	m_swarm_resumedata;
	date_t			enter_share_date;	//!< the present date when SHARE is entered
	file_size_t		enter_share_anyavail;	//!< the amount in anyavail when SHARE is entered
	
	/*************** bt_ezsession_t	***************************************/
	bt_ezsession_t *	bt_ezsession;
	bt_err_t		launch_ezsession()		throw();

	/*************** bt_ezswarm_t	***************************************/
	bt_ezswarm_t *		bt_ezswarm;
	bool 			neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw();
	bt_err_t		launch_ezswarm()		throw();
					
	/*************** resumedata stuff	*******************************/
	bool			resumedata_todelete;	//!< true if the resumedata file is to be deleted
	file_path_t		resumedata_filename()		const throw();
	bt_err_t		resumedata_load_if_avail()	throw();
	bt_err_t		resumedata_save()		throw();

	/*************** main timeout_t callback	***********************/
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** resumedata_timeout	*******************************/
	timeout_t		resumedata_timeout;
	bool 			resumedata_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	/*************** ezswarm_timeout	*******************************/
	timeout_t		display_timeout;
	bool 			display_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** lib_session_exit_t	*******************************/
	lib_session_exit_t *	lib_session_exit;
	bool			neoip_lib_session_exit_cb(void *cb_userptr
						, lib_session_exit_t &session_exit)	throw();
public:
	/*************** ctor/dtor	***************************************/
	get_apps_t()		throw();
	~get_apps_t()		throw();
	
	/*************** setup function	***************************************/
	get_apps_t &	set_profile(const get_profile_t &profile)		throw();
	bt_err_t	start()	throw();

	/*************** query function	***************************************/
	static clineopt_arr_t		clineopt_arr()		throw();
	const bt_swarm_resumedata_t &	swarm_resumedata()	const throw()	{ return m_swarm_resumedata;		}
	const bt_mfile_t &		bt_mfile()		const throw()	{ return swarm_resumedata().bt_mfile();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_GET_APPS_HPP__  */


 
