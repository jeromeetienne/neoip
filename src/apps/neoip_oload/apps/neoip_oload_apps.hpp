/*! \file
    \brief Header of the \ref oload_apps_t
    
*/


#ifndef __NEOIP_OLOAD_APPS_HPP__ 
#define __NEOIP_OLOAD_APPS_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_oload_apps_wikidbg.hpp"
#include "neoip_oload_itor_cb.hpp"
#include "neoip_http_sresp_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_ezsession_t;

class	oload_itor_t;
class	oload_swarm_t;

class	oload_mod_vapi_t;

class	clineopt_arr_t;
class	bt_httpo_listener_t;
class	bt_httpo_full_t;
class	http_listener_t;
class	http_uri_t;
class	apps_httpdetect_t;
class	bt_cast_prefetch_t;
class	oload_httpo_ctrl_t;
class	oload_flash_xdom_t;

/** \brief Handle the http offload capability
 */
class oload_apps_t : NEOIP_COPY_CTOR_DENY, private oload_itor_cb_t
			, private wikidbg_obj_t<oload_apps_t, oload_apps_wikidbg_init> {
private:
	file_path_t		m_io_pfile_dirpath;
	
	/*************** service stuff	***************************************/
	apps_httpdetect_t *	m_apps_httpdetect;	//!< to detect this apps from the web
	bt_cast_prefetch_t *	m_cast_prefetch;	//!< to allow prefetching from the web
	oload_httpo_ctrl_t *	m_httpo_ctrl;		//!< to allow rpc to control httpo_full
	oload_flash_xdom_t *	m_flash_xdom;		//!< to allow flash to dload thru oload
	
	/*************** bt_httpo_listener_t	*******************************/
	http_listener_t *	m_http_listener;	//!< the http_listener_t to receive external http
	bt_httpo_listener_t *	m_httpo_listener;
	bt_err_t		launch_httpo(const strvar_db_t &oload_conf)	throw();
	void			httpo_full_post_esta(bt_httpo_full_t *httpo_full)throw();

	/*************** bt_ezsession_t	***************************************/
	bt_ezsession_t *	m_bt_ezsession;
	bt_err_t		launch_ezsession(const strvar_db_t &oload_conf)	throw();

	/*************** oload_itor_t	***************************************/
	std::list<oload_itor_t *>		itor_db;
	void itor_dolink(oload_itor_t *itor)	throw()	{ itor_db.push_back(itor);	}
	void itor_unlink(oload_itor_t *itor)	throw()	{ itor_db.remove(itor);		}
	oload_itor_t *	itor_by_inner_uri(const http_uri_t &inner_uri)		const throw();
	bool 		neoip_oload_itor_cb(void *cb_userptr, oload_itor_t &cb_oload_itor
				, const bt_err_t &bt_err, const bt_mfile_t &bt_mfile)	throw();

	/*************** oload_swarm_t	***************************************/
	std::list<oload_swarm_t *>		swarm_db;
	void swarm_dolink(oload_swarm_t *swarm)	throw()	{ swarm_db.push_back(swarm);	}
	void swarm_unlink(oload_swarm_t *swarm)	throw()	{ swarm_db.remove(swarm);	}
	oload_swarm_t *	swarm_by_inner_uri(const http_uri_t &inner_uri)		const throw();

	/*************** oload_mod_vapi_t	*******************************/
	std::list<oload_mod_vapi_t *>		mod_db;
	void mod_dolink(oload_mod_vapi_t *mod)	throw()	{ mod_db.push_back(mod);	}
	void mod_unlink(oload_mod_vapi_t *mod)	throw()	{ mod_db.remove(mod);		}
	bt_err_t	launch_mod()	throw();
public:
	/*************** ctor/dtor	***************************************/
	oload_apps_t()		throw();
	~oload_apps_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t		start()			throw();
	
	/*************** Query function	***************************************/
	http_listener_t *	http_listener()		const throw()	{ return m_http_listener;	}
	bt_httpo_listener_t *	httpo_listener()	const throw()	{ return m_httpo_listener;	}
	bt_ezsession_t *	bt_ezsession()		const throw()	{ return m_bt_ezsession;	}
	const file_path_t &	io_pfile_dirpath()	const throw()	{ return m_io_pfile_dirpath;	}
	static clineopt_arr_t	clineopt_arr()		throw();

	/*************** List of friend class	*******************************/
	friend class	oload_apps_wikidbg_t;
	friend class	oload_itor_t;
	friend class	oload_swarm_t;
	friend class	oload_mod_raw_t;
	friend class	oload_mod_flv_t;
	friend class	oload_httpo_ctrl_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_APPS_HPP__  */










