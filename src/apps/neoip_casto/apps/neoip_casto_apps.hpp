/*! \file
    \brief Header of the \ref casto_apps_t
    
*/


#ifndef __NEOIP_BT_CASTO_HPP__ 
#define __NEOIP_BT_CASTO_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_casto_apps_wikidbg.hpp"
#include "neoip_casto_itor_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_httpo_resp_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casto_itor_t;
class	casto_swarm_t;
class	clineopt_arr_t;
class	bt_httpo_listener_t;
class	bt_ezsession_t;
class	http_listener_t;
class	file_path_t;
class	apps_httpdetect_t;
class	bt_cast_prefetch_t;

/** \brief Handle the http offload capability
 */
class casto_apps_t : NEOIP_COPY_CTOR_DENY, private bt_httpo_resp_cb_t, private casto_itor_cb_t
			, private wikidbg_obj_t<casto_apps_t, casto_apps_wikidbg_init> {
private:
	file_path_t		m_io_pfile_dirpath;
	apps_httpdetect_t *	m_apps_httpdetect;	//!< to detect this apps from the web
	bt_cast_prefetch_t *	m_cast_prefetch;	//!< to allow prefetching from the web

	http_uri_t		m_dfl_mdata_srv_uri;	//!< default mdata_srv_uri

	/*************** bt_httpo_listener_t	***************************************/
	http_listener_t *	m_http_listener;	//!< the http_listener_t to receive external http
	bt_httpo_listener_t *	m_httpo_listener;
	bt_err_t		launch_httpo(const strvar_db_t &casto_conf)	throw();

	/*************** bt_httpo_resp_cb_t	*******************************/
	bt_httpo_resp_t*	m_httpo_resp;
	bool			neoip_bt_httpo_resp_cb(void *cb_userptr, bt_httpo_resp_t &cb_bt_httpo_resp
						, const bt_httpo_event_t &httpo_event)	throw();	

	/*************** bt_ezsession_t	***************************************/
	bt_ezsession_t *	m_bt_ezsession;
	bt_err_t		launch_ezsession(const strvar_db_t &oload_conf)	throw();

	/*************** casto_itor_t	***************************************/
	std::list<casto_itor_t *>		itor_db;
	void itor_dolink(casto_itor_t *itor)	throw()	{ itor_db.push_back(itor);	}
	void itor_unlink(casto_itor_t *itor)	throw()	{ itor_db.remove(itor);		}
	casto_itor_t *	itor_by(const http_uri_t &mdata_srv_uri, const std::string &cast_name
					, const std::string &cast_privhash)	const throw();
	bool 		neoip_casto_itor_cb(void *cb_userptr, casto_itor_t &cb_casto_itor
					, const bt_err_t &bt_err
					, const bt_cast_mdata_t &cast_mdata)	throw();

	/*************** casto_swarm_t	***************************************/
	std::list<casto_swarm_t *>		swarm_db;
	void swarm_dolink(casto_swarm_t *swarm)	throw()	{ swarm_db.push_back(swarm);	}
	void swarm_unlink(casto_swarm_t *swarm)	throw()	{ swarm_db.remove(swarm);		}
	casto_swarm_t *	swarm_by(const http_uri_t &mdata_srv_uri, const std::string &cast_name
					, const std::string &cast_privhash)	const throw();
public:
	/*************** ctor/dtor	***************************************/
	casto_apps_t()		throw();
	~casto_apps_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t		start()		throw();
	
	/*************** Query function	***************************************/
	http_listener_t *	http_listener()		const throw()	{ return m_http_listener;	}
	bt_httpo_listener_t *	httpo_listener()	const throw()	{ return m_httpo_listener;		}
	bt_ezsession_t *	bt_ezsession()		const throw()	{ return m_bt_ezsession;	}
	const file_path_t &	io_pfile_dirpath()	const throw()	{ return m_io_pfile_dirpath;	}
	const http_uri_t &	dfl_mdata_srv_uri()	const throw()	{ return m_dfl_mdata_srv_uri;}
	static clineopt_arr_t	clineopt_arr()		throw();
	

	/*************** List of friend class	*******************************/
	friend class	casto_apps_wikidbg_t;
	friend class	casto_itor_t;
	friend class	casto_swarm_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CASTO_HPP__  */










