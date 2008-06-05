/*! \file
    \brief Declaration of the ntudp_resp_t
    
*/


#ifndef __NEOIP_NTUDP_RESP_HPP__ 
#define __NEOIP_NTUDP_RESP_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <list>
/* local include */
#include "neoip_ntudp_resp_cb.hpp"
#include "neoip_ntudp_resp_profile.hpp"
#include "neoip_ntudp_resp_direct_cb.hpp"
#include "neoip_ntudp_resp_estarelay_cb.hpp"
#include "neoip_ntudp_resp_reverse_cb.hpp"
#include "neoip_ntudp_resp_wikidbg.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_event_t;
class	udp_full_t;

/** \brief class definition for ntudp_resp_t
 */
class ntudp_resp_t  : NEOIP_COPY_CTOR_DENY
					, private ntudp_resp_direct_cb_t
					, private ntudp_resp_estarelay_cb_t
					, private ntudp_resp_reverse_cb_t
					, private wikidbg_obj_t<ntudp_resp_t, ntudp_resp_wikidbg_init>
					{
private:
	/*************** internal data	***************************************/
	ntudp_peer_t *		ntudp_peer;	//!< backpointer on the attached ntudp_peer_t
	ntudp_addr_t		m_listen_addr;	//!< the address on which to listen
	ntudp_resp_profile_t *	profile;	//!< the profile attached to this object

	/*************** List of direct	***************************************/
	std::list<ntudp_resp_direct_t *>		direct_db;
	void direct_link(ntudp_resp_direct_t *direct)	throw()	{ direct_db.push_back(direct);	}
	void direct_unlink(ntudp_resp_direct_t *direct)	throw()	{ direct_db.remove(direct);	}
	ntudp_resp_direct_t *get_resp_direct_for_cnxaddr(const ntudp_addr_t &local_addr
						, const ntudp_addr_t &remote_addr) throw();
	bool	neoip_ntudp_resp_direct_cb(void *cb_userptr, ntudp_resp_direct_t &cb_ntudp_resp_direct
					, udp_full_t *udp_full
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
					
	/*************** List of estarelay	*******************************/
	std::list<ntudp_resp_estarelay_t *>		estarelay_db;
	void estarelay_link(ntudp_resp_estarelay_t *estarelay)	throw()	{ estarelay_db.push_back(estarelay);	}
	void estarelay_unlink(ntudp_resp_estarelay_t *estarelay)throw()	{ estarelay_db.remove(estarelay);	}
	ntudp_resp_estarelay_t *get_resp_estarelay_for_cnxaddr(const ntudp_addr_t &local_addr
						, const ntudp_addr_t &remote_addr) throw();
	bool	neoip_ntudp_resp_estarelay_cb(void *cb_userptr, ntudp_resp_estarelay_t &cb_ntudp_resp_estarelay
					, udp_full_t *udp_full
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
					
	/*************** List of reverse	*******************************/
	std::list<ntudp_resp_reverse_t *>		reverse_db;
	void reverse_link(ntudp_resp_reverse_t *reverse)	throw()	{ reverse_db.push_back(reverse);	}
	void reverse_unlink(ntudp_resp_reverse_t *reverse)	throw()	{ reverse_db.remove(reverse);	}
	ntudp_resp_reverse_t *get_resp_reverse_for_cnxaddr(const ntudp_addr_t &local_addr
						, const ntudp_addr_t &remote_addr) throw();
	bool	neoip_ntudp_resp_reverse_cb(void *cb_userptr, ntudp_resp_reverse_t &cb_ntudp_resp_reverse
					, udp_full_t *udp_full
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();


	/*************** Utility Function	*******************************/
	bool	notify_cnx_established(const ntudp_addr_t &local_addr, const ntudp_addr_t &remote_addr
					, udp_full_t *udp_full
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
	void	delete_subresp_for_cnxaddr(const ntudp_addr_t &local_addr
							, const ntudp_addr_t &remote_addr)	throw();

	/*************** callback	***************************************/
	ntudp_resp_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const ntudp_event_t &ntudp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_resp_t()			throw();
	~ntudp_resp_t()			throw();

	/*************** Setup function	***************************************/
	ntudp_resp_t &	set_profile(const ntudp_resp_profile_t &profile)		throw();
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &m_listen_addr
					, ntudp_resp_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	bool			is_null()		const throw()	{ return callback == NULL;	}
	const ntudp_addr_t &	listen_addr()		const throw()	{ return m_listen_addr;		}

	/*************** cnx request notification	***********************/
	void			notify_recved_direct_cnx_request(pkt_t &incoming_pkt
							, udp_full_t *udp_full)			throw();
	static datum_t		notify_recved_pkt_tunl_srv2cli(ntudp_peer_t *ntudp_peer
							, const datum_t &reved_datum)		throw();

	/*************** display function	*******************************/
	std::string		to_string()		const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_resp_t &ntudp_resp ) throw()
							{ return os << ntudp_resp.to_string();	}

	/*************** Friend class	***************************************/
	friend class ntudp_resp_direct_t;
	friend class ntudp_resp_estarelay_t;
	friend class ntudp_resp_reverse_t;
	friend class ntudp_resp_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RESP_HPP__  */



