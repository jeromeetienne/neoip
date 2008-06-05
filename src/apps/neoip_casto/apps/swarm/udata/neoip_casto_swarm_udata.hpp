/*! \file
    \brief Header of the \ref casto_swarm_udata_t
    
*/


#ifndef __NEOIP_CASTO_SWARM_UDATA_HPP__ 
#define __NEOIP_CASTO_SWARM_UDATA_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_casto_swarm_udata_wikidbg.hpp"
#include "neoip_bt_cast_mdata_client_cb.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_httpo_full_cb.hpp"
#include "neoip_bt_utmsg_bcast_handler_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casto_swarm_t;

/** \brief Handle the swarm part for the bt_oload_stat_t
 */
class casto_swarm_udata_t : NEOIP_COPY_CTOR_DENY, private bt_utmsg_bcast_handler_cb_t
			, private timeout_cb_t
			, private wikidbg_obj_t<casto_swarm_udata_t, casto_swarm_udata_wikidbg_init, bt_httpo_full_cb_t> {
private:
	casto_swarm_t *	m_casto_swarm;	//!< backpointer to the attached casto_swarm_t

	/*************** bt_utmsg_bcast_handler_t	***********************/
	bt_utmsg_bcast_handler_t *	m_bcast_handler;
	bool neoip_bt_utmsg_bcast_handler_cb(void *cb_userptr, bt_utmsg_bcast_handler_t &cb_bcast_handler
							, pkt_t &recved_pkt)	throw();
	/*************** idle_timeout	***************************************/
	timeout_t	idle_timeout;	//!< to periodically send a packet
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	casto_swarm_udata_t()		throw();
	~casto_swarm_udata_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(casto_swarm_t *p_casto_swarm)	throw();
	
	/*************** Query function	***************************************/
	casto_swarm_t *	casto_swarm()	const throw()	{ return m_casto_swarm;	}

	/*************** List of friend class	*******************************/
	friend class	casto_swarm_udata_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTO_SWARM_UDATA_HPP__ */










