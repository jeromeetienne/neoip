/*! \file
    \brief Declaration of the kad_store_t

- from the paper, "To store a key,value pair, a participant locates 
  the k closest nodes to the key and sends them store RPCs."
  
*/

/* system include */
/* local include */
#include "neoip_kad_store.hpp"
#include "neoip_kad_closestnode.hpp"
#include "neoip_kad_store_rpc.hpp"
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
kad_store_t::kad_store_t()		throw()
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
kad_store_t::~kad_store_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all pending kad_store_rpc_t
	while( !store_rpc_db.empty() ){
		kad_store_rpc_t *	store_rpc	= store_rpc_db.front();
		// remove the object from the database
		store_rpc_db.pop_front();
		// delete the object itself
		nipmem_delete store_rpc;
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
 * 
 * @param local_storage	true if the kad_recdups_t must be stored in the local database, false
 * 		        otherwise. This is usefull when kad_db_t use kad_store_t to republish its
 *                      records, to just do the network part of the kad_store_t without changing
 *                      the local database.
 */
kad_err_t	kad_store_t::start(kad_peer_t *kad_peer, const kad_recdups_t &kad_recdups
				, const delay_t &expire_delay
				, kad_store_cb_t *callback, void *userptr, bool local_storage) throw()
{
	const kad_profile_t &	profile	= kad_peer->get_profile();	
	kad_err_t		kad_err;
	// copy some parameters
	this->kad_peer		= kad_peer;
	this->kad_recdups	= kad_recdups;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// start the expire_timeout
	expire_timeout.start( expire_delay, this, NULL );

	// start the kad_closestnode_t
	// - from the paper, "To store a key,value pair, a participant locates 
	//   the k closest nodes to the key and sends them store RPCs."	
	kad_closestnode	= nipmem_new kad_closestnode_t();
	kad_err		= kad_closestnode->start(kad_peer, kad_recdups.get_keyid()
							, profile.kbucket().nb_replication()
							, delay_t::NEVER, this, NULL);
	if( kad_err.failed() )	return kad_err;	

	// put the records in the local database - IIF local_storage == true
	if( local_storage ){
		kad_err		= store_local_db(kad_recdups);
		if( kad_err.failed() )	return kad_err;	
	}

	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  Publish in Local Database
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Publish the kad_recdups_t in the local database
 */
kad_err_t	kad_store_t::store_local_db(const kad_recdups_t &kad_recdups)	const throw()
{
	kad_db_t *	local_db	= kad_peer->get_local_db();
	
	// check if all the records are allowed in the local database
	if( !local_db->may_accept(kad_recdups) )
		return kad_err_t(kad_err_t::ERROR, "Record not acceptable in the local db");

	// remove all the records which are already in the local database
	// - thus the new record MAY NOT loose the tie with the previous one
	//   and is GARANTEED to be inserted.
	for(size_t i = 0; i < kad_recdups.size(); i++ ){
		const kad_rec_t &	kad_rec = kad_recdups[i];
		// if this kad_recid_t is not in the database, skip it
		if( !local_db->contain( kad_rec.get_recid() ) )	continue;
		// remove this kad_recid_t from the database
		local_db->remove( kad_rec.get_recid() );
	}

	// put it in the local database
	local_db->update(kad_recdups);

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
bool	kad_store_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// notify the timedout
	std::string	reason	= "kad_store_t timedout";
	return notify_callback( kad_event_t::build_timedout(reason) );
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_closestnode_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_closestnode_t has an event to notify
 */
bool 	kad_store_t::neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
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

	// - from the paper, "To store a key,value pair, a participant locates 
	//   the k closest nodes to the key and sends them store RPCs."
	//   - the kad_closestnode located the k closest nodes, now a STORE rpc is sent to each of them

	// send a RPC to each of the n closest nodes
	for( size_t i = 0; i < nclosest_node.size(); i++ ){
		kad_store_rpc_t*store_rpc;
		kad_err_t	kad_err;
		kad_addr_t	remote_addr	= nclosest_node[i].get_kad_addr();
		cookie_id_t	cookie_id	= nclosest_node[i].get_cookie_id();
		// Start the kad_store_rpc_t
		store_rpc	= nipmem_new kad_store_rpc_t();
		kad_err		= store_rpc->start(kad_peer, kad_recdups, cookie_id, remote_addr
							, expire_timeout.get_expire_delay().a_bit_less()
							, this, NULL);
		if( kad_err.failed() ){
			nipmem_delete store_rpc;
			continue;
		}
		// link this kad_store_rpc to the database
		store_rpc_db.push_back( store_rpc );
	}
	
	// if no STORE rpc succeed to start or if no node were provided, notify an error
	if( store_rpc_db.empty() ){
		// notify the caller with a kad_event_t::TIMEDOUT 
		notify_callback( kad_event_t::build_timedout("No nodes available") );
		// NOTE: here the tokeep is ignored, as the rpc is deleted anyway
		return false;
	}
	
	// backup the size of store_rpc_db in nb_queried_peer
	nb_queried_peer	= store_rpc_db.size();
	// return dontkeep
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_store_rpc_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_store_rpc_t has an event to notify
 */
bool kad_store_t::neoip_kad_store_rpc_cb(void *cb_userptr, kad_store_rpc_t &cb_kad_store_rpc
					, const kad_event_t &kad_event)	throw()
{
	kad_store_rpc_t*store_rpc	= &cb_kad_store_rpc;
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be store_rpc_ok()
	DBG_ASSERT( kad_event.is_store_rpc_ok() );	

	// remove this store_rpc from the database
	store_rpc_db.remove( store_rpc );
	nipmem_zdelete	store_rpc;

	// if this STORE rpc failed, update the nb_unreach_peer
	if( kad_event.is_fatal() )	nb_unreach_peer++;

	// if all the store_rpc have been handled, notify the result now
	if( store_rpc_db.empty() ){
		kad_event_t	event	= kad_event_t::build_completed();
		// if all kad_store_rpc_t have reported fatal event, return a TIMEDOUT 
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
bool 	kad_store_t::notify_callback(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_store_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END


