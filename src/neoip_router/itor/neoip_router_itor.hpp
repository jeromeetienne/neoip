

#ifndef __NEOIP_ROUTER_ITOR_HPP__ 
#define __NEOIP_ROUTER_ITOR_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_router_itor_wikidbg.hpp"
#include "neoip_router_itor_cnx_cb.hpp"
#include "neoip_router_peerid.hpp"
#include "neoip_router_err.hpp"
#include "neoip_router_name.hpp"
#include "neoip_timeout.hpp"
#include "neoip_dnsgrab_request.hpp"
#include "neoip_kad_query_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	router_peer_t;
class	router_full_t;
class	kad_recdups_t;
class	socket_addr_t;

/** \brief Class which implement neoip_router
 */
class router_itor_t : NEOIP_COPY_CTOR_DENY, private kad_query_cb_t, private router_itor_cnx_cb_t
				, private timeout_cb_t
				, private wikidbg_obj_t<router_itor_t, router_itor_wikidbg_init>
				{
private:
	router_peer_t *		router_peer;		//!< backpointer to the router_peer_t
	router_peerid_t		m_remote_peerid;	//!< the remote peerid
	router_name_t		m_remote_dnsname;	//!< the remote router_name_t (is_fully_qualified())

	bool			autodelete_due2err()	throw();

	/*************** expire_timeout	*******************************/
	timeout_t	expire_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	/*************** dnsgrab_request_t	*******************************/		
	std::list<dnsgrab_request_t>	dnsgrab_req_db;	//!< all the dnsgrab_request_t for this router_itor_t
	void			notify_dnsgrab(const router_full_t *router_full)	throw();
	
	/*************** kad_query_t	***************************************/
	kad_query_t *		kad_query;
	bool			neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
								, const kad_event_t &kad_event)	throw();
	void			handle_recved_kad_recdups(const kad_recdups_t &kad_recdups)	throw();
	void			handle_recved_remote_oaddr(const socket_addr_t &remote_oaddr)	throw();

	/*************** List of router_itor_cnx_t	***********************/
	std::list<router_itor_cnx_t *>		cnx_db;
	void 	cnx_dolink(router_itor_cnx_t *cnx)	throw()	{ cnx_db.push_back(cnx);	}
	void 	cnx_unlink(router_itor_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}
	bool	neoip_router_itor_cnx_cb(void *cb_userptr, router_itor_cnx_t &cb_itor_cnx
					, socket_full_t *socket_full
					, const ip_addr_t &local_addr, const ip_addr_t &remote_addr
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
	/*************** inner packet queue	*******************************/
	typedef	std::pair<uint16_t, pkt_t>	innerpkt_t;
	std::list<innerpkt_t>	innerpkt_queue;
public:
	/*************** ctor/dtor	***************************************/
	router_itor_t(router_peer_t *router_peer)	throw();
	~router_itor_t()				throw();

	/*************** Setup Function	***************************************/
	router_err_t	start(const router_peerid_t &m_remote_peerid
					, const router_name_t &m_remote_dnsname)	throw();
	
	/*************** Query Function	***************************************/
	const router_peerid_t &	remote_peerid()		const throw()	{ return m_remote_peerid;	}
	const router_name_t &	remote_dnsname()	const throw()	{ return m_remote_dnsname;	}
	
	/*************** Action function	*******************************/
	router_itor_t &		queue_dnsgrab_request(const dnsgrab_request_t &dnsgrab_request)	throw();
	router_itor_t &		queue_inner_pkt(uint16_t ethertype, const pkt_t &pkt)		throw();

	/*************** Friend Class	***************************************/
	friend class	router_itor_wikidbg_t;
	friend class	router_itor_cnx_t;
	friend class	router_itor_cnx_wikidbg_t;
};





NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ITOR_HPP__  */



