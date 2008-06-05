/*! \file
    \brief Header of the \ref kad_peer_t
    
*/


#ifndef __NEOIP_KAD_PEER_HPP__ 
#define __NEOIP_KAD_PEER_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_peer_wikidbg.hpp"
#include "neoip_kad_profile.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_stat.hpp"
#include "neoip_kad_bstrap_cb.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_pkt.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_listener_t;
class kad_kbucket_t;
class kad_bstrap_t;
class kad_db_t;
class nslan_publish_t;

/** \brief class definition for kad_peer_t
 */
class kad_peer_t : NEOIP_COPY_CTOR_DENY, private kad_bstrap_cb_t
				, private wikidbg_obj_t<kad_peer_t, kad_peer_wikidbg_init>
				{
private:
	kad_listener_t *kad_listener;	//!< backpointer to the kad_listener_t
	kad_profile_t	profile;	//!< the profile providing parameters for this kad_peer_t	
	kad_peerid_t	m_local_peerid;	//!< the local peerid
	kad_realmid_t	realm_id;	//!< kad_realmid_t to which this kad_peer_t is attached
	kad_kbucket_t *	kbucket;	//!< pointer on the kbucket class
	kad_db_t *	local_db;	//!< database of record published by the local node
	kad_db_t *	remote_db;	//!< database of record published here by remote nodes
	kad_stat_t	stat;		//!< statistic of the kad_peer

	/*************** external kad request	*******************************/
	class		rpc_server_t;
	rpc_server_t *	rpc_server;

	/*************** kad_bstrap_t 	***************************************/
	nslan_publish_t*nslan_publish;	//!< to publish the peer record for the nslan_peer_t	
	kad_bstrap_t *	kad_bstrap;	//!< bootstapping object (if == NULL, bootstrapping is completed)
	bool 		neoip_kad_bstrap_cb(void *cb_userptr, kad_bstrap_t &cb_kad_bstrap) throw();
	kad_err_t	kad_bstrap_init()	throw();
	void		kad_bstrap_deinit()	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_peer_t()	throw();
	~kad_peer_t()	throw();

	/*************** setup functions	*******************************/
	kad_peer_t &	set_profile(const kad_profile_t &profile)		throw();
	kad_err_t	start(kad_listener_t *kad_listener, const kad_realmid_t &realm_id
				, const kad_peerid_t &m_local_peerid = kad_peerid_t::build_random()) throw();

	/*************** various query functions	***********************/
	kad_listener_t *	get_listener()		throw()		{ return kad_listener;	}
	kad_kbucket_t *		get_kbucket()		throw()		{ return kbucket;	}
	kad_db_t *		get_local_db()		throw()		{ return local_db;	}	
	kad_db_t *		get_remote_db()		throw()		{ return remote_db;	}
	kad_stat_t *		get_stat()		throw()		{ return &stat;		}
	const kad_profile_t &	get_profile()		const throw()	{ return profile;	}
	const kad_realmid_t &	get_realmid()		const throw()	{ return realm_id;	}
	const kad_peerid_t &	local_peerid()		const throw()	{ return m_local_peerid;}
	kad_addr_t		srckaddr_build(const ipport_addr_t &remote_oaddr)	const throw();
	void			srckaddr_parse(const kad_addr_t &remote_kaddr
						, const ipport_addr_t &remote_oaddr)	const throw();
	
	// to handle the remote request from the kad_listener_t
	pkt_t		handle_request(pkt_t &pkt, const ipport_addr_t &local_oaddr
							, const ipport_addr_t &remote_oaddr)	throw();

	/*************** update function	*******************************/
	void		update_listen_pview(const ipport_addr_t &new_listen_pview)	throw();
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const kad_peer_t &kad_peer)	throw()
					{ return os << kad_peer.to_string();	}	

	/*************** List of friend function	***********************/
	friend	class	kad_peer_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_PEER_HPP__  */










