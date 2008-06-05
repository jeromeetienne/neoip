/*! \file
    \brief Declaration of the ntudp_pserver_pool_t
    
*/


#ifndef __NEOIP_NTUDP_PSERVER_POOL_HPP__ 
#define __NEOIP_NTUDP_PSERVER_POOL_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_ntudp_pserver_pool_wikidbg.hpp"
#include "neoip_ntudp_pserver_pool_profile.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_kad_query_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_expireset.hpp"
#include "neoip_event_hook.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_peerid_t;
class	kad_recdups_t;

/** \brief class definition for ntudp_pserver_pool_t
 */
class ntudp_pserver_pool_t  : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private kad_query_cb_t
			, private wikidbg_obj_t<ntudp_pserver_pool_t, ntudp_pserver_pool_wikidbg_init>
			{
private:
	/*************** internal data	***************************************/
	ntudp_peer_t *			ntudp_peer;	//!< backpointer on the attached ntudp_peer_t
	ntudp_pserver_pool_profile_t	profile;	//!< the profile attached to this object
	expireset_t<ipport_addr_t>	reach_db;	//!< contain all the reachable ipport_addr_t
	expireset_t<ipport_addr_t>	unreach_db;	//!< contain all the unreachable ipport_addr_t
	expireset_t<ipport_addr_t>	unknown_db;	//!< contain all the unknown ipport_addr_t

	/*************** kad_query_t	***************************************/
	kad_query_t *	kad_query;
	bool		neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
							, const kad_event_t &kad_event)	throw();
	void		handle_recved_kad_recdups(const kad_recdups_t &kad_recdups)	throw();
	bool		handle_recved_pserver_addr(const ipport_addr_t &pserver_addr
							, const ntudp_peerid_t &pserver_peerid
							, const delay_t &record_ttl)	throw();
	
	/*************** noquery stuff	***************************************/
	delaygen_t	delaygen_noquery;	//!< the delay_t generator waiting between query
	timeout_t	timeout_noquery;	//!< to wait before retrigering kad_rec_src
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	bool		want_more_unknown;	//!< true if a caller asked for more, false otherwise.
	event_hook_t	new_unknown_hook;	//!< the event_hook_t for all the callers to register on.
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_pool_t()		throw();
	~ntudp_pserver_pool_t()		throw();

	/*************** Setup function	***************************************/
	ntudp_pserver_pool_t &	set_profile(const ntudp_pserver_pool_profile_t &profile)	throw();	
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer)	throw();
	
	/*************** Array operator	for the databases	***************/
	size_t			reach_size()		const throw()	{ return reach_db.size();	}
	const ipport_addr_t &	reach_at(size_t idx)	const throw()	{ return reach_db[idx];		}
	bool			reach_contain(const ipport_addr_t &ipport_addr )	const throw()
						{ return reach_db.contain(ipport_addr);			}
	delay_t			reach_expire_delay_at(size_t idx) const throw()
						{ return reach_db.expire_delay_at(idx);			}

	size_t			unreach_size()		const throw()	{ return unreach_db.size();	}
	const ipport_addr_t &	unreach_at(size_t idx)	const throw()	{ return unreach_db[idx];	}
	bool			unreach_contain(const ipport_addr_t &ipport_addr )	const throw()
						{ return unreach_db.contain(ipport_addr);		}
	delay_t			unreach_expire_delay_at(size_t idx) const throw()
						{ return unreach_db.expire_delay_at(idx);			}

	size_t			unknown_size()		const throw()	{ return unknown_db.size();	}
	const ipport_addr_t &	unknown_at(size_t idx)	const throw()	{ return unknown_db[idx];	}
	bool			unknown_contain(const ipport_addr_t &ipport_addr )	const throw()
						{ return unknown_db.contain(ipport_addr);		}
	delay_t			unknown_expire_delay_at(size_t idx) const throw()
						{ return unknown_db.expire_delay_at(idx);			}
	
	/*************** for caller to Notify reachability 	***************/	
	void	notify_reach(const ipport_addr_t &ipport_addr)			throw();
	void	notify_unreach(const ipport_addr_t &ipport_addr)		throw();
	void	notify_timedout(const ipport_addr_t &ipport_addr, const delay_t &timedout_delay)throw();

	/*************** for caller to BE notified when new unknown occurs ****/
	void	new_unknown_subscribe(event_hook_cb_t *callback, void *userptr)		throw()
					{ new_unknown_hook.append(0, callback, userptr); }
	void	new_unknown_unsubscribe(event_hook_cb_t *callback, void *userptr)	throw()
					{ new_unknown_hook.remove(0, callback, userptr); }

	/*************** Source API	***************************************/
	void	get_more_unknown()	throw();
	
	/*************** list of friend	***************************************/
	friend	class ntudp_pserver_pool_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_POOL_HPP__  */



