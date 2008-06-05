/*! \file
    \brief Declaration of the kad_delete_t
    
*/


#ifndef __NEOIP_KAD_DELETE_HPP__ 
#define __NEOIP_KAD_DELETE_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_delete_cb.hpp"
#include "neoip_kad_delete_wikidbg.hpp"
#include "neoip_kad_closestnode_cb.hpp"
#include "neoip_kad_delete_rpc_cb.hpp"
#include "neoip_kad_id.hpp"
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
class kad_delete_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private kad_closestnode_cb_t
			, private kad_delete_rpc_cb_t
			, private wikidbg_obj_t<kad_delete_t, kad_delete_wikidbg_init> {
private:
	kad_peer_t *	kad_peer;		//!< backpointer on the kad_peer_t
	kad_recid_t	kad_recid;		//!< the kad_recid_t of the record to delete
	kad_keyid_t	kad_keyid;		//!< the kad_keyid_t of the record to delete
	size_t		nb_queried_peer;	//!< count the peers which have been queried
	size_t		nb_unreach_peer;	//!< count the peer which failed to reply

	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	

	/*************** kad_closestnode_t	***************************************/
	kad_closestnode_t *	kad_closestnode;
	bool 		neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
							, const kad_event_t &kad_event)		throw();

	/*************** kad_delete_rpc_t	*******************************/
	std::list<kad_delete_rpc_t *>	delete_rpc_db;
	bool 		neoip_kad_delete_rpc_cb(void *cb_userptr, kad_delete_rpc_t &cb_kad_delete_rpc
							, const kad_event_t &kad_event)		throw();
						
	/*************** Callback	***************************************/
	kad_delete_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_delete_t()		throw();
	~kad_delete_t()		throw();
	
	/*************** Setup Function	***************************************/
	kad_err_t	start(kad_peer_t *kad_peer, const kad_recid_t &kad_recid
					, const kad_keyid_t &kad_keyid, const delay_t &expire_delay
					, kad_delete_cb_t *callback, void *userptr) 		throw();

	/*************** Query function	***************************************/
	kad_peer_t *	get_kad_peer()	throw()		{ return kad_peer;		}
	bool		is_autodelete()	const throw()	{ return callback == NULL;	}

	/*************** List of friend classes	*******************************/
	friend class	kad_delete_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_DELETE_HPP__ 



