/*! \file
    \brief Definition of the \ref kad_nsearch_lnode_t
 
*/

/* system include */
/* local include */
#include "neoip_kad_nsearch_lnode.hpp"
#include "neoip_kad_nsearch.hpp"
#include "neoip_kad_findnode_rpc.hpp"
#include "neoip_kad_findsomeval_rpc.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

	
/** \brief Constructor
 */
kad_nsearch_lnode_t::kad_nsearch_lnode_t(kad_nsearch_llist_t *llist, const kad_addr_t &m_kad_addr)throw()
{
	// log to debug
	KLOG_DBG("Creating a lnode_t with " << m_kad_addr);
	// copy some parameters
	this->llist		= llist;
	this->kad_nsearch	= llist->kad_nsearch;
	this->m_kad_addr	= m_kad_addr;
	DBG_ASSERT( kad_addr().oaddr().is_fully_qualified() );
	// init the rpc
	rpc_birthdate		= date_t();
	findnode_rpc		= NULL;
	findsomeval_rpc		= NULL;
	// link this lnode_t to the list
	llist->lnode_link(this);
}

/** \brief Destructor
 */
kad_nsearch_lnode_t::~kad_nsearch_lnode_t()	throw()
{
	// log to debug
	KLOG_DBG("Destroying a lnode_t with " << kad_addr);
	// unlink this lnode_t from the list
	kad_nsearch->llist->lnode_unlink(this);
	// if a rpc is in progress, delete it
	nipmem_zdelete findnode_rpc;
	nipmem_zdelete findsomeval_rpc;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          UTILITY function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return true if the node has provided an answer
 * 
 * - used to estimate if the kad_nsearch_t is completed
 * - from the paper, "The lookup terminates when the initiator has queried 
 *   and gotten responses from the k closest nodes it has seen."
 */
bool	kad_nsearch_lnode_t::is_online()	const throw()
{
	// if the current_state is SUCCEED, it is considered online
	if( current_state() == state_t::SUCCEED )	return true;
	// else it is NOT - it may be offline or undertermined
	return false;
}

/** \brief return true if the node is to be considered offline in the node search
 * 
 * - NOTE: be carefull a node online'ness may be undetermined if is in null
 *         or QUERYING
 *         - in this case, is_online() AND is_offline() are both false at the same time
 */
bool	kad_nsearch_lnode_t::is_offline()	const throw()
{
	const kad_nsearch_profile_t &	profile	= kad_nsearch->kad_peer->get_profile().nsearch();
	// if a query failed, dont considere this node
	if( current_state() == state_t::FAILED )	
		return true;
	// from the paper, "Nodes that fail to respond quickly are removed from
	// consideration until and unless they do respond."
	if( current_state() == state_t::QUERYING && rpc_age() > profile.rpccli_soft_timeout())
		return true;
	// in all other cases, the result is undetermined
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     query_start
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start query this node
 */
kad_err_t kad_nsearch_lnode_t::start()	throw()
{
	const kad_nsearch_profile_t &	profile	= kad_nsearch->kad_peer->get_profile().nsearch();	
	kad_peer_t *			kad_peer= kad_nsearch->kad_peer;
	kad_err_t			kad_err;
	// sanity check - the current_state MUST be is_null()
	DBG_ASSERT( current_state().is_null() );

	// log to debug
	KLOG_DBG("Start querying node " << kad_addr);

	// pass the state to querying
	m_current_state	= state_t::QUERYING;

	// set the rpc_birthdate
	rpc_birthdate	= date_t::present();

	// create the query rpc
	// start the query according to the query_type
	const kad_pkttype_t &	query_type	= kad_nsearch->query_type;	
	if( query_type.is_FINDNODE_REQUEST() ){
		findnode_rpc	= nipmem_new kad_findnode_rpc_t();
		kad_err		= findnode_rpc->start(kad_peer, kad_nsearch->target_id, kad_addr()
						, profile.rpccli_hard_timeout(), this, NULL);
	}else if( query_type.is_FINDSOMEVAL_REQUEST() ){
		findsomeval_rpc	= nipmem_new kad_findsomeval_rpc_t();
		kad_err		= findsomeval_rpc->start(kad_peer, kad_nsearch->target_id
						, llist->findsome_max_nb_rec, kad_addr()
						, profile.rpccli_hard_timeout(), this, NULL);
	}else{
		DBG_ASSERT( 0 );
	}

	// if the start failed, delete the rpc and mark it unused
	if( kad_err.failed() ){
		// delete the rpc and mark it unused
		nipmem_zdelete findnode_rpc;
		nipmem_zdelete findsomeval_rpc;
		// pass the state to failed
		m_current_state	= state_t::FAILED;
		// return the error
		return kad_err;
	}
	// return no error
	return kad_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     query callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_findnode_t has an event to notify
 */
bool kad_nsearch_lnode_t::neoip_kad_findnode_rpc_cb(void *cb_userptr
					, kad_findnode_rpc_t &cb_findnode_rpc
					, const kad_event_t &kad_event)	throw()
{
	return rpc_event_cb(kad_event);
}

/** \brief callback notified when a kad_findsomeval_t has an event to notify
 */
bool kad_nsearch_lnode_t::neoip_kad_findsomeval_rpc_cb(void *cb_userptr
					, kad_findsomeval_rpc_t &cb_findsomeval_rpc
					, const kad_event_t &kad_event)	throw()
{
	return rpc_event_cb(kad_event);
}

/** \brief callback notified when any rpc has an event to notify
 */
bool kad_nsearch_lnode_t::rpc_event_cb(const kad_event_t &kad_event)	throw()
{
	const kad_nsearch_profile_t &	profile	= kad_nsearch->kad_peer->get_profile().nsearch();	
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	
	// sanity check - the state MUST be QUERYING when receiving an event from RPC
	DBG_ASSERT( current_state() == state_t::QUERYING );
	
	// delete the rpc and mark it unused
	nipmem_zdelete	findnode_rpc;
	nipmem_zdelete	findsomeval_rpc;
	
	// if the rpc failed, set the state to state_t::FAILED
	if( kad_event.is_fatal() ){
		// set the current_state to FAILED
		m_current_state	= state_t::FAILED;
		// log to debug
		KLOG_INFO("rpc to" << kad_addr() << " failed due to " << kad_event);
	}else{
		// set the current_state to state_t::SUCCEED
		m_current_state	= state_t::SUCCEED;
		// Handle the reply according to the query_type
		bool	tokeep	= handle_reply(kad_event);
		if( !tokeep )	return false;
	}

	// If the kad_nsearch is now completed, notify the caller
	if( llist->nsearch_is_completed() ){
		llist->notify_end_result();
		// NOTE: here the tokeep is ignored, as the rpc is deleted anyway
		return false;
	}

	// compute the number of running query wished - either alpha or k depending on previous results
	// - from the paper, "If a round of find nodes fails to return a node any closer than
	//   the closest already seen, the initiator resends the find node to all of the k 
	//   closest nodes it has not already queried."	
	size_t	nb_wished_query;
	if( llist->nb_succ_useless_query >= profile.nb_concurrent_req() ){
		nb_wished_query	= kad_nsearch->n_closest;
	}else{
		nb_wished_query	= profile.nb_concurrent_req();
	}

	// count the number of valid query currently running
	size_t	nb_querying	= llist->get_nb_querying_node();
	// if the number of running query is already enougth, do nothing
	if( nb_querying >= nb_wished_query )	return false;
	// launch as many query as necessary to get nb_wished_query running
	llist->launch_queries( nb_wished_query - nb_querying );

	// dont return 'tokeep' as the RPC has been deleted
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    reply processing
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle a successfull reply
 * 
 * - just a forward the kad_event to the handler specific to the query_type
 */
bool kad_nsearch_lnode_t::handle_reply(const kad_event_t &kad_event)		throw()
{
	// Handle the reply according to the query_type	
	const kad_pkttype_t &	query_type	= kad_nsearch->query_type;
	if( query_type.is_FINDNODE_REQUEST() )		return handle_findnode_reply(kad_event);
	else if( query_type.is_FINDSOMEVAL_REQUEST() )	return handle_findsomeval_reply(kad_event);
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;	
}

/** \brief Handle the reply of a sucessfull FINDNODE rpc
 * 
 * @return a 'tokeep' aka false if the kad_nsearch_t has been deleted, true otherwise
 */
bool kad_nsearch_lnode_t::handle_findnode_reply(const kad_event_t &kad_event)	throw()
{
	kad_addr_arr_t	kad_addr_arr;
	cookie_id_t	rpc_cookie;	
	// sanity check - the event MUST be findnode_ok()
	DBG_ASSERT( kad_event.is_findnode_rpc_ok() );
	DBG_ASSERT( !kad_event.is_fatal() );

	// get the kad_addr_arr+cookie from the event
	kad_addr_arr	= kad_event.get_addr_arr(&m_cookie_id);
	// log to debug
	KLOG_DBG("kad_addr_arr=" << kad_addr_arr << " cookie_id=" << cookie_id());
	// process the replied kad_addr_arr
	process_replied_addr_arr(kad_addr_arr);
	// return 'tokeep'
	return true;
}

/** \brief Handle the reply of a sucessfull FINDSOMEVAL rpc
 * 
 * @return a 'tokeep' aka false if the kad_nsearch_t has been deleted, true otherwise
 */
bool kad_nsearch_lnode_t::handle_findsomeval_reply(const kad_event_t &kad_event)	throw()
{
	const kad_nsearch_profile_t &	profile	= kad_nsearch->kad_peer->get_profile().nsearch();	
	// sanity check - the event MUST be findsomeval_ok()
	DBG_ASSERT( kad_event.is_findsomeval_rpc_ok() );
	DBG_ASSERT( !kad_event.is_fatal() );

	// if the FINDONEVAL returned value, notify them to the caller
	// - from the paper, "the [value retrieval] procedure halts immediately when
	//   any node returns the value."
	if( kad_event.is_recdups() ){
		kad_db_t *	remote_db = kad_nsearch->kad_peer->get_remote_db();
		// if there is a node on which to cache, replicate the record to this node
		if( profile.caching_enable() && !llist->caching_candidate.is_null() ){
			bool	dummy;
			kad_recdups_t	kad_recdups	= kad_event.get_recdups(&dummy);
			remote_db->replicate_in(kad_recdups, llist->caching_candidate);
		}
		// forward the recdups event to the caller
		return kad_nsearch->notify_callback(kad_event);		
	}else{
		kad_addr_arr_t kad_addr_arr;
		// get the kad_addr_arr+cookie from the event
		kad_addr_arr	= kad_event.get_addr_arr(&m_cookie_id);
	
		// process the replied kad_addr_arr
		process_replied_addr_arr(kad_addr_arr);
		
		// update the last node which reported not to have the record
		// - from the paper, "For caching purposes, once a node search succeeds, the
		//   requesting node stores the (key,value) pair at the closest node it
		//   observed to the key that did not return the value."
		if( profile.caching_enable() ){
			kad_targetid_t&	target_id = kad_nsearch->target_id;
			kad_addr_t &	prev_cand = llist->caching_candidate;
			kad_addr_t	curr_cand = kad_addr();
			// compute the distance between the current candidate and the target_id
			kad_targetid_t	curr_dist = target_id ^ curr_cand.get_peerid();
			// if no other node is candidate for caching or if the previous candidate
			// is further than the current node, put the current node as candidate
			if( prev_cand.is_null() || curr_dist < (target_id ^ prev_cand.get_peerid()) )
				llist->caching_candidate	= curr_cand;
		}
	}
	// return 'tokeep'
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Handle a replied kad_addr_arr from any type of RPC
 */
void kad_nsearch_lnode_t::process_replied_addr_arr(const kad_addr_arr_t &replied_addr_arr)	throw()
{
	// log to debug
	KLOG_DBG("rpc to " << kad_addr() << " succeed." << " So updating learned node with "
							<< replied_addr_arr );

	// update the learnednode database with the replied node list
	bool	got_closer_node = llist->merge_addr_arr( replied_addr_arr );

	// - update the counter of query failing to provide nodes closer 
	//   than the closest already seen
	//   - from the paper, "If a round of find nodes fails to return a node any closer than
	//     the closest already seen, the initiator resends the find node to all of the k 
	//     closest nodes it has not already queried."
	if( got_closer_node )	llist->nb_succ_useless_query = 0;
	else			llist->nb_succ_useless_query++;
}


NEOIP_NAMESPACE_END;






