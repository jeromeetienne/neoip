/*! \file
    \brief Header of the bt_ezswarm_share_t
    
*/


#ifndef __NEOIP_BT_EZSWARM_SHARE_HPP__ 
#define __NEOIP_BT_EZSWARM_SHARE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_ezswarm_share_wikidbg.hpp"
#include "neoip_bt_swarm_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_ezswarm_t;
class	bt_err_t;
class	bt_http_ecnx_pool_t;
class	bt_io_vapi_t;
class	bt_peerpick_vapi_t;
class	bt_peerpick_mode_t;
class	bt_peersrc_http_t;
class	bt_peersrc_kad_t;
class	bt_peersrc_nslan_t;
class	bt_peersrc_utpex_t;
class	bt_utmsg_byteacct_t;
class	bt_utmsg_bcast_t;
class	bt_utmsg_fstart_t;
class	bt_utmsg_piecewish_t;
class	bt_utmsg_punavail_t;

/** \brief class definition for bt_ezswarm_share_t
 */
class bt_ezswarm_share_t : NEOIP_COPY_CTOR_DENY, private bt_swarm_cb_t
			, private wikidbg_obj_t<bt_ezswarm_share_t, bt_ezswarm_share_wikidbg_init> {
private:
	bt_ezswarm_t *		bt_ezswarm;		//!< backpointer to bt_ezswarm_t
	
	/*************** satelite class for bt_swarm_t	***********************/
	bt_http_ecnx_pool_t *	m_http_ecnx_pool;	//!< the bt_http_ecnx_pool_t if configured
	bt_peerpick_vapi_t *	m_peerpick_vapi;	//!< the bt_peerpick_vapi_t to user
	bt_peersrc_http_t *	m_peersrc_http;		//!< the bt_peersrc_http_t if configured
	bt_peersrc_kad_t *	m_peersrc_kad;		//!< the bt_peersrc_kad_t if configured
	bt_peersrc_nslan_t *	m_peersrc_nslan;	//!< the bt_peersrc_nslan_t if configured
	bt_peersrc_utpex_t *	m_peersrc_utpex;	//!< the bt_peersrc_utpex if configured
	bt_utmsg_byteacct_t *	m_utmsg_byteacct;
	bt_utmsg_bcast_t *	m_utmsg_bcast;
	bt_utmsg_fstart_t *	m_utmsg_fstart;
	bt_utmsg_piecewish_t *	m_utmsg_piecewish;
	bt_utmsg_punavail_t *	m_utmsg_punavail;

	/*************** bt_swarm_t	***************************************/
	bt_swarm_t *		m_bt_swarm;
	bool 			neoip_bt_swarm_cb(void *cb_userptr, bt_swarm_t &cb_bt_swarm
						, const bt_swarm_event_t &swarm_event)	throw();

public:
	/*************** ctor/dtor	***************************************/
	bt_ezswarm_share_t() 		throw();
	~bt_ezswarm_share_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_ezswarm_t *bt_ezswarm)				throw();
	
	/*************** query function	***************************************/
	bt_http_ecnx_pool_t *	http_ecnx_pool()	throw()	{ return m_http_ecnx_pool;	}
	bt_peerpick_vapi_t *	peerpick_vapi()		throw()	{ return m_peerpick_vapi;	}
	bt_peersrc_http_t *	peersrc_http()		throw()	{ return m_peersrc_http;	}
	bt_peersrc_kad_t *	peersrc_kad()		throw()	{ return m_peersrc_kad;		}
	bt_peersrc_nslan_t *	peersrc_nslan()		throw()	{ return m_peersrc_nslan;	}
	bt_peersrc_utpex_t *	peersrc_utpex()		throw()	{ return m_peersrc_utpex;	}
	bt_utmsg_byteacct_t *	utmsg_byteacct()	throw()	{ return m_utmsg_byteacct;	}
	bt_utmsg_bcast_t *	utmsg_bcast()		throw()	{ return m_utmsg_bcast;		}
	bt_utmsg_fstart_t *	utmsg_fstart()		throw()	{ return m_utmsg_fstart;	}
	bt_utmsg_piecewish_t *	utmsg_piecewish()	throw()	{ return m_utmsg_piecewish;	}
	bt_utmsg_punavail_t *	utmsg_punavail()	throw()	{ return m_utmsg_punavail;	}
	bt_swarm_t *		bt_swarm()		throw()	{ return m_bt_swarm;		}

	/*************** list of friend class	*******************************/
	friend class	bt_ezswarm_share_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_SHARE_HPP__  */



