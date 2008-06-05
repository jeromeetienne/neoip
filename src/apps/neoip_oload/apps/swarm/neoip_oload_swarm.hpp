/*! \file
    \brief Header of the \ref oload_swarm_t
    
*/


#ifndef __NEOIP_OLOAD_SWARM_HPP__ 
#define __NEOIP_OLOAD_SWARM_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_oload_swarm_wikidbg.hpp"
#include "neoip_oload_swarm_profile.hpp"
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_httpo_full_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	oload_apps_t;
class	bt_httpo_full_t;
class	bt_mfile_t;
class	bt_io_pfile_asyncdel_t;

/** \brief Handle the swarm part for the bt_oload_stat_t
 */
class oload_swarm_t : NEOIP_COPY_CTOR_DENY, private bt_ezswarm_cb_t
			, private bt_httpo_full_cb_t, private timeout_cb_t
			, private wikidbg_obj_t<oload_swarm_t, oload_swarm_wikidbg_init, bt_httpo_full_cb_t> {
private:
	oload_apps_t *		oload_apps;	//!< backpointer to the attached oload_apps_t
	http_uri_t		m_inner_uri;	//!< the inner_uri for this oload_swarm_t
	oload_swarm_profile_t	m_profile;	//!< the current oload_swarm_profile_t
	bt_io_pfile_asyncdel_t*	pfile_asyncdel;	//!< the object to periodically delete piece

	/*************** Internal function	*******************************/
	bool			autodelete(const std::string &reason = "")	throw();
	bool			autodelete(const bt_err_t &bt_err)	throw()	{ return autodelete(bt_err.to_string());	}

	/*************** bt_httpo_full_t	*******************************/
	std::list<bt_httpo_full_t *>	m_httpo_full_db;
	bool			neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
						, const bt_httpo_event_t &httpo_event)	throw();
	void			httpo_full_start_all()					throw();
	void			httpo_full_start_one(bt_httpo_full_t *httpo_full)	throw();
	bt_err_t		httpo_full_is_mfile_compatible(bt_httpo_full_t *httpo_full)throw();
	bt_err_t		httpo_full_do_start(bt_httpo_full_t *httpo_full)	throw();
	void			httpo_full_db_dtor(const std::string &reason)		throw();

	/*************** bt_ezswarm_t	***************************************/
	bt_ezswarm_t *		bt_ezswarm;
	bool 			neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw();
	bool			handle_piece_newly_avail(size_t pieceidx)		throw();
	bt_err_t		launch_ezswarm(const bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)		throw();

	/*************** idle_timeout	***************************************/
	timeout_t		idle_timeout;
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	oload_swarm_t()		throw();
	~oload_swarm_t()	throw();

	/*************** Setup function	***************************************/
	oload_swarm_t &		profile(const oload_swarm_profile_t &m_profile)	throw();
	bt_err_t		start(oload_apps_t *oload_apps, const http_uri_t &nested_uri
						, const bt_mfile_t &bt_mfile)	throw();
	
	/*************** Query function	***************************************/
	const oload_swarm_profile_t &profile()	const throw()	{ return m_profile;	}
	const http_uri_t &	inner_uri()	const throw()	{ return m_inner_uri;	}
	
	/*************** Action function	*******************************/
	void			httpo_full_push(bt_httpo_full_t *httpo_full)	throw();

	/*************** List of friend class	*******************************/
	friend class	oload_swarm_wikidbg_t;
	friend class	oload_httpo_ctrl_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_SWARM_HPP__ */










