/*! \file
    \brief Declaration of the ntudp_peer_t
    
*/


#ifndef __NEOIP_NTUDP_PEER_HPP__ 
#define __NEOIP_NTUDP_PEER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_ntudp_peer_wikidbg.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_ntudp_profile.hpp"
#include "neoip_ntudp_npos_res.hpp"
#include "neoip_ntudp_npos_watch_cb.hpp"
#include "neoip_ntudp_rdvpt_arr.hpp"
#include "neoip_ntudp_rdvpt_ftor.hpp"
#include "neoip_ntudp_relpeer_cb.hpp"
#include "neoip_netif_addr_watch_cb.hpp"
#include "neoip_kad_publish_cb.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_ndiag_aview_cb.hpp"
#include "neoip_event_hook.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class udp_vresp_t;
class kad_listener_t;
class kad_peer_t;

class ntudp_resp_t;
class ntudp_itor_t;
class ntudp_full_t;

class ntudp_portid_t;

class ntudp_pserver_pool_t;
class ntudp_npos_server_t;
class ntudp_relpeer_t;
class ntudp_pserver_t;
class ntudp_aview_pool_t;

/** \brief class definition for ntudp_peer_t
 */
class ntudp_peer_t  : NEOIP_COPY_CTOR_DENY, private ntudp_npos_watch_cb_t, private netif_addr_watch_cb_t
					, private ntudp_relpeer_cb_t, private kad_publish_cb_t
					, private ndiag_aview_cb_t
					, private wikidbg_obj_t<ntudp_peer_t, ntudp_peer_wikidbg_init>
					{
public:	enum hook_level_t {
		HOOK_NONE,
		HOOK_PRE_PEER_DELETION,
		HOOK_MAX,
	};
private:
	udp_vresp_t *		m_udp_vresp;	//!< the core udp_vresp_t listening on a ipport_addr_t
	kad_listener_t *	m_kad_listener;	//!< the kad_listener for this ntudp_peer_t 
	kad_peer_t *		m_kad_peer;	//!< the kad_peer_t for the underlying NS

	ntudp_pserver_pool_t *	m_pserver_pool;	//!< the pool of public server addresses

	ntudp_profile_t		profile;	//!< the profile for this ntudp_peer_t
	event_hook_t		event_hook;
	
	
	ntudp_npos_server_t *	m_npos_server;	//!< the server needed for network diagnostic layer 
	ntudp_npos_res_t	current_npos_res;//!< the current network position result
	void			update_npos_res()			throw();

	/*************** ndiag_aview_t	***************************************/
	ndiag_aview_t *		m_ndiag_aview;
	bool 			neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
						, const ipport_addr_t &new_ipport_pview)	throw();
					
	/*************** ntudp_npos_watch_t	***************************************/
	ntudp_npos_watch_t *	m_npos_watch;	//!< the updating layer for the network position
	bool 			neoip_ntudp_npos_watch_cb(void *cb_userptr, ntudp_npos_watch_t &cb_ntudp_npos_watch
						, const ntudp_npos_res_t &ntudp_npos_res)	throw();

	/*************** netif_addr_watch_t	***************************************/
	netif_addr_watch_t *	netif_addr_watch;//!< the netif_addr_watch_t to notify the netif addresses
						//!< changes. it MAY be null IIF 
						//!< listen_aview_cfg().lview().ipaddr() is NOT null
	bool 			neoip_netif_addr_watch_cb(void *cb_userptr, netif_addr_watch_t &cb_netif_addr_watch
						, const netif_addr_arr_t &netif_addr_arr)	throw();					
	
	/*************** inetreach ok stuff	*******************************/
	ntudp_pserver_t *	ntudp_pserver;	//!< the ntudp_pserver_t (non-NULL IFF inetreach ok)
	
	/*************** inetreach ko stuff	*******************************/
	ntudp_relpeer_t *	ntudp_relpeer;	//!< the ntudp_relpeer_t (non-NULL IFF inetreach ko)
	bool			neoip_ntudp_relpeer_cb(void *cb_userptr
							, ntudp_relpeer_t &cb_ntudp_relpeer)	throw();

	/*************** natted ok stuff	*******************************/
	ntudp_aview_pool_t *	m_aview_pool;	//!< the pool of local/public view of local address
	
	ntudp_err_t		become_inetreach_ok()	throw();
	ntudp_err_t		become_inetreach_ko()	throw();
	ntudp_err_t		become_natted_ok()	throw();
	ntudp_err_t		become_natted_ko()	throw();

	/*************** listen_addr local/public view from configuration *****/
	ipport_aview_t		m_listen_aview_cfg;	//!< the listen ipport_aview_t from the caller
							//!< configuration (aka MAY NOT be current one)
	ntudp_peerid_t		m_local_peerid;		//!< the ntudp_peerid_t for the ntudp_peer_t
	ntudp_rdvpt_ftor_t	rdvpt_ftor;

	/*************** peer record stuff	*******************************/
	void			publish_peer_rec()	throw();
	kad_publish_t *		kad_publish;	//!< the kad_publish_t for the peer record
	bool 			neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
							, const kad_event_t &cb_kad_event) throw();

	/*************** store the ntudp_resp_t	*******************************/
	std::list<ntudp_resp_t *>	ntudp_resp_db;
	void ntudp_resp_link(ntudp_resp_t *ntudp_resp) 	throw()	{ ntudp_resp_db.push_back(ntudp_resp);	}
	void ntudp_resp_unlink(ntudp_resp_t *ntudp_resp)throw()	{ ntudp_resp_db.remove(ntudp_resp);	}
	/*************** store the ntudp_itor_t	*******************************/
	std::list<ntudp_itor_t *>	ntudp_itor_db;
	void ntudp_itor_link(ntudp_itor_t *ntudp_itor) 	throw()	{ ntudp_itor_db.push_back(ntudp_itor);	}
	void ntudp_itor_unlink(ntudp_itor_t *ntudp_itor)throw()	{ ntudp_itor_db.remove(ntudp_itor);	}
	/*************** store the ntudp_full_t	*******************************/
	std::list<ntudp_full_t *>	ntudp_full_db;
	void ntudp_full_link(ntudp_full_t *ntudp_full) 	throw()	{ ntudp_full_db.push_back(ntudp_full);	}
	void ntudp_full_unlink(ntudp_full_t *ntudp_full)throw()	{ ntudp_full_db.remove(ntudp_full);	}
