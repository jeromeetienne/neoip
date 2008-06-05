/*! \file
    \brief Header of the bt_ezsession_t
    
*/


#ifndef __NEOIP_BT_EZSESSION_HPP__ 
#define __NEOIP_BT_EZSESSION_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_ezsession_opt.hpp"
#include "neoip_bt_ezsession_profile.hpp"
#include "neoip_bt_ezsession_wikidbg.hpp"
#include "neoip_ndiag_aview_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_session_t;
class	bt_ezswarm_t;
class	bt_err_t;
class	bt_id_t;
class	bt_io_cache_pool_t;
class	udp_vresp_t;
class	kad_listener_t;
class	kad_peer_t;
class	kad_peerid_t;
class	nslan_listener_t;
class	nslan_peer_t;
class	rate_sched_t;
class	ipport_aview_t;

/** \brief class definition for bt_ezsession_t
 */
class bt_ezsession_t : NEOIP_COPY_CTOR_DENY, private ndiag_aview_cb_t
			, private wikidbg_obj_t<bt_ezsession_t, bt_ezsession_wikidbg_init> {
public:
	typedef std::list<bt_ezswarm_t *>	ezswarm_db_t;
private:
	bt_session_t *		m_bt_session;
	bt_ezsession_profile_t	m_profile;
	rate_sched_t *		m_xmit_rsched;	//!< rate_sched_t for xmit
	rate_sched_t *		m_recv_rsched;	//!< rate_sched_t for recvs
	bt_io_cache_pool_t *	m_io_cache_pool;//!< bt_io_cache_pool_t for caching bt_io_vapi_t

	/*************** nslan stuff 	***************************************/
	nslan_listener_t *	m_nslan_listener;//!< the nslan_listener for this kad_listener_t
	nslan_peer_t *		m_nslan_peer;	//!< the nslan_peer_t for all kad_peer_t

	/*************** stuff for bt_peersrc_kad_t	***********************/
	udp_vresp_t *		m_udp_vresp;	//!< the core udp_vresp_t listening on a ipport_addr_t
	kad_listener_t *	m_kad_listener;	//!< the kad_listener for this ntudp_peer_t 
	kad_peer_t *		m_kad_peer;	//!< the kad_peer_t for the underlying NS


	/*************** ndiag_aview_t main callback	***********************/
	bool 			neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
					, const ipport_addr_t &new_ipport_pview)	throw();

	/*************** ndiag_aview_t for udp	*******************************/
	ndiag_aview_t *		m_ndiag_aview_udp;
	bool 			ndiag_aview_udp_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
					, const ipport_addr_t &new_ipport_pview)	throw();

	/*************** ndiag_aview_t for tcp	*******************************/
	ndiag_aview_t *		m_ndiag_aview_tcp;
	bool 			ndiag_aview_tcp_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
					, const ipport_addr_t &new_ipport_pview)	throw();
					
	/*************** bt_ezswarm_t	***************************************/
	ezswarm_db_t		m_ezswarm_db;
	void ezswarm_dolink(bt_ezswarm_t *ezswarm)	throw()	{ m_ezswarm_db.push_back(ezswarm);	}
	void ezswarm_unlink(bt_ezswarm_t *ezswarm)	throw()	{ m_ezswarm_db.remove(ezswarm);		}
public:
	/*************** ctor/dtor	***************************************/
	bt_ezsession_t() 		throw();
	~bt_ezsession_t()		throw();
	
	/*************** setup function	***************************************/
	bt_ezsession_t&	profile(const bt_ezsession_profile_t &profile)		throw();
	bt_err_t	start(const bt_ezsession_opt_t &ezsession_opt, const ipport_aview_t &listen_aview_tcp
						, const ipport_aview_t &listen_aview_udp
						, const kad_peerid_t &kad_peerid
						, const bt_id_t &local_peerid)	throw();
	
	/*************** query function	***************************************/
	const bt_ezsession_profile_t &profile()	const throw()	{ return m_profile;		}
	bt_session_t *		bt_session()	throw()		{ return m_bt_session;		}
	ndiag_aview_t *		ndiag_aview_tcp()throw()	{ return m_ndiag_aview_tcp;	}
	udp_vresp_t *		udp_vresp()	throw()		{ return m_udp_vresp;		}
	kad_listener_t *	kad_listener()	throw()		{ return m_kad_listener;	}
	kad_peer_t *		kad_peer()	throw()		{ return m_kad_peer;		}
	ndiag_aview_t *		ndiag_aview_udp()throw()	{ return m_ndiag_aview_udp;	}
	rate_sched_t *		xmit_rsched()	throw()		{ return m_xmit_rsched;		}
	rate_sched_t *		recv_rsched()	throw()		{ return m_recv_rsched;		}
	bt_io_cache_pool_t *	io_cache_pool()	throw()		{ return m_io_cache_pool;	}
	const ezswarm_db_t &	ezswarm_db()	const throw()	{ return m_ezswarm_db;		}
	bt_ezswarm_t *		bt_ezswarm_by_infohash(const bt_id_t &infohash)	const throw();
	nslan_peer_t *		nslan_peer()	const throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_ezsession_wikidbg_t;
	friend class	bt_ezswarm_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSESSION_HPP__  */



