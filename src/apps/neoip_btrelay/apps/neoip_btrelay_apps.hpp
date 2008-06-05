/*! \file
    \brief Header of the \ref btrelay_apps_t
    
*/


#ifndef __NEOIP_BT_BTRELAY_HPP__ 
#define __NEOIP_BT_BTRELAY_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_btrelay_apps_wikidbg.hpp"
#include "neoip_btrelay_apps_profile.hpp"
#include "neoip_btrelay_itor_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	btrelay_itor_t;
class	btrelay_swarm_t;
class	clineopt_arr_t;
class	bt_ezsession_t;
class	file_path_t;
class	bt_relay_t;

/** \brief Handle the http offload capability
 */
class btrelay_apps_t : NEOIP_COPY_CTOR_DENY, private btrelay_itor_cb_t
			, private wikidbg_obj_t<btrelay_apps_t, btrelay_apps_wikidbg_init> {
private:
	btrelay_apps_profile_t	m_profile;
	file_path_t		m_io_pfile_dirpath;
	
	/*************** bt_ezsession_t	***************************************/
	bt_ezsession_t*	m_bt_ezsession;
	bt_err_t	launch_ezsession()		throw();

	/*************** bt_relay_t	****************************************/
	bt_relay_t *	m_bt_relay;	//!< handle globally the relay feature on all swarms

	/*************** fatal error notification	***********************/
	bool		fatal_error(const std::string &reason)	throw();
	bool		fatal_error(const bt_err_t &bt_err)	throw();
	
	/*************** btrelay_itor_t	***************************************/
	std::list<btrelay_itor_t *>		itor_db;
	void itor_dolink(btrelay_itor_t *itor)	throw()	{ itor_db.push_back(itor);	}
	void itor_unlink(btrelay_itor_t *itor)	throw()	{ itor_db.remove(itor);		}
	bool 	neoip_btrelay_itor_cb(void *cb_userptr, btrelay_itor_t &cb_btrelay_itor
				, const bt_err_t &bt_err, const bt_mfile_t &bt_mfile
				, const bt_cast_mdata_t &cast_mdata)			throw();
	bt_err_t	launch_itor(const std::string &link_type
							, const std::string &link_addr)	throw();

	/*************** btrelay_swarm_t	***************************************/
	std::list<btrelay_swarm_t *>		swarm_db;
	void swarm_dolink(btrelay_swarm_t *swarm)	throw()	{ swarm_db.push_back(swarm);	}
	void swarm_unlink(btrelay_swarm_t *swarm)	throw()	{ swarm_db.remove(swarm);	}
public:
	/*************** ctor/dtor	***************************************/
	btrelay_apps_t()	throw();
	~btrelay_apps_t()	throw();
	
	/*************** Setup function	***************************************/
	bt_err_t		start()		throw();
	
	/*************** Query function	***************************************/
	bt_ezsession_t *	bt_ezsession()		const throw()	{ return m_bt_ezsession;}
	bt_relay_t *		bt_relay()		const throw()	{ return m_bt_relay;	}
	const btrelay_apps_profile_t &profile()		const throw()	{ return m_profile;	}
	const file_path_t &	io_pfile_dirpath()	const throw()	{ return m_io_pfile_dirpath;	}
	static clineopt_arr_t	clineopt_arr()		throw();
	

	/*************** List of friend class	*******************************/
	friend class	btrelay_apps_wikidbg_t;
	friend class	btrelay_itor_t;
	friend class	btrelay_swarm_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_BTRELAY_HPP__  */










