

#ifndef __NEOIP_ROUTER_FULL_HPP__ 
#define __NEOIP_ROUTER_FULL_HPP__ 
/* system include */
/* local include */
#include "neoip_router_full_wikidbg.hpp"
#include "neoip_router_peerid.hpp"
#include "neoip_router_err.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_netif_vdev_cb.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_router_name.hpp"
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	router_peer_t;
class	router_rident_t;

/** \brief Class which implement neoip_router
 */
class router_full_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t, private netif_vdev_cb_t
				, private wikidbg_obj_t<router_full_t, router_full_wikidbg_init>
				{
private:
	router_peer_t *	router_peer;		//!< backpointer to the router_peer_t
	router_peerid_t	m_remote_peerid;	//!< the peerid of the remote peer
	router_name_t	m_remote_dnsname;	//!< the router_name_t of the remote peer
	ip_addr_t	m_local_iaddr;		//!< the local inner ip address
	ip_addr_t	m_remote_iaddr;		//!< the remove inner ip address
	pkt_t		estapkt_in;		//!< incoming packet from the connection establishement
	pkt_t		estapkt_out;		//!< outgoing packet from the connection establishement

	/************** internal function	*******************************/
	size_t		mtu_overhead()	throw();

	/*************** netif_vdev	***************************************/
	netif_vdev_t *	netif_vdev;		//!< the netif_vdev_t for this router_full_t
	bool neoip_netif_vdev_cb(void *cb_userptr, netif_vdev_t &cb_netif_vdev, uint16_t ethertype
								, pkt_t &pkt)	throw();

	/************** socket_full_t	***************************************/
	socket_full_t*	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
				, const socket_event_t &socket_event)		throw();
	/************** socket_event_t handling	*******************************/
	void		handle_socket_recved_data(pkt_t &pkt)			throw();
	void		handle_socket_new_mtu(size_t new_mtu)			throw();	
public:
	/*************** ctor/dtor	***************************************/
	router_full_t(router_peer_t *router_peer)	throw();
	~router_full_t()				throw();

	/*************** Setup Function	***************************************/
	router_err_t	start(socket_full_t *socket_full, const ip_addr_t &m_local_iaddr
					, const ip_addr_t &m_remote_iaddr
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();

	/*************** Query Function	***************************************/
	const ip_addr_t	&	local_iaddr()		const throw()	{ return m_local_iaddr;		}
	const ip_addr_t	&	remote_iaddr()		const throw()	{ return m_remote_iaddr;	}
	const router_peerid_t &	remote_peerid()		const throw()	{ return m_remote_peerid;	}
	const router_name_t &	remote_dnsname()	const throw()	{ return m_remote_dnsname;	}
	
	/*************** action function	*******************************/
	void		forward_pkt(uint16_t ethertype, pkt_t &pkt)	throw();

	/*************** List of friend class	*******************************/
	friend class	router_full_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_FULL_HPP__  */



