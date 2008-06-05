/*! \file
    \brief Declaration of the kad_delete_t

- from the paper, "To store a key,value pair, a participant locates 
  the k closest nodes to the key and sends them store RPCs."
  
*/

/* system include */
/* local include */
#include "neoip_kad_delete.hpp"
#include "neoip_kad_closestnode.hpp"
#include "neoip_kad_delete_rpc.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_caddr_arr.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_cookie.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_delete_t::kad_delete_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero the field
	kad_peer	= NULL;
	kad_closestnode	= NULL;
	nb_queried_peer	= 0;
	nb_unreach_peer	= 0;
}

/** \brief Desstructor
 */
kad_delete_t::~kad_delete_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all pending kad_delete_rpc_t
	while( !delete_rpc_db.empty() ){
		kad_delete_rpc_t *	delete_rpc	= delete_rpc_db.front();
		// remove the object from the database
		delete_rpc_db.pop_front();
		// delete the object itself
		nipmem_delete delete_rpc;
	}
	// delete kad_closestnode_t if needed
	nipmem_zdelete kad_closestnode;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                    Setup function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
kad_err_t	kad_delete_t::start(kad_peer_t *kad_peer, const kad_recid_t &kad_recid
				, const kad_keyid_t &kad_keyid, const delay_t &expire_delay
				, kad_delete_cb_t *callback, void *userptr) 		throw()
{
	const kad_profile_t &	profile	= kad_peer->get_profile();	
	kad_err_t		kad_err;
	// copy some parameters
	this->kad_peer	= kad_peer;
	this->kad_recid	= kad_recid;
	this->kad_keyid	= kad_keyid;
	this->callback	= callback;
	this->userptr	= userptr;
	
	// start the expire_timeout
	expire_timeout.start( expire_delay, this, NULL );

	// start the kad_closestnode_t
	kad_closestnode	= nipmem_new kad_closestnode_t();
	kad_err		= kad_closestnode->start(kad_peer, kad_keyid, profile.kbucket().nb_replication()
							,delay_t::NEVER, this, NULL);
	if( kad_err.failed() )	return kad_err;	

	// remove the records in the local database if needed
	kad_db_t *	local_db	= kad_peer->get_local_db();
	if( local_db->contain(kad_recid) )	local_db->remove(kad_recid);

	// return no error
	return kad_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         expire_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the expire_timeout expire
 */
bool	kad_delete_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// notify the timedout
	std::string	reason	= "kad_delete_t timedout";
	return notify_callback( kad_event_t::build_timedout(reason) );
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_closestnode_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_closestnode_t has an event to notify
 */
bool 	kad_delete_t::neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
					, const kad_event_t &kad_event)		throw()
{
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be closestnode_ok()
	DBG_ASSERT( kad_event.is_closestnode_ok() );

	// destroy the kad_closestnode and mark it unused
	nipmem_zdelete kad_closestnode;
			
	// sanity check - the kad_closestnode can't failed as the timeout is not set
	DBG_ASSERT( !kad_event.is_fatal() );
	// sanity check - the kad_event MUST be a caddr_arr
	DBG_ASSERT( kad_event.is_caddr_arr() );
	
	// get the n_closest node from the list
	const kad_caddr_arr_t &	nclosest_node	= kad_event.get_caddr_arr();

	// send a RPC to each of the n closest nodes
	for( size_t i = 0; i < nclosest_node.size(); i++ ){
		kad_delete_rpc_t*delete_rpc;
		kad_err_t	kad_err;
		kad_addr_t	remote_addr	= nclosest_node[i].get_kad_addr();
		cookie_id_t	cookie_id	= nclosest_node[i].get_cookie_id();
		// Start the kad_delete_rpc_t
		delete_rpc	= nipmem_new kad_delete_rpc_t();
		kad_err		= delete_rpc->start(kad_peer, kad_recid, cookie_id, remote_addr
							, expire_timeout.get_expire_delay().a_bit_less()
							, this, NULL);
		if( kad_err.failed() ){
			nipmem_delete delete_rpc;
			continue;
		}
		// link this kad_delete_rpc to the database
		delete_rpc_db.push_back( delete_rpc );
	}
	
	// if no STORE rpc succeed to start or if no node were provided, notify an error
	if( delete_rpc_db.empty() ){
		// notify the caller with a kad_event_t::TIMEDOUT 
		notify_callback( kad_event_t::build_timedout("No nodes available") );
		// NOTE: here the tokeep is ignored, as the rpc is deleted anyway
		return false;
	}
	
	// backup the size of delete_rpc_db in nb_queried_peer
	nb_queried_peer	= delete_rpc_db.size();
	// return dontkeep
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_delete_rpc_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_delete_rpc_t has an event to notify
 */
bool kad_delete_t::neoip_kad_delete_rpc_cb(void *cb_userptr, kad_delete_rpc_t &cb_kad_delete_rpc
					, const kad_event_t &kad_event)	throw()
{
	kad_delete_rpc_t*delete_rpc	= &cb_kad_delete_rpc;
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be delete_rpc_ok()
	DBG_ASSERT( kad_event.is_delete_rpc_ok() );	

	// remove this delete_rpc from the database
	delete_rpc_db.remove( delete_rpc );
	nipmem_zdelete	delete_rpc;

	// if this STORE rpc failed, update the nb_unreach_peer
	if( kad_event.is_fatal() )	nb_unreach_peer++;

	// if all the delete_rpc have been handled, notify the result now
	if( delete_rpc_db.empty() ){
		kad_event_t	event	= kad_event_t::build_completed();
		// if all kad_delete_rpc_t have reported fatal event, return a TIMEDOUT 
		// - TODO this is an inproper event for this case
		if( nb_queried_peer == nb_unreach_peer )
			event = kad_event_t::build_timedout("All rpc failed!");
		// notify the event
		notify_callback(event);
		// NOTE: here the tokeep is ignored, as the rpc is deleted anyway
		return false;
	}

	// NOTE: dont keep the rpc as it is deleted
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	kad_delete_t::notify_callback(const kad_event_t &kad_event)	throw()
{
	// if the callback is NULL, autodelete
	if( !callback ){
		nipmem_delete this;
		return false;
	}	
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_delete_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END


