/*! \file
    \brief Header of the \ref btcli_apps_t class

*/


#ifndef __NEOIP_BTCLI_APPS_HPP__ 
#define __NEOIP_BTCLI_APPS_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_btcli_profile.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_path_arr.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_err_t;
class	bt_id_t;
class	bt_ezsession_t;
class	clineopt_arr_t;
class	btcli_swarm_t;
class	btcli_resp_t;

/** \brief The object to run the neoip_get apps
 */
class btcli_apps_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t {
private:
	btcli_profile_t	m_profile;
	file_path_t	m_fulldata_dir;	//!< dirname where to store datafile for fully downloaded
	file_path_t	m_fullmeta_dir;	//!< dirname where to store metafile for fully downloaded
	file_path_t	m_partdata_dir;	//!< dirname where to store datafile for partially downloaded
	file_path_t	m_partmeta_dir;	//!< dirname where to store metafile for partially downloaded
	
	/*************** initialization	function	***********************/
	bt_err_t	read_fileconf()						throw();
	bt_err_t	launch_from_partmeta_dir()				throw();
	bt_err_t	launch_swarm(const file_path_t &metadata_basename)	throw();

	/*************** bt_ezsession_t	***************************************/
	bt_ezsession_t *m_bt_ezsession;
	bt_err_t	launch_ezsession()		throw();
	
	btcli_resp_t *	btcli_resp;		//!< to handle the resp on the bt_ezsession

	/*************** lookmeta_dir stuff	*******************************/
	bt_err_t	lookmeta_dir_scan()					throw();
	file_path_arr_t	lookmeta_dir_arr;
	timeout_t	lookmeta_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** btcli_swarm_t	***************************************/
	std::list<btcli_swarm_t *>	swarm_db;
	void swarm_dolink(btcli_swarm_t *swarm)throw()	{ swarm_db.push_back(swarm);	}
	void swarm_unlink(btcli_swarm_t *swarm)throw()	{ swarm_db.remove(swarm);	}
public:
	/*************** ctor/dtor	***************************************/
	btcli_apps_t()		throw();
	~btcli_apps_t()		throw();
	
	/*************** setup function	***************************************/
	btcli_apps_t &	set_profile(const btcli_profile_t &profile)		throw();
	bt_err_t	start()	throw();

	/*************** query function	***************************************/
	bt_ezsession_t *	bt_ezsession()		throw()		{ return m_bt_ezsession;}
	const btcli_profile_t &	profile()		const throw()	{ return m_profile;	}
	const file_path_t &	fulldata_dir()		const throw()	{ return m_fulldata_dir;}
	const file_path_t &	fullmeta_dir()		const throw()	{ return m_fullmeta_dir;}
	const file_path_t &	partdata_dir()		const throw()	{ return m_partdata_dir;}
	const file_path_t &	partmeta_dir()		const throw()	{ return m_partmeta_dir;}
	static clineopt_arr_t	clineopt_arr()		throw();
	btcli_swarm_t *		swarm_by_basename(const file_path_t &metadata_basename)	const throw();
	btcli_swarm_t *		swarm_by_infohash(const bt_id_t &infohash)		const throw();
	
	/*************** List of friend class	*******************************/
	friend class	btcli_swarm_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTCLI_APPS_HPP__  */


 
