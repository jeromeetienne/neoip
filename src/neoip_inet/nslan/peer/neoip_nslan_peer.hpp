/*! \file
    \brief Declaration of the nslan_peer_t
    
*/


#ifndef __NEOIP_NSLAN_PEER_HPP__ 
#define __NEOIP_NSLAN_PEER_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_nslan_peer_wikidbg.hpp"
#include "neoip_nslan_id.hpp"
#include "neoip_nslan_rec_arr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class nslan_listener_t;
class nslan_publish_t;
class nslan_query_t;
class pkt_t;
class ipport_addr_t;


/** \brief handle a realm of nslan
 */
class nslan_peer_t : NEOIP_COPY_CTOR_DENY
				, private wikidbg_obj_t<nslan_peer_t, nslan_peer_wikidbg_init>
				{
private:
	nslan_listener_t *	nslan_listener;	//!< backpointer to the nslan_listener_t
	nslan_realmid_t		realmid;	//!< the realmid of this nslan_peer_t

	/*************** misc function	***************************************/
	nslan_rec_arr_t		get_publish_records(const nslan_keyid_t &keyid)	const throw();

	/*************** to store every nslan_publish_t	***********************/
	std::multimap<nslan_keyid_t, nslan_publish_t *>	publish_db;
	void	publish_link(nslan_publish_t *publish)		throw();
	void	publish_unlink(nslan_publish_t *publish)	throw();
	
	/*************** to store every nslan_query_t	***********************/
	std::multimap<nslan_keyid_t, nslan_query_t *>	query_db;
	void	query_link(nslan_query_t *query)		throw();
	void	query_unlink(nslan_query_t *query)		throw();
	
	/*************** packet building	*******************************/
	pkt_t		build_reply(const nslan_keyid_t &keyid
					, const nslan_rec_arr_t &rec_arr)	const throw();		
public:
	/*************** ctor/dtor	***************************************/
	nslan_peer_t(nslan_listener_t *listener, const nslan_realmid_t &realmid)	throw();
	~nslan_peer_t()							throw();
	
	/*************** handle recved_pkt_t	*******************************/
	void	handle_incoming_pkt(const ipport_addr_t &remote_addr, pkt_t &pkt)	throw();
	
	/*************** query function	***************************************/
	const nslan_realmid_t	get_realmid()		const throw()	{ return realmid;		}
	nslan_listener_t *	get_nslan_listener()	throw()		{ return nslan_listener;	}

	/*************** List of friend function	***********************/
	friend	class	nslan_peer_wikidbg_t;
	friend	class	nslan_publish_t;
	friend	class	nslan_query_t;
};



NEOIP_NAMESPACE_END

#endif // __NEOIP_NSLAN_PEER_HPP__ 



