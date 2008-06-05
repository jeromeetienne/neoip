/*! \file
    \brief Definition of the \ref kad_bstrap_cand_t

\par Algorithm Description
- do a rpc findnode toward the remote_addr
  - it aims to test the reachability of the remote_addr
- if it fails, notify the faillure
- if it succeed, start a closestnode on the local peerid using the result of the previous findnode.
  - from the paper, "[the bootstrapping node] performs a node lookup for its
    own node ID."
- if the closestnode fails, notify the faillure
- if the closestnode suceed, notify the suceess

*/

/* system include */
/* local include */
#include "neoip_kad_bstrap_cand.hpp"
#include "neoip_kad_bstrap.hpp"
#include "neoip_kad_ping_rpc.hpp"
#include "neoip_kad_closestnode.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_bstrap_cand_t::kad_bstrap_cand_t(kad_bstrap_t *kad_bstrap)		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the kad_bstrap backpointer
	this->kad_bstrap	= kad_bstrap;
	// zero some fields
	ping_rpc		= NULL;
	kad_closestnode		= NULL;
	// link it to the database
	kad_bstrap->cand_dolink(this);
}

/** \brief Destructor
 */
kad_bstrap_cand_t::~kad_bstrap_cand_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");	
	// link it to the database
	kad_bstrap->cand_unlink(this);
	// delete the kad_ping_rpc_t if needed
	nipmem_zdelete ping_rpc;
	// delete the kad_closestnode if needed
	nipmem_zdelete kad_closestnode;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           START function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation on this candidate
 */
kad_err_t	kad_bstrap_cand_t::start(const kad_addr_t &remote_addr, kad_bstrap_cand_cb_t *callback
						, void *userptr)	throw()
{
	kad_peerid_t	local_peerid	= kad_bstrap->kad_peer->local_peerid();
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter remote_addr=" << remote_addr);
	// sanity check - the ping_rpc and kad_closestnode MUST be NULL
	DBG_ASSERT( ping_rpc == NULL );
	DBG_ASSERT( kad_closestnode == NULL );

	// copy the parameter
	this->remote_addr	= remote_addr;
	this->callback		= callback;
	this->userptr		= userptr;

	// ALGO:
	// - from the paper, "To join the network, a node u must have a contact 
	//   to an already participating node w"
	//   - this PING is used to determine if this candidate is online
	//   - if it fails, the candidate is considered offline
	//   - if it succeed, the candidate is considered online
	
	// launch a PING on this kad_addr with the local peerid as target
	delay_t		rpc_timeout	= kad_bstrap->profile.cand_ping_rpc_timeout();
	ping_rpc	= nipmem_new kad_ping_rpc_t();
	kad_err		= ping_rpc->start(kad_bstrap->kad_peer, remote_addr, rpc_timeout, this, NULL);
	if( kad_err.failed() )	return kad_err;

	// return no error
	return kad_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_ping_rpc_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_ping_rpc_t has an event to notify
 */
bool kad_bstrap_cand_t::neoip_kad_ping_rpc_cb(void *cb_userptr, kad_ping_rpc_t &cb_ping_rpc
					, const kad_event_t &kad_event)	throw()
{
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be ping_rpc_ok()
	DBG_ASSERT( kad_event.is_ping_rpc_ok() );
	// sanity check - here the kad_closestnode MUST be NULL
	DBG_ASSERT( !kad_closestnode );

	// if the query failed, notify the faillure 
	if( kad_event.is_fatal() )	return notify_callback(kad_err_t(kad_err_t::ERROR, kad_event.to_string()) );
	
	// sanity check - here the kad_event MUST be a is_completed()
	DBG_ASSERT( kad_event.is_completed() );
	
	// - from the paper, "[the bootstrapping node] performs a node lookup for its own node ID."
	kad_peer_t *		kad_peer= kad_bstrap->kad_peer;
	kad_targetid_t		targetid= kad_peer->local_peerid();
	const kad_profile_t & 	profile	= kad_peer->get_profile();
	// start a kad_closestnode on the local peerid
	kad_closestnode	= nipmem_new kad_closestnode_t();
	kad_err		= kad_closestnode->start(kad_peer, targetid, profile.kbucket().nb_replication()
						, kad_bstrap->profile.cand_closestnode_timeout()
						, this, NULL );
	if( kad_err.failed() )	return notify_callback(kad_err);

	
	// destroy the ping_rpc and mark it unused
	nipmem_zdelete	ping_rpc;

	// dont return 'tokeep' as the kad_ping_rpc_t has been deleted	
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_closestnode_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_closestnode_t has an event to notify
 */
bool 	kad_bstrap_cand_t::neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_closestnode
					, const kad_event_t &kad_event)		throw()
{
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be closestnode_ok()
	DBG_ASSERT( kad_event.is_closestnode_ok() );

	// destroy the kad_closestnode and mark it unused
	nipmem_zdelete kad_closestnode;
	
	// if the kad_closestnode failed, notify the faillure to the caller
	if( kad_event.is_fatal() )	return notify_callback( kad_err_t(kad_err_t::ERROR, kad_event.to_string()) );
	
	// notify the success of this kad_bstrap_cand_t to the caller
	return notify_callback(kad_err_t::OK);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 */
bool 	kad_bstrap_cand_t::notify_callback(const kad_err_t &kad_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_bstrap_cand_cb(userptr, *this, kad_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - it MUST have been deleted - specific to kad_bstrap_cand_t
	DBG_ASSERT( tokeep == false );
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;






