/*! \file
    \brief Header of the \ref casti_apps_t

*/


#ifndef __NEOIP_CASTI_APPS_HPP__
#define __NEOIP_CASTI_APPS_HPP__
/* system include */
/* local include */
#include "neoip_casti_apps_wikidbg.hpp"
#include "neoip_casti_apps_profile.hpp"
#include "neoip_casti_inetreach_httpd_cb.hpp"
#include "neoip_bt_cast_mdata_server_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_ezsession_t;
class	casti_swarm_t;
class	casti_swarm_arg_t;
class	casti_ctrl_cline_t;
class	casti_ctrl_wpage_t;
class	casti_inetreach_httpd_t;
class	bt_cast_id_t;
class	clineopt_arr_t;
class	http_listener_t;
class	apps_httpdetect_t;

/** \brief Handle the casti capability
 */
class casti_apps_t : NEOIP_COPY_CTOR_DENY, private bt_cast_mdata_server_cb_t
			, private casti_inetreach_httpd_cb_t
			, private wikidbg_obj_t<casti_apps_t, casti_apps_wikidbg_init> {
private:
	casti_apps_profile_t	m_profile;
	file_path_t		m_io_pfile_dirpath;

	casti_ctrl_cline_t *	m_ctrl_cline;
	casti_ctrl_wpage_t *	m_ctrl_wpage;

	apps_httpdetect_t *	m_apps_httpdetect;	//!< to detect this apps from the web


	http_uri_t		m_dfl_http_peersrc_uri;	//!< default http_peersrc_uri
	http_uri_t		m_dfl_mdata_srv_uri;	//!< default mdata_srv_uri

	/*************** casti_inetreach_httpd_t	***********************/
	casti_inetreach_httpd_t*m_inetreach_httpd;	//!< a http_listener which attempts to be inetreach
	bool		neoip_casti_inetreach_httpd_cb(void *cb_userptr
					, casti_inetreach_httpd_t &cb_inetreach_httpd
					, const ipport_addr_t &new_ipport_pview)	throw();

	/*************** http_listener_t	*******************************/
	http_listener_t *	m_http_listener;	//!< the http_listener_t to receive external http
	bt_err_t		launch_http_listener(const strvar_db_t &casti_conf)	throw();

	/*************** bt_ezsession_t	***************************************/
	bt_ezsession_t *	m_bt_ezsession;
	bt_err_t		launch_ezsession(const strvar_db_t &oload_conf)	throw();

	/*************** bt_cast_mdata_server_t	*******************************/
	bt_cast_mdata_server_t *m_mdata_server;
	bool			neoip_bt_cast_mdata_server_cb(void *cb_userptr
					, bt_cast_mdata_server_t &cb_mdata_server
					, const bt_cast_id_t &cast_id
					, bt_cast_mdata_t *cast_mdata_out)	throw();

	/*************** casti_swarm_t	***************************************/
	std::list<casti_swarm_t *>	swarm_db;
	void swarm_dolink(casti_swarm_t *swarm)	throw()	{ swarm_db.push_back(swarm);	}
	void swarm_unlink(casti_swarm_t *swarm)	throw()	{ swarm_db.remove(swarm);	}
	casti_swarm_t *	swarm_by_cast_id(const bt_cast_id_t &cast_id)		const throw();
public:
	/*************** ctor/dtor	***************************************/
	casti_apps_t()		throw();
	~casti_apps_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t		start()		throw();

	/*************** Query function	***************************************/
	http_listener_t *	http_listener()		const throw()	{ return m_http_listener;	}
	casti_inetreach_httpd_t* inetreach_httpd()	const throw()	{ return m_inetreach_httpd;	}
	bt_cast_mdata_server_t*	mdata_server()		const throw()	{ return m_mdata_server;	}
	bt_ezsession_t *	bt_ezsession()		const throw()	{ return m_bt_ezsession;	}
	const casti_apps_profile_t &profile()		const throw()	{ return m_profile;		}
	const file_path_t &	io_pfile_dirpath()	const throw()	{ return m_io_pfile_dirpath;	}
	const http_uri_t &	dfl_http_peersrc_uri()	const throw()	{ return m_dfl_http_peersrc_uri;}
	const http_uri_t &	dfl_mdata_srv_uri()	const throw()	{ return m_dfl_mdata_srv_uri;}
	static clineopt_arr_t	clineopt_arr()		throw();
	casti_swarm_arg_t	swarm_arg_default()	throw();
	casti_swarm_t *		swarm_by(const http_uri_t &mdata_srv_uri, const std::string &cast_name
					, const std::string &cast_privtext)	const throw();


	/*************** List of friend class	*******************************/
	friend class	casti_apps_wikidbg_t;
	friend class	casti_swarm_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_APPS_HPP__  */










