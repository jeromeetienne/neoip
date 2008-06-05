/*! \file
    \brief Definition of the \ref kad_db_replicate_t

*/

/* system include */
/* local include */
#include "neoip_kad_db_replicate.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_store_rpc.hpp"
#include "neoip_kad_findnode_rpc.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_db_replicate_t::kad_db_replicate_t(kad_db_t *kad_db)	throw()
{
	// copy the parameter
	this->kad_db	= kad_db;
}

/** \brief Constructor by default
 */
kad_db_replicate_t::~kad_db_replicate_t()	throw()
{
	// delete all the pending kad_findnode_rpc_t
	while( !findnode_rpc_db.empty() ){
		nipmem_delete	findnode_rpc_db.front();
		findnode_rpc_db.remove( findnode_rpc_db.front() );	
	}
	// delete all the pending kad_recdups_t in findnode_recdups_db
	while( !findnode_recdups_db.empty() ){
		nipmem_delete	findnode_recdups_db.front();
		findnode_recdups_db.remove( findnode_recdups_db.front() );	
	}
	// delete all the pending kad_store_rpc_t
	while( !store_rpc_db.empty() ){
		nipmem_delete	store_rpc_db.front();
		store_rpc_db.remove( store_rpc_db.front() );	
	}
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   replicate_in function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief do a STORE rpc of this record toward this particular node
 * 
 * - just a helper function on top the the replicate_in(kad_recdups_t)
 */
kad_err_t	kad_db_replicate_t::replicate_in(const kad_rec_t &kad_rec
					, const kad_addr_t &remote_addr)	throw()
{
	// create a kad_recdups which contains only this kad_rec
	kad_recdups_t	kad_recdups	= kad_recdups_t().update(kad_rec);
	// call the replicate_in for kad_recdups
	return replicate_in(kad_recdups, remote_addr);
}

/** \brief do a STORE rpc of all those records toward this particular node
 * 
 * - if cookie_store_ok(), it launches a dummy FINDNODE rpc first just to get the cookie and 
 *   then do the STORE rpc
 * - else it does the STORE rpc immediatly
 */
kad_err_t	kad_db_replicate_t::replicate_in(const kad_recdups_t &kad_recdups
					, const kad_addr_t &remote_addr)	throw()
{
	kad_peer_t *	kad_peer	= kad_db->kad_peer;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the remote_addr MUST be fully_qualified
	DBG_ASSERT( remote_addr.is_fully_qualified() );
	
	// Start a FINDNODE or a STORE depending on cookie_store_ok()
	if( kad_peer->get_profile().rpc().cookie_store_ok() ){
		return start_findnode_rpc(kad_recdups, remote_addr);
	}else{
		return start_store_rpc(kad_recdups, cookie_id_t(), remote_addr);
	}
	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       kad_findnode_rpc_t stuff
// - dummy stuff to get a valid cookie from the remote destination
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start a kad_findnode_rpc_t 
 */
kad_err_t	kad_db_replicate_t::start_findnode_rpc(const kad_recdups_t &kad_recdups
					, const kad_addr_t &remote_addr)	throw()
{
	kad_peer_t *	kad_peer	= kad_db->kad_peer;
	const kad_db_profile_t &profile = kad_db->get_profile();
	kad_findnode_rpc_t *findnode_rpc;
	kad_err_t	kad_err;
	// copy the kad_recdups and put its pointer in a database
	kad_recdups_t *	recdups_cpy	= nipmem_new kad_recdups_t(kad_recdups);
	// Start the kad_findnode_rpc_t with the recdups_cpy as userpointer
	findnode_rpc	= nipmem_new kad_findnode_rpc_t();
	kad_err		= findnode_rpc->start(kad_peer, remote_addr.get_peerid(), remote_addr
						, profile.replicate_findnode_rpc_timeout()
						, this, recdups_cpy);	
	// if the starting failed, delete the findnode_rpc and returned the error
	if( kad_err.failed() ){
		nipmem_delete	findnode_rpc;
		nipmem_delete	recdups_cpy;
		return kad_err;
	}
	// put this findnode_rpc to the findnode_rpc_db
	findnode_rpc_db.push_back( findnode_rpc );
	findnode_recdups_db.push_back(recdups_cpy);
	// return no error
	return kad_err_t::OK;
}


/** \brief callback notified when a kad_findnode_rpc_t has an event to notify
 */
bool kad_db_replicate_t::neoip_kad_findnode_rpc_cb(void *cb_userptr, kad_findnode_rpc_t &cb_findnode_rpc
						, const kad_event_t &kad_event)	throw()
{
	kad_peer_t *	kad_peer	= kad_db->kad_peer;	
	kad_findnode_rpc_t *findnode_rpc= &cb_findnode_rpc;
	kad_addr_t	remote_addr	= findnode_rpc->get_remote_addr();
	kad_recdups_t *	recdups_userptr	= (kad_recdups_t *)cb_userptr;
	kad_recdups_t	findnode_recdups;
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be findnode_rpc_ok()
	DBG_ASSERT( kad_event.is_findnode_rpc_ok() );
	// sanity check - here the kad_profile_t MUST be cookie_store_ok()
	DBG_ASSERT( kad_peer->get_profile().rpc().cookie_store_ok() );

	// copy the kad_recdups from userpointer and remove it from findnode_recdups_db
	findnode_recdups	= *recdups_userptr;
	// delete it and remove it from the database
	nipmem_delete	recdups_userptr;
	findnode_recdups_db.remove(recdups_userptr);

	// if FINDNODE RPC has been successfull, get the cookie and launch the actual STORE rpc
	if( !kad_event.is_fatal() ){
		// sanity check - here the event MUST be is_addr_arr()
		DBG_ASSERT( kad_event.is_addr_arr() );

		// get the cookie from the event - the kad_addr_arr is ignored
		cookie_id_t cookie_id;
		kad_event.get_addr_arr(&cookie_id);

		// launch the STORE RPC using this cookie_id
		start_store_rpc(findnode_recdups, cookie_id, remote_addr);
	}

	// remove this kad_findnode_rpc_t from the database and delete it
	findnode_rpc_db.remove( findnode_rpc );
	nipmem_delete	findnode_rpc;

	// NOTE: dont keep the rpc as it is deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       kad_store_rpc_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start a kad_store_rpc_t 
 */
kad_err_t	kad_db_replicate_t::start_store_rpc(const kad_recdups_t &kad_recdups
					, const cookie_id_t &cookie_id
					, const kad_addr_t &remote_addr)	throw()
{
	const kad_db_profile_t &profile = kad_db->get_profile();
	kad_peer_t *	kad_peer	= kad_db->kad_peer;
	kad_store_rpc_t*store_rpc;
	kad_err_t	kad_err;

	// create and start a kad_store_rpc_t
	store_rpc	= nipmem_new kad_store_rpc_t();
	kad_err		= store_rpc->start(kad_peer, kad_recdups, cookie_id, remote_addr
						, profile.replicate_store_rpc_timeout()
						, this, NULL);	
	// if the starting failed, delete the store_rpc and returned the error
	if( kad_err.failed() ){
		nipmem_delete	store_rpc;
		return kad_err;
	}	
	// link this store_rpc to the store_rpc_db
	store_rpc_db.push_back( store_rpc );
	// return no error
	return kad_err_t::OK;
}

/** \brief callback notified when a kad_store_rpc_t has an event to notify
 */
bool kad_db_replicate_t::neoip_kad_store_rpc_cb(void *cb_userptr, kad_store_rpc_t &cb_store_rpc
						, const kad_event_t &kad_event)	throw()
{
	kad_store_rpc_t*store_rpc	= &cb_store_rpc;
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the kad_event MUST be store_rpc_ok()
	DBG_ASSERT( kad_event.is_store_rpc_ok() );

	// NOTE: if the STORE succeed. no action to take


	// remove this kad_store_rpc_t from the database and delete it
	nipmem_delete	store_rpc;
	store_rpc_db.remove( store_rpc );

	// NOTE: dont keep the rpc as it is deleted
	return false;
}
NEOIP_NAMESPACE_END;






