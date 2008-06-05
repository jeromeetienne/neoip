/*! \file
    \brief Declaration of the kad_query_t

- from the paper, "To store a key,value pair, a participant locates 
  the k closest nodes to the key and sends them store RPCs."
  
*/

/* system include */
/* local include */
#include "neoip_kad_query.hpp"
#include "neoip_kad_closestnode.hpp"
#include "neoip_kad_findallval_rpc.hpp"
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
kad_query_t::kad_query_t()		throw()
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
kad_query_t::~kad_query_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all pending kad_findallval_rpc_t
	while( !findallval_rpc_db.empty() ){
		kad_findallval_rpc_t *	findallval_rpc	= findallval_rpc_db.front();
		// remove the object from the database
		findallval_rpc_db.pop_front();
		// delete the object itself
		nipmem_delete findallval_rpc;
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
kad_err_t	kad_query_t::start(kad_peer_t *kad_peer, const kad_keyid_t &kad_keyid
					, size_t max_nb_record, const delay_t &expire_delay
					, kad_query_cb_t *callback, void *userptr) 		throw()
{
	const kad_profile_t &	profile	= kad_peer->get_profile();	
	kad_err_t		kad_err;
	// log to debug
	KLOG_DBG("enter callback " << neoip_cpp_typename(*callback) );		
	// copy some parameters
	this->kad_peer		= kad_peer;
	this->kad_keyid		= kad_keyid;
	this->max_nb_record	= max_nb_record;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// start the expire_timeout
	expire_timeout.start( expire_delay, this, NULL );

	// start the kad_closestnode_t
	// - from the paper, "To store a key,value pair, a participant locates 
	//   the k closest nodes to the key and sends them store RPCs."	
	kad_closestnode	= nipmem_new kad_closestnode_t();
	kad_err		= kad_closestnode->start(kad_peer, kad_keyid, profile.kbucket().nb_replication()
							, delay_t::NEVER, this, NULL);
	if( kad_err.failed() )	return kad_err;	

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
bool	kad_query_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// notify the timedout
	std::string	reason	= "kad_query_t timedout";
	return notify_callback( kad_event_t::build_timedout(reason) );
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_closestnode_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_closestnode_t has an event to notify
 */
bool 	kad_query_t::neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
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
		kad_findallval_rpc_t*findallval_rpc;
		kad_err_t	kad_err;
		kad_addr_t	remote_addr	= nclosest_node[i].get_kad_addr();
		cookie_id_t	cookie_id	= nclosest_node[i].get_cookie_id();
		// Start the kad_findallval_rpc_t
		findallval_rpc	= nipmem_new kad_findallval_rpc_t();
		kad_err		= findallval_rpc->start(kad_peer, kad_recid_t(), kad_keyid, false
							, max_nb_record, remote_addr
							, expire_timeout.get_expire_delay().a_bit_less()
							, this, NULL);
		if( kad_err.failed() ){
			nipmem_delete findallval_rpc;
			continue;
		}
		// link this kad_findallval_rpc to the database
		findallval_rpc_db.push_back( findallval_rpc );
	}
	
	// if no STORE rpc succeed to start or if no node were provided, notify an error
	if( findallval_rpc_db.empty() ){
		// notify the caller with a kad_event_t::TIMEDOUT 
		notify_callback( kad_event_t::build_timedout("No nodes available") );
		// NOTE: here the tokeep is ignored, as the rpc is deleted anyway
		return false;
	}
	
	// backup the size of findallval_rpc_db in nb_queried_peer
	nb_queried_peer	= findallval_rpc_db.size();
	// return dontkeep
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_findallval_rpc_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_findallval_rpc_t has an event to notify
 */
bool kad_query_t::neoip_kad_findallval_rpc_cb(void *cb_userptr, kad_findallval_rpc_t &cb_kad_findallval_rpc
					, const kad_event_t &kad_event)	throw()
{
	kad_findallval_rpc_t*	findallval_rpc	= &cb_kad_findallval_rpc;
	kad_addr_t		remote_addr	= findallval_rpc->get_remote_addr();
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be findallval_rpc_ok()
	DBG_ASSERT( kad_event.is_findallval_rpc_ok() );	

	// remove this findallval_rpc from the database
	findallval_rpc_db.remove( findallval_rpc );
	nipmem_zdelete	findallval_rpc;

	// if this rpc failed, update the nb_unreach_peer
	if( kad_event.is_fatal() )	nb_unreach_peer++;

	// if the notified kad_event_t is a RECDUPS, handle it
	if( kad_event.is_recdups() ){
		bool	tokeep = handle_recved_recdups(kad_event, remote_addr);
		if( !tokeep )	return false;
	}


	// if all the findallval_rpc have been handled, notify the result now
	if( findallval_rpc_db.empty() ){
		kad_event_t	event	= kad_event_t::build_recdups(result_recdups, false);
		// if all kad_findallval_rpc_t have reported fatal event, return a TIMEDOUT 
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


/** \brief Process the event when it is for a FINDALLVAL rpc and it isnt fatal
 * 
 * @return a 'tokeep' on the kad_findallval_rpc_t
 */
bool	kad_query_t::handle_recved_recdups(const kad_event_t &kad_event
					, const kad_addr_t &remote_addr)	throw()
{
	// sanity check - here the event MUST be is_findallval_rpc_ok() and is_recdups_ok()
	DBG_ASSERT(kad_event.is_findallval_rpc_ok());
	DBG_ASSERT(kad_event.is_recdups());

	// get the found record and the 'has_more_record' flags
	bool			has_more_record;
	const kad_recdups_t &	kad_recdups	= kad_event.get_recdups(&has_more_record);

	// put the found record with the already found ones
	result_recdups.update(kad_recdups);
	// log to debug
	KLOG_DBG("result_recdups="		<< result_recdups);
	KLOG_DBG("add recdups from rpc event="	<< kad_recdups);

	// if too many records have been found, truncate the result_recdups and notify the caller now
	if( max_nb_record && result_recdups.size() >= max_nb_record ){
		// set report_has_more to the one just received by the event
		bool	report_has_more	= has_more_record;
		// If the result_recdups contains too many records, truncate it 
		// and set 'has_more_record' whatever the just received one
		if( max_nb_record && result_recdups.size() > max_nb_record ){
			result_recdups.truncate_at_tail(result_recdups.size() - max_nb_record);
			report_has_more	= true;
		}
		// notify the caller, with has_more_record set to true as more records are available
		return notify_callback( kad_event_t::build_recdups(result_recdups, report_has_more) );
	}

	// if this node has more record, relaunch a query
	if( has_more_record ){
		kad_findallval_rpc_t *	findallval_rpc;
		kad_err_t		kad_err;
		kad_recid_t		recid_gt;
		// compute the maximum number of records to request
		DBG_ASSERT( max_nb_record && result_recdups.size() <= max_nb_record );
		size_t	max_nb_rec_to_request	= result_recdups.size() - max_nb_record;
		// sanity check - if the remote node has more records, this kad_recdups MUST NOT be empty
		DBG_ASSERT( kad_recdups.size() );
		// compute the next recid_gt
		recid_gt	= kad_recdups[kad_recdups.size()-1].get_recid();
		// init the rpc
		findallval_rpc	= nipmem_new kad_findallval_rpc_t();	
		// start a FINDALLVAL rpc with a null recid and a false keyid_ge
		// - keyid_ge is made only to dump the whole db of the remote.
		kad_err		= findallval_rpc->start(kad_peer, recid_gt, kad_keyid, false
							, max_nb_rec_to_request, remote_addr
							, expire_timeout.get_expire_delay().a_bit_less()
							, this, NULL);	
		// if starting this rpc failed, delete it else link it to the database
		if( kad_err.failed() )	nipmem_delete findallval_rpc;
		else			findallval_rpc_db.push_back( findallval_rpc );
	}
	// return 'tokeep'
	return true;
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
bool 	kad_query_t::notify_callback(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_query_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END


