/*! \file
    \brief Header of the \ref btrelay_swarm_t
    
*/


#ifndef __NEOIP_BTRELAY_SWARM_HPP__ 
#define __NEOIP_BTRELAY_SWARM_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_btrelay_swarm_wikidbg.hpp"
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_utmsg_bcast_handler_cb.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	btrelay_apps_t;
class	bt_mfile_t;

/** \brief Handle the swarm part for the btrelay_apps_t
 */
class btrelay_swarm_t : NEOIP_COPY_CTOR_DENY, private bt_ezswarm_cb_t
			, private bt_utmsg_bcast_handler_cb_t
			, private timeout_cb_t
			, private wikidbg_obj_t<btrelay_swarm_t, btrelay_swarm_wikidbg_init> {
private:
	btrelay_apps_t *	btrelay_apps;	//!< backpointer to the attached btrelay_apps_t
	bt_cast_mdata_t		m_cast_mdata;	//!< a non null bt_cast_mdata_t IIF link_type == "cast"

	/*************** linking to bt_relay_t	*******************************/
	void			dolink_relay()	throw();
	void			unlink_relay()	throw();
	
	/*************** Internal function	*******************************/
	bool			autodelete(const std::string &reason = "")	throw();
	bool			autodelete(const bt_err_t &bt_err)	throw()	{ return autodelete(bt_err.to_string());	}

	/*************** bt_ezswarm_t	***************************************/
	bt_ezswarm_t *		bt_ezswarm;
	bool 			neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw();
	bt_err_t		launch_ezswarm(const bt_mfile_t &bt_mfile)		throw();

	/*************** bt_utmsg_bcast_handler_t	***********************/
	bt_utmsg_bcast_handler_t *bcast_handler;
	bool neoip_bt_utmsg_bcast_handler_cb(void *cb_userptr, bt_utmsg_bcast_handler_t &cb_bcast_handler
							, pkt_t &recved_pkt)	throw();
	void			bcast_handler_ctor()				throw();
	void			bcast_handler_dtor()				throw();

	/*************** delseed_timeout	*******************************/
	timeout_t		delseed_timeout;
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	btrelay_swarm_t()	throw();
	~btrelay_swarm_t()	throw();

	/*************** Setup function	***************************************/
	bt_err_t		start(btrelay_apps_t *btrelay_apps, const bt_mfile_t &bt_mfile
					, const bt_cast_mdata_t &m_cast_mdata)	throw();
	
	/*************** Query function	***************************************/
	const bt_cast_mdata_t &	cast_mdata()	const throw()	{ return m_cast_mdata;	}

	/*************** List of friend class	*******************************/
	friend class	btrelay_swarm_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTRELAY_SWARM_HPP__ */










