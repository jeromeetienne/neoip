/*! \file
    \brief Definition of the \ref kad_nsearch_t
    
*/

/* system include */
/* local include */
#include "neoip_kad_nsearch.hpp"
#include "neoip_kad_nsearch_llist.hpp"
#include "neoip_kad_nsearch_lnode.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_caddr.hpp"
#include "neoip_kad_caddr_arr.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor by default
 */
kad_nsearch_t::kad_nsearch_t(kad_nsearch_cb_t *callback, void *userptr, kad_peer_t *kad_peer)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->kad_peer	= kad_peer;
	this->callback	= callback;
	this->userptr	= userptr;
	// create the list of learned node
	llist		= nipmem_new kad_nsearch_llist_t(this);
}

/** \brief Destructor by default
 */
kad_nsearch_t::~kad_nsearch_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the list of learned node
	nipmem_delete	llist;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief start a node search using FINDNODE rpc
 */
kad_err_t kad_nsearch_t::start_findnode(const kad_peerid_t &peerid, size_t n_closest
					, const delay_t &timeout_delay
					, const kad_addr_arr_t &base_addr_arr) 	throw()
{
	// copy the parameter
	target_id	= peerid;
	// set the query_type	
	query_type	= kad_pkttype_t(kad_peer->get_profile().pkttype()).FINDNODE_REQUEST();
	// call the post start() function
	return post_start(n_closest, timeout_delay, base_addr_arr);
}

/** \brief start a node search using FINDSOMEVAL rpc
 */
kad_err_t kad_nsearch_t::start_findsomeval(const kad_keyid_t &keyid, size_t max_nb_record
					, size_t n_closest, const delay_t &timeout_delay
					, const kad_addr_arr_t &base_addr_arr) 	throw()
{
	// copy the parameter
	target_id			= keyid;
	llist->findsome_max_nb_rec	= max_nb_record;
	// set the query_type	
	query_type	= kad_pkttype_t(kad_peer->get_profile().pkttype()).FINDSOMEVAL_REQUEST();
	// call the post start() function
	return post_start(n_closest, timeout_delay, base_addr_arr);
}

/** \brief Post processing for all start()
 */
kad_err_t kad_nsearch_t::post_start(size_t n_closest, const delay_t &timeout_delay
					, const kad_addr_arr_t &base_addr_arr) 	throw()
{
	const kad_nsearch_profile_t &	profile	= kad_peer->get_profile().nsearch();
	kad_kbucket_t *			kbucket = kad_peer->get_kbucket();
	kad_addr_arr_t	init_addr_arr;
	// sanity check - here the query_type MUST NOT be null
	DBG_ASSERT( !query_type.is_null() );
	// copy the n_closest
	this->n_closest	= n_closest;

	// notify a nsearch of this target_id to the kbucket
	// - from the paper "To handle pathological cases in which there are no 
	//   lookups for a particular ID range, each node refreshes any bucket
	//   to which it has not performed a node lookup in the past hour."
	kbucket->notify_nsearch(target_id);
	
	// if the base_addr_arr is empty, get the n_closest closest nodes to the target_id
	// - it could be populated by the caller. e.g. the bootstrapping uses this feature
	if( base_addr_arr.empty() ){
		init_addr_arr	= kbucket->get_nclosest_addr(target_id, n_closest, NULL);
		/** NOTE: about an issue in the paper
		 * - from the paper, "The lookup initiator starts by picking alpha nodes
		 *   from its closest non-empty k-bucket (or, if that bucket has fewer than
		 *   alpha entries, it just takes the alpha closest nodes it knows of )."
		 *   - this algo will fail if those alpha nodes are off-line.
		 * - here the k closest nodes are taken instead
		 *   - only the k closest nodes are very likely to have at least one
		 *     still online. Only k is tuned according to this assumption. 
		 *     alpha doesn't provide such waranty.
		 */
	}else{	// if a base_addr_arr is provided, use it as initial kad_addr_arr
		init_addr_arr	= base_addr_arr;
	}
	// log to debug
	KLOG_DBG("init_addr_arr=" << init_addr_arr);
	// initial population of learnednode_db with the init_addr_arr
	llist->merge_addr_arr( init_addr_arr );

	// to set the timeout delay for the whole nsearch
	expire_timeout.start(timeout_delay, this, NULL);

	// launch the alpha first query
	llist->launch_queries( profile.nb_concurrent_req() );

	// return no error
	return kad_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         expire_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	kad_nsearch_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// notify the timedout
	std::string	reason		= "NodeSearch with " + query_type.to_string() + " timedout";
	kad_event_t	kad_event	= kad_event_t::build_timedout(reason);
	return notify_callback(kad_event);
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
bool 	kad_nsearch_t::notify_callback(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_nsearch_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END;






