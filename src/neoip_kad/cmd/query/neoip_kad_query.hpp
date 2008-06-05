/*! \file
    \brief Declaration of the kad_query_t
    
*/


#ifndef __NEOIP_KAD_QUERY_HPP__ 
#define __NEOIP_KAD_QUERY_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_query_cb.hpp"
#include "neoip_kad_query_wikidbg.hpp"
#include "neoip_kad_closestnode_cb.hpp"
#include "neoip_kad_findallval_rpc_cb.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_timeout.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_addr_t;

/** \brief do a RPC client for kad
 */
class kad_query_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private kad_closestnode_cb_t
				, private kad_findallval_rpc_cb_t
				, private wikidbg_obj_t<kad_query_t, kad_query_wikidbg_init> {
private:
	kad_peer_t *	kad_peer;		//!< backpointer on the kad_peer_t
	kad_keyid_t	kad_keyid;		//!< the kad_keyid_t of the records to query
	size_t		max_nb_record;		//!< the maximum number of record to return
	size_t		nb_queried_peer;	//!< count the peers which have been queried
	size_t		nb_unreach_peer;	//!< count the peer which failed to reply
	
	kad_recdups_t	result_recdups;		//!< all the kad_rec_t which have been already received

	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	

	/*************** kad_closestnode_t	***************************************/
	kad_closestnode_t *	kad_closestnode;
	bool 		neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
							, const kad_event_t &kad_event)		throw();

	/*************** kad_findallval_rpc_t	*******************************/
	std::list<kad_findallval_rpc_t *>	findallval_rpc_db;
	bool 		neoip_kad_findallval_rpc_cb(void *cb_userptr, kad_findallval_rpc_t &cb_kad_findallval_rpc
							, const kad_event_t &kad_event)		throw();
	bool		handle_recved_recdups(const kad_event_t &kad_event
							, const kad_addr_t &remote_addr)	throw();

	/*************** Callback	***************************************/
	kad_query_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_query_t()		throw();
	~kad_query_t()		throw();
	
	/*************** Setup Functoin	***************************************/
	kad_err_t	start(kad_peer_t *kad_peer, const kad_keyid_t &kad_keyid
					, size_t max_nb_record, const delay_t &expire_delay
					, kad_query_cb_t *callback, void *userptr) 		throw();

	/*************** List of friend classes	*******************************/
	friend class	kad_query_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_QUERY_HPP__ 



