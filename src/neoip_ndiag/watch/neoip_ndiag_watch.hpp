/*! \file
    \brief Header of the ndiag_watch_t
    
*/


#ifndef __NEOIP_NDIAG_WATCH_HPP__ 
#define __NEOIP_NDIAG_WATCH_HPP__ 

/* system include */
/* local include */
#include "neoip_ndiag_watch_profile.hpp"
#include "neoip_ndiag_watch_wikidbg.hpp"
#include "neoip_ndiag_watch_init.hpp"
#include "neoip_upnp_watch_cb.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ndiag_err_t;
class	ndiag_cacheport_t;

/** \brief watch various aspect of the network position
 * 
 * - it watches the ip_addr_t pview of the current network position
 * - it watches the upnp_router availability at the current network position 
 */
class ndiag_watch_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t, private upnp_watch_cb_t
				, private wikidbg_obj_t<ndiag_watch_t, ndiag_watch_wikidbg_init> {
public:	/////////////////////// constant declaration ///////////////////////////
	enum hook_level_t {
		NONE,
		IPADDR_PVIEW_CHANGED,
		UPNPDISC_CHANGED,
		HOOK_MAX,
	};	
private:
	ndiag_watch_profile_t	profile;	//!< the profile for this router_peer_t
	event_hook_t *		event_hook;	//!< handle the ndiag_watch hooks
	ip_addr_t		m_ipaddr_pview;	//!< the current value of the ip_addr_t pview
	bool			m_upnp_isavail;	//!< true if a upnprouter is available
	ndiag_cacheport_t *	m_cacheport;	//!< pointer on the ndiag_cacheport_t
	/*************** upnp_watch_t	***************************************/
	upnp_watch_t *	m_upnp_watch;
	bool 		neoip_upnp_watch_cb(void *cb_userptr, upnp_watch_t &cb_upnp_watch
					, const upnp_watch_event_t &watch_event)	throw();

	/*************** event_zerotimer	*******************************/
	zerotimer_t	event_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ndiag_watch_t()		throw();
	~ndiag_watch_t()	throw();

	/*************** setup function	***************************************/
	ndiag_watch_t &		set_profile(const ndiag_watch_profile_t &profile)	throw();
	ndiag_err_t 		start()							throw();

	/*************** query function	*******************************/
	const ndiag_watch_profile_t &get_profile() const throw(){ return profile;		}
	const ip_addr_t &	ipaddr_pview()	const throw()	{ return m_ipaddr_pview;	}
	bool			upnp_isavail()	const throw()	{ return m_upnp_isavail;	}
	bool			upnp_unavail()	const throw()	{ return !upnp_isavail();	}
	upnp_watch_t *		upnp_watch()	const throw()	{ return m_upnp_watch;		}
	ndiag_cacheport_t *	ndiag_cacheport()const throw()	{ return m_cacheport;		}

	/*************** action function	*******************************/
	void	notify_ipaddr_pview(const ip_addr_t &new_ipaddr_pview)	throw();
	void	event_hook_append(int level_no, event_hook_cb_t *callback, void *userptr) 	throw();
	void	event_hook_remove(int level_no, event_hook_cb_t *callback, void *userptr) 	throw();

	/*************** List of friend function	***********************/
	friend	class	ndiag_watch_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NDIAG_WATCH_HPP__  */



