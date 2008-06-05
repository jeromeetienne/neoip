/*! \file
    \brief Declaration of the nslan_listener_t
    
*/


#ifndef __NEOIP_NSLAN_LISTENER_HPP__ 
#define __NEOIP_NSLAN_LISTENER_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_nslan_listener_wikidbg.hpp"
#include "neoip_nslan_id.hpp"
#include "neoip_nudp.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class nslan_peer_t;
class nslan_query_t;

/** \brief handle the listener of the nslan
 */
class nslan_listener_t : NEOIP_COPY_CTOR_DENY, private nudp_cb_t
				, private wikidbg_obj_t<nslan_listener_t, nslan_listener_wikidbg_init>
				{
private:
	// the network connection
	nudp_t *	nudp;
	bool		neoip_inet_nudp_event_cb(void *cb_userptr, nudp_t &cb_nudp, pkt_t &pkt
					, const ipport_addr_t &local_addr
					, const ipport_addr_t &remote_addr)	throw();
	ipport_addr_t	listen_addr;

	/*************** misc function	***************************************/
	inet_err_t	send_pkt(const ipport_addr_t &dest_addr, const pkt_t &pkt)	const throw();

	/*************** to store every nslan_peer_t	***********************/
	std::multimap<nslan_realmid_t, nslan_peer_t *>	peer_db;
	void		peer_link(nslan_peer_t *peer)	throw();
	void		peer_unlink(nslan_peer_t *peer)	throw();
public:
	/*************** ctor/dtor	***************************************/
	nslan_listener_t()		throw();
	~nslan_listener_t()		throw();

	/*************** setup function	***************************************/
	inet_err_t	start(const ipport_addr_t &listen_addr)	throw();
	
	/*************** query function	***************************************/
	const ipport_addr_t &	get_listen_addr()	const throw()	{ return listen_addr;	}

	/*************** Friend Class	***************************************/
	friend	class	nslan_listener_wikidbg_t;	
	friend	class	nslan_peer_t;
	friend	class	nslan_query_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_NSLAN_LISTENER_HPP__ 



