/*! \file
    \brief Header of the \ref casti_swarm_t

*/


#ifndef __NEOIP_CASTI_SWARM_HPP__
#define __NEOIP_CASTI_SWARM_HPP__
/* system include */
/* local include */
#include "neoip_casti_swarm_wikidbg.hpp"
#include "neoip_casti_swarm_profile.hpp"
#include "neoip_bt_cast_mdata_dopublish_cb.hpp"
#include "neoip_bt_cast_mdata_unpublish_cb.hpp"
#include "neoip_bt_cast_id.hpp"
#include "neoip_bt_scasti_mod_type.hpp"
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_file_size.hpp"
#include "neoip_file_size_arr.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_apps_t;
class	casti_swarm_arg_t;
class	casti_swarm_udata_t;
class	casti_swarm_spos_t;
class	casti_swarm_scasti_t;
class	bt_cast_mdata_t;
class	bt_cast_udata_t;
class	bt_scasti_vapi_t;
class	bt_mfile_t;
class	bt_ezswarm_state_t;

/** \brief Handle the swarm part for the bt_oload_stat_t
 */
class casti_swarm_t : NEOIP_COPY_CTOR_DENY, private bt_ezswarm_cb_t
			, private bt_cast_mdata_unpublish_cb_t
			, private bt_cast_mdata_dopublish_cb_t
			, private wikidbg_obj_t<casti_swarm_t, casti_swarm_wikidbg_init> {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the bt_utmsg_bcast_t key to send the casti swarm data
	static const std::string 	UTMSG_BCAST_KEY;
private:
	casti_apps_t *		casti_apps;	//!< backpointer to the attached casti_apps_t
	casti_swarm_profile_t	m_profile;

	bt_cast_id_t		m_cast_id;	//!< the bt_cast_id_t for this casti_swarm_t
	std::string		m_cast_name;	//!< the cast_name for this swarm
	std::string		m_cast_privtext;//!< the cast_privtext for this swarm
	http_uri_t		m_mdata_srv_uri;//!< the http_uri_t for the mdata_server
	http_uri_t		m_scasti_uri;	//!< source http_uri_t for this casti_swarm_t
	bt_scasti_mod_type_t	m_scasti_mod;	//!< the bt_scasti_mod_type_t for this casti_swarm_t
	http_uri_t		m_http_peersrc_uri;//!< http_uri_t for the bt_peersrc_http_t
	std::string		m_web2srv_str;	//!< the web2srv_str for this swarm

	size_t			pieceq_beg;
	size_t			pieceq_end;
	size_t			mdata_nonce;

	/*************** Sub service	***************************************/
	casti_swarm_udata_t *	m_swarm_udata;	//!< pointer on the udata handler
	casti_swarm_spos_t*	m_swarm_spos;	//!< pointer on the bt_cast_spos_arr_t handler
	casti_swarm_scasti_t *	m_swarm_scasti;	//!< pointer on the bt_scasti_vapi_t handler

	/*************** Internal function	*******************************/
	bool			autodelete(const bt_err_t &bt_err)	throw()	{ return autodelete(bt_err.to_string());	}
	bool			autodelete(const std::string &reason = "")	throw();
	bt_scasti_vapi_t *	scasti_vapi()					const throw();

	/*************** bt_ezswarm_t	***************************************/
	bt_ezswarm_t *		m_bt_ezswarm;
	bool 			neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw();
	bt_err_t		launch_ezswarm(const bt_mfile_t &bt_mfile)		throw();
	bt_err_t		bt_ezswarm_enter_share()				throw();
	void			bt_ezswarm_leave_share()				throw();

	/*************** bt_cast_mdata_dopublish_t	*******************************/
	bt_cast_mdata_dopublish_t *m_mdata_dopublish;
	bool	neoip_bt_cast_mdata_dopublish_cb(void *cb_userptr
				, bt_cast_mdata_dopublish_t &cb_mdata_dopublish
				, bt_cast_mdata_t *cast_mdata_out
				, uint16_t *port_lview_out, uint16_t *post_pview_out
				, std::string *casti_uri_pathquery_out)		throw();

	/*************** bt_cast_mdata_unpublish_t	***********************/
	bt_cast_mdata_unpublish_t *m_mdata_unpublish;
	bool	neoip_bt_cast_mdata_unpublish_cb(void *cb_userptr, bt_cast_mdata_unpublish_t &cb_swarm_unpublish
						, const bt_err_t &bt_err)	throw();
public:
	/*************** ctor/dtor	***************************************/
	casti_swarm_t()		throw();
	~casti_swarm_t()	throw();

	/*************** Setup function	***************************************/
	casti_swarm_t &	profile(const casti_swarm_profile_t &profile)			throw();
	bt_err_t	start(casti_swarm_arg_t &swarm_arg)	throw();

	/*************** Query function	***************************************/
	const http_uri_t &		mdata_srv_uri()	const throw()	{ return m_mdata_srv_uri;}
	const std::string &		cast_name()	const throw()	{ return m_cast_name;	}
	const std::string &		cast_privtext()	const throw()	{ return m_cast_privtext;}
	const http_uri_t &		http_peersrc_uri()const throw()	{ return m_http_peersrc_uri;}
	bt_cast_id_t			cast_id()	const throw()	{ return m_cast_id;	}
	const http_uri_t &		scasti_uri()	const throw()	{ return m_scasti_uri;	}
	const bt_scasti_mod_type_t &	scasti_mod()	const throw()	{ return m_scasti_mod;	}
	bt_ezswarm_t *			bt_ezswarm()	const throw()	{ return m_bt_ezswarm;	}
	casti_swarm_udata_t *		swarm_udata()	const throw()	{ return m_swarm_udata;	}
	casti_swarm_spos_t *		swarm_spos()	const throw()	{ return m_swarm_spos;	}
	casti_swarm_scasti_t *		swarm_scasti()	const throw()	{ return m_swarm_scasti;}
	const casti_swarm_profile_t &	profile()	const throw()	{ return m_profile;	}
	bool				is_published()	const throw();
	std::string			cast_privhash()	const throw();
	bt_ezswarm_state_t		state()		const throw();
	bt_cast_mdata_t			current_mdata()	const throw();
	bt_cast_udata_t			current_udata()	const throw();

	/*************** Action function	*******************************/
	bool		gracefull_shutdown(const std::string &reason)		throw();
	void		start_publishing()		throw();
	void		notify_republish_required()	throw();

	/*************** List of friend class	*******************************/
	friend class	casti_swarm_wikidbg_t;
	friend class	casti_swarm_udata_t;
	friend class	casti_swarm_spos_t;
	friend class	casti_swarm_scasti_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_SWARM_HPP__ */










