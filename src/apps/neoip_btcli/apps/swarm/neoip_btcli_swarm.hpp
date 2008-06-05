/*! \file
    \brief Header of the \ref btcli_swarm_t class

*/


#ifndef __NEOIP_BTCLI_SWARM_HPP__ 
#define __NEOIP_BTCLI_SWARM_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_lib_session_exit_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	btcli_apps_t;
class	bt_err_t;
class	bt_id_t;

/** \brief The object to run the neoip_get apps
 */
class btcli_swarm_t : NEOIP_COPY_CTOR_DENY, private bt_ezswarm_cb_t, private lib_session_exit_cb_t
					, private timeout_cb_t {
private:
	btcli_apps_t *	btcli_apps;	//!< backpointer to the btcli_apps_t
	bool		is_full;	//!< true if the bt_swarm_t is fully locally available
	
	bt_err_t	move_part2full_dir()	throw();
	
	/*************** bt_ezswarm_t	***************************************/
	bt_ezswarm_t *	bt_ezswarm;
	bool 		neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw();
	bt_err_t	launch_ezswarm()						throw();

	/*************** bt_swarm_resumedata_t stuff	***********************/
	file_path_t	m_metadata_basename;
	timeout_t	resumedata_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	file_path_t	resumedata_path_from_metadata(const file_path_t &metadata_path)	throw();
	bt_err_t	resumedata_save()						throw();
	
	/*************** lib_session_exit_t	*******************************/
	lib_session_exit_t *lib_session_exit;
	bool		neoip_lib_session_exit_cb(void *cb_userptr, lib_session_exit_t &session_exit)	throw();
public:
	/*************** ctor/dtor	***************************************/
	btcli_swarm_t()			throw();
	~btcli_swarm_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(btcli_apps_t *btcli_apps, const file_path_t &m_metadata_basename)	throw();

	/*************** query function	***************************************/
	const file_path_t &	metadata_basename()	const throw()	{ return m_metadata_basename;	}
	const bt_id_t &		infohash()		const throw();

	/*************** action function	*******************************/
	void			do_command(const std::string &command)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTCLI_SWARM_HPP__  */


 