public:
	/*************** ctor/dtor	***************************************/
	ntudp_peer_t()			throw();
	~ntudp_peer_t()			throw();

	/*************** Setup Function	***************************************/
	ntudp_peer_t &	set_rdvpt_ftor(const ntudp_rdvpt_ftor_t &rdvpt_ftor)	throw();
	ntudp_peer_t &	set_profile(const ntudp_profile_t &profile)		throw();
	ntudp_err_t	start(const ipport_aview_t &m_listen_aview_cfg
					, const ntudp_npos_res_t &init_npos_res
					, const ntudp_peerid_t &m_local_peerid) throw();

	/*************** Query Function	***************************************/
	bool			is_started()		const throw()	{ return !local_peerid().is_null();}

	udp_vresp_t *		udp_vresp()		const throw()	{ return m_udp_vresp;		}
	kad_listener_t *	kad_listener()		const throw()	{ return m_kad_listener;	}
	kad_peer_t *		kad_peer()		const throw()	{ return m_kad_peer;		}
	ndiag_aview_t *		ndiag_aview()		const throw()	{ return m_ndiag_aview;		}

	ntudp_npos_server_t *	npos_server()		const throw()	{ return m_npos_server;		}
	ntudp_npos_watch_t *	npos_watch()		const throw()	{ return m_npos_watch;		}
	ntudp_pserver_pool_t *	pserver_pool()		const throw()	{ return m_pserver_pool;	}
	ntudp_aview_pool_t *	aview_pool()		const throw()	{ return m_aview_pool;		}
	// TODO remove all the get_*
	const ntudp_npos_res_t&	get_npos_res()		const throw()	{ return current_npos_res;	}

	const ntudp_peerid_t &	local_peerid()		const throw()	{ return m_local_peerid;	}
	const ntudp_profile_t &	get_profile()		const throw()	{ return profile;		}
	ntudp_rdvpt_arr_t 	get_rdvpt_arr()		const throw();

	const ipport_aview_t &	listen_aview_cfg()	const throw()	{ return m_listen_aview_cfg;	}
	ipport_aview_t		listen_aview_run()	const throw()	{ return ipport_aview_t(listen_addr_lview(), listen_addr_pview());	}
	const ipport_addr_t &	listen_addr_lview()	const throw();
	ipport_addr_t		listen_addr_pview()	const throw();


	/*************** query function for connection	***********************/
	ntudp_resp_t *		resp_from_portid(const ntudp_portid_t &listen_portid)	throw();
	ntudp_itor_t *		itor_from_cnxaddr(const ntudp_addr_t &local_addr, const ntudp_addr_t &remote_addr)	throw();
	ntudp_full_t *		full_from_cnxaddr(const ntudp_addr_t &local_addr, const ntudp_addr_t &remote_addr)	throw();
	bool			cnxaddr_is_bound(const ntudp_addr_t &local_addr, const ntudp_addr_t &remote_addr) 	throw();

	/*************** event_hook management	*******************************/
	void	event_hook_append(int level_no, event_hook_cb_t *callback, void *userptr) throw();
	void	event_hook_remove(int level_no, event_hook_cb_t *callback, void *userptr) throw();

	/*************** List of friend classes	*******************************/
	friend class	ntudp_peer_wikidbg_t;
	friend class	ntudp_resp_t;
	friend class	ntudp_itor_t;
	friend class	ntudp_full_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PEER_HPP__  */



