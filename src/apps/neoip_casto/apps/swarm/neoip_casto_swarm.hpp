/*! \file
    \brief Header of the \ref casto_swarm_t
    
*/


#ifndef __NEOIP_CASTO_SWARM_HPP__ 
#define __NEOIP_CASTO_SWARM_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_casto_swarm_wikidbg.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casto_apps_t;
class	casto_swarm_udata_t;
class	casto_swarm_httpo_t;
class	bt_httpo_full_t;
class	bt_mfile_t;

/** \brief Handle the swarm part for the bt_oload_stat_t
 */
class casto_swarm_t : NEOIP_COPY_CTOR_DENY, private bt_ezswarm_cb_t
			, private wikidbg_obj_t<casto_swarm_t, casto_swarm_wikidbg_init> {
private:
	casto_apps_t *		m_casto_apps;	//!< backpointer to the attached casto_apps_t
	std::string		m_cast_name;	//!< the cast_name for this swarm
	std::string		m_cast_privhash;	//!< the cast_privhash for this swarm
	http_uri_t		m_mdata_srv_uri;//!< the http_uri_t for the bt_cast_mdata_server_t
	bt_cast_mdata_t		m_cast_mdata;	//!< the retrieved bt_cast_mdata_t
	int64_t			m_casti_dtime;	//!< the delta between local time and casti_date
	
	/*************** Subpart handler	*******************************/
	casto_swarm_udata_t *	m_swarm_udata;	//!< pointer on the casto_swarm_udata_t
	casto_swarm_httpo_t *	m_swarm_httpo;	//!< pointer on the casto_swarm_httpo_t

	/*************** Internal function	*******************************/
	bool			autodelete(const std::string &reason = "")	throw();
	void			local_fstart(bool new_val)			throw();
	void			update_fstart_if_needed()			throw();

	/*************** bt_ezswarm_t	***************************************/
	bt_ezswarm_t *		bt_ezswarm;
	bool 			neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw();
	bool			handle_piece_newly_avail(size_t pieceidx)		throw();
	bt_err_t		launch_ezswarm(const bt_mfile_t &bt_mfile)		throw();
	bt_err_t		bt_ezswarm_enter_share()				throw();
	void			bt_ezswarm_leave_share()				throw();
public:
	/*************** ctor/dtor	***************************************/
	casto_swarm_t()		throw();
	~casto_swarm_t()	throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(casto_apps_t *p_casto_apps, const http_uri_t &m_mdata_srv_uri
					, const std::string &m_cast_name
					, const std::string &m_cast_privhash
					, const bt_cast_mdata_t &p_cast_mdata)	throw();
	
	/*************** Query function	***************************************/
	const bt_cast_mdata_t &	cast_mdata()	const throw()	{ return m_cast_mdata;	}
	const int64_t &		casti_dtime()	const throw()	{ return m_casti_dtime;	}
	casto_swarm_httpo_t *	swarm_httpo()	const throw()	{ return m_swarm_httpo;	}
	casto_swarm_udata_t *	swarm_udata()	const throw()	{ return m_swarm_udata;	}
	const http_uri_t &	mdata_srv_uri()	const throw()	{ return m_mdata_srv_uri;}
	const std::string &	cast_name()	const throw()	{ return m_cast_name;	}
	const std::string &	cast_privhash()	const throw()	{ return m_cast_privhash;}

	/*************** Action function	*******************************/
	void			httpo_full_push(bt_httpo_full_t *httpo_full)	throw();

	/*************** List of friend class	*******************************/
	friend class	casto_swarm_wikidbg_t;
	friend class	casto_swarm_httpo_t;
	friend class	casto_swarm_udata_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTO_SWARM_HPP__ */










