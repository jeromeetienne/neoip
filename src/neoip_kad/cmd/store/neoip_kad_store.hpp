/*! \file
    \brief Declaration of the kad_store_t
    
*/


#ifndef __NEOIP_KAD_STORE_HPP__ 
#define __NEOIP_KAD_STORE_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_store_cb.hpp"
#include "neoip_kad_store_wikidbg.hpp"
#include "neoip_kad_closestnode_cb.hpp"
#include "neoip_kad_store_rpc_cb.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_timeout.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;

/** \brief do a RPC client for kad
 */
class kad_store_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private kad_closestnode_cb_t
				, private kad_store_rpc_cb_t
				, private wikidbg_obj_t<kad_store_t, kad_store_wikidbg_init> {
private:
	kad_peer_t *	kad_peer;		//!< backpointer on the kad_peer_t
	kad_recdups_t	kad_recdups;		//!< the kad_recdups_t to store
	size_t		nb_queried_peer;	//!< count the peers which have been queried
	size_t		nb_unreach_peer;	//!< count the peer which failed to reply

	kad_err_t	store_local_db(const kad_recdups_t &kad_recdups)	const throw();

	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	

	/*************** kad_closestnode_t	***************************************/
	kad_closestnode_t *	kad_closestnode;
	bool 		neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
							, const kad_event_t &kad_event)		throw();

	/*************** kad_store_rpc_t	*******************************/
	std::list<kad_store_rpc_t *>	store_rpc_db;
	bool 		neoip_kad_store_rpc_cb(void *cb_userptr, kad_store_rpc_t &cb_kad_store_rpc
							, const kad_event_t &kad_event)		throw();
						
	/*************** Callback	***************************************/
	kad_store_cb_t *callback;	//!< the callback to notify result
	void *		userptr;	//!< the userptr associated with the callback
	bool		notify_callback(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_store_t()		throw();
	~kad_store_t()	throw();
	
	/*************** Setup Functoin	***************************************/
	kad_err_t	start(kad_peer_t *kad_peer, const kad_recdups_t &kad_recdups
					, const delay_t &expire_delay
					, kad_store_cb_t *callback, void *userptr
					, bool local_storage = true) 		throw();

	//! Start helper to ease the publication of a single kad_rec_t
	kad_err_t	start(kad_peer_t *kad_peer, const kad_rec_t &kad_rec, const delay_t &expire_delay
					, kad_store_cb_t *callback, void *userptr
					, bool local_storage = true) 		throw()
	{ return start(kad_peer, kad_recdups_t().update(kad_rec), expire_delay, callback, userptr, local_storage); }


	/*************** List of friend classes	*******************************/
	friend class	kad_store_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_STORE_HPP__ 



