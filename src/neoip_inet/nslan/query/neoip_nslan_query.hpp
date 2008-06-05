/*! \file
    \brief Declaration of the nslan_query_t
    
*/


#ifndef __NEOIP_NSLAN_QUERY_HPP__ 
#define __NEOIP_NSLAN_QUERY_HPP__ 
/* system include */
/* local include */
#include "neoip_nslan_query_cb.hpp"
#include "neoip_nslan_query_wikidbg.hpp"
#include "neoip_nslan_id.hpp"
#include "neoip_nslan_rec_arr.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_timeout.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_pkt.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nslan_peer_t;
class	ipport_addr_t;

/** \brief handle a nslan_query_t on a given nslan_peer_t
 */
class nslan_query_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private zerotimer_cb_t
				, private wikidbg_obj_t<nslan_query_t, nslan_query_wikidbg_init>
				{
private:
	nslan_peer_t *	nslan_peer;	//!< backpointer to the nslan_peer_t
	nslan_keyid_t	keyid;		//!< the key to query

	/*************** rxmit_timeout	***************************************/
	delaygen_t	rxmit_delaygen;		//!< the delay_t generator for the rxmit
	timeout_t	rxmit_timeout;		//!< to rxmit the request
	bool		rxmit_timeout_cb()	throw();
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** timeout for the query expiration	***************/
	timeout_t	expire_timeout;
	bool		expire_timeout_cb()	throw();

	/*************** packet building	*******************************/
	pkt_t		build_request()	const throw();

	/*************** received nslan_rec_t	*******************************/
	std::list<std::pair<nslan_rec_t, ipport_addr_t> >	recved_record_queue;
	zerotimer_t	deliver_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();	
	
	/*************** callback stuff	***************************************/
	nslan_query_cb_t *	callback;	//!< the callback to which notify event
	void *			userptr;	//!< the userptr associated with the aboce callback
	bool			notify_callback(const nslan_event_t &nslan_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks


public:
	/*************** ctor/dtor	***************************************/
	nslan_query_t(nslan_peer_t *nslan_peer, nslan_keyid_t &keyid, delay_t expire_delay
					, nslan_query_cb_t *callback, void *userptr)		throw();
	~nslan_query_t()	throw();
	
	/*************** handle recved_pkt_t	*******************************/
	void	queue_incoming_rec_arr(const nslan_rec_arr_t &nslan_rec_arr
					, const ipport_addr_t &remote_addr)	throw();

	/*************** query function	***************************************/
	const nslan_keyid_t &	get_keyid()	const throw()	{ return keyid;	}

	/*************** List of friend function	***********************/
	friend	class	nslan_query_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_NSLAN_QUERY_HPP__ 



