/*! \file
    \brief Declaration of the ntudp_itor_t
    
*/


#ifndef __NEOIP_NTUDP_ITOR_HPP__ 
#define __NEOIP_NTUDP_ITOR_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <list>
/* local include */
#include "neoip_ntudp_itor_cb.hpp"
#include "neoip_ntudp_itor_profile.hpp"
#include "neoip_ntudp_itor_direct_cb.hpp"
#include "neoip_ntudp_itor_estarelay_cb.hpp"
#include "neoip_ntudp_itor_reverse_cb.hpp"
#include "neoip_ntudp_itor_wikidbg.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_kad_query_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_peer_rec_t;
class	ntudp_rdvpt_t;
class	ntudp_npos_res_t;
class	ntudp_event_t;
class	ntudp_nonce_t;
class	kad_recdups_t;
class	ipport_addr_t;

/** \brief class definition for ntudp_itor_t
 */
class ntudp_itor_t  : NEOIP_COPY_CTOR_DENY, private kad_query_cb_t, private timeout_cb_t
					, private ntudp_itor_direct_cb_t
					, private ntudp_itor_estarelay_cb_t
					, private ntudp_itor_reverse_cb_t
					, private wikidbg_obj_t<ntudp_itor_t, ntudp_itor_wikidbg_init>
					{
private:
	/*************** internal data	***************************************/
	ntudp_peer_t *		ntudp_peer;	//!< backpointer on the attached ntudp_peer_t
	ntudp_addr_t		m_local_addr;	//!< the local address
	ntudp_addr_t		m_remote_addr;	//!< the remote address
	delay_t			expire_delay;	//!< the delay before timeing out this ntudp_itor_t
	ntudp_itor_profile_t *	profile;	//!< the profile attached to this object

	/*************** kad_query_t	***************************************/
	kad_query_t *		kad_query;
	bool			neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
								, const kad_event_t &kad_event)	throw();
	void			handle_recved_kad_recdups(const kad_recdups_t &kad_recdups)	throw();
	void			handle_recved_rdvpt(const ntudp_rdvpt_t &ntudp_rdvpt
						, const ntudp_npos_res_t &remote_npos)		throw();

	/*************** List of direct	***************************************/
	std::list<ntudp_itor_direct_t *>		direct_db;
	void direct_link(ntudp_itor_direct_t *direct)	throw()	{ direct_db.push_back(direct);	}
	void direct_unlink(ntudp_itor_direct_t *direct)	throw()	{ direct_db.remove(direct);	}

	/*************** List of estarelay	*************EXPIRE_DELAY_DFL******************/
	std::list<ntudp_itor_estarelay_t *>		estarelay_db;
	void estarelay_link(ntudp_itor_estarelay_t *estarelay)	throw()	{ estarelay_db.push_back(estarelay);	}
	void estarelay_unlink(ntudp_itor_estarelay_t *estarelay)throw()	{ estarelay_db.remove(estarelay);	}

	/*************** List of reverse	*******************************/
	std::list<ntudp_itor_reverse_t *>		reverse_db;
	void reverse_link(ntudp_itor_reverse_t *reverse)	throw()	{ reverse_db.push_back(reverse);	}
	void reverse_unlink(ntudp_itor_reverse_t *reverse)	throw()	{ reverse_db.remove(reverse);	}

	/*************** subitor callback	*******************************/
	void	subitor_common_cb(udp_full_t *udp_full, const pkt_t &estapkt_in
					, const pkt_t &estapkt_out)				throw();
	bool	neoip_ntudp_itor_direct_cb(void *cb_userptr, ntudp_itor_direct_t &cb_ntudp_itor_direct
					, udp_full_t *udp_full
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
	bool	neoip_ntudp_itor_estarelay_cb(void *cb_userptr, ntudp_itor_estarelay_t &cb_ntudp_itor_estarelay
					, udp_full_t *udp_full
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
	bool	neoip_ntudp_itor_reverse_cb(void *cb_userptr, ntudp_itor_reverse_t &cb_ntudp_itor_reverse
					, udp_full_t *udp_full
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();


	/*************** Utility Function	*******************************/
	bool			sub_itor_is_running()						const throw();
	void			delete_all_subitor()						throw();
	ntudp_itor_direct_t *	get_direct_from_pserver(const ipport_addr_t &pserver_ipport)	throw();
	ntudp_itor_estarelay_t*	get_estarelay_from_pserver(const ipport_addr_t &pserver_ipport)	throw();
	ntudp_itor_reverse_t *	get_reverse_from_pserver(const ipport_addr_t &pserver_ipport)	throw();
	ntudp_itor_reverse_t *	get_reverse_from_nonce(const ntudp_nonce_t &client_nonce)	throw();

	/*************** expire_timeout	***************************************/
	timeout_t		expire_timeout;	//!< to be notified with the ntudp_itor_t expire
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** callback	***************************************/
	ntudp_itor_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const ntudp_event_t &ntudp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_itor_t()			throw();
	~ntudp_itor_t()			throw();

	/*************** Setup function	***************************************/
	ntudp_itor_t &	set_callback(ntudp_itor_cb_t *callback, void *userptr)		throw();
	ntudp_itor_t &	set_local_addr(const ntudp_addr_t &m_local_addr)		throw();
	ntudp_itor_t &	set_timeout(const delay_t &expire_delay)			throw();
	ntudp_itor_t &	set_profile(const ntudp_itor_profile_t &profile)		throw();
	ntudp_err_t	set_remote_addr(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &m_remote_addr)throw();
	ntudp_err_t	start()								throw();
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &m_remote_addr
				, ntudp_itor_cb_t *callback, void *userptr)		throw();
	
	/*************** Query function	***************************************/
	bool			is_null()		const throw()	{ return callback == NULL;	}
	const ntudp_addr_t &	local_addr()		const throw()	{ return m_local_addr;		}
	const ntudp_addr_t &	remote_addr()		const throw()	{ return m_remote_addr;		}

#if 0
	/*************** Compatibility layer	*******************************/
	const ntudp_addr_t &	get_local_addr()	const throw()	{ return local_addr();		}
	const ntudp_addr_t &	get_remote_addr()	const throw()	{ return remote_addr();		}
#endif

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_itor_t &ntudp_itor ) throw()
		{ return os << ntudp_itor.to_string();	}

	/*************** Friend class	***************************************/
	friend class ntudp_itor_direct_t;
	friend class ntudp_itor_estarelay_t;
	friend class ntudp_itor_estarelay_2pserver_t;
	friend class ntudp_itor_estarelay_2peer_t;
	friend class ntudp_itor_reverse_t;
	friend class ntudp_itor_wikidbg_t;
	friend class ntudp_pserver_reverse_t;	// to notify this ntudp_itor_t when a reverse is received
};
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ITOR_HPP__  */



