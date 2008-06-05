/*! \file
    \brief Declaration of the kad_listener_t
    
*/


#ifndef __NEOIP_KAD_LISTENER_HPP__ 
#define __NEOIP_KAD_LISTENER_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <list>
/* local include */
#include "neoip_kad_listener_wikidbg.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_kad_srvcnx_cb.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_addr_t;
class	kad_srvcnx_t;
class	kad_event_t;
class	udp_vresp_t;
class	nslan_listener_t;
class	nslan_peer_t;

/** \brief do a kad server
 */
class kad_listener_t : private kad_srvcnx_cb_t
				, private wikidbg_obj_t<kad_listener_t, kad_listener_wikidbg_init>
				{
private:
	ipport_aview_t		m_listen_aview;		//!< the public view of the listen_oaddr
	udp_vresp_t *		udp_vresp;		//!< pointer on the udp_vresp_t to run on

	/*************** kad_peer_t database	*******************************/
	std::list<kad_peer_t *>	peer_db;	//!< the kad_peer_t database
	void			peer_link(kad_peer_t *kad_peer)			throw();
	void			peer_unlink(kad_peer_t *kad_peer)		throw();
	bool			is_peer_compatible(kad_peer_t *kad_peer)	throw();

	/*************** nslan stuff for bootstrapping	***********************/
	nslan_listener_t *	m_nslan_listener;//!< the nslan_listener for this kad_listener_t
	nslan_peer_t *		m_nslan_peer;	//!< the nslan_peer_t for all kad_peer_t
	
	/*************** to handle incoming request	***********************/
	kad_srvcnx_t *		kad_srvcnx;
	bool			neoip_kad_srvcnx_cb(void *cb_userptr, kad_srvcnx_t &cb_kad_srvcnx
						, pkt_t &pkt, const ipport_addr_t &local_oaddr
						, const ipport_addr_t &remote_oaddr)	throw();
	
	/*************** peer_cmd management	*******************************/
	kad_peer_t *		get_kad_peer(const kad_realmid_t &realm_id
							, const kad_peerid_t &peerid)	throw();
							
public:
	/*************** ctor/dtor	***************************************/
	kad_listener_t()		throw();
	~kad_listener_t()		throw();
	
	/*************** setup function	***************************************/
	kad_err_t	start(const ipport_addr_t &nslan_addr, udp_vresp_t *udp_vresp
					, const ipport_aview_t &m_listen_aview)		throw();

	/*************** query function	***************************************/
	const ipport_aview_t &	listen_aview()		const throw()	{ return m_listen_aview;	}
	nslan_listener_t *	nslan_listener()	const throw()	{ return m_nslan_listener;	}
	nslan_peer_t *		nslan_peer()		const throw()	{ return m_nslan_peer;		}

	/*************** helper query function	*******************************/
	const ipport_addr_t &	listen_lview()		const throw()	{ return listen_aview().lview();}
	const ipport_addr_t &	listen_pview()		const throw()	{ return listen_aview().pview();}

	/*************** update function	*******************************/
	void		notify_echoed_local_ipaddr(const ip_addr_t &echoed_ipaddr
						, const kad_addr_t &remote_addr)	throw();
	void		update_listen_pview(const ipport_addr_t &new_listen_pview)	throw();

	/*************** List of friend function	***********************/
	friend class	kad_listener_wikidbg_t;
	friend class	kad_peer_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_LISTENER_HPP__ 



