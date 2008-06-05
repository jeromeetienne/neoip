/*! \file
    \brief Definition of the \ref kad_kbucket_t nested class bucklist_t

- in paper section 2.1, "least-recently seen node at the head,most-recently seen at the tail."
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_kbucklist.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_closestnode.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_kbucket_t::bucklist_t::bucklist_t(kad_kbucket_t *kad_kbucket)		throw()
{
	const kad_kbucket_profile_t &	profile = kad_kbucket->kad_peer->get_profile().kbucket();
	// copy the kad_kbucket
	this->kad_kbucket	= kad_kbucket;
	// reset the kad_closestnode
	kad_closestnode		= NULL;
	// to set the timeout delay
	refresh_timeout.start(profile.refresh_idle_bucklist_timeout(), this, NULL);
}

/** \brief Destructor
 */
kad_kbucket_t::bucklist_t::~bucklist_t()	throw()
{
	// delete the kad_closestnode if needed
	nipmem_zdelete kad_closestnode;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  utility function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if this peerid is present in the list
 */
bool	kad_kbucket_t::bucklist_t::peerid_is_present(const kad_peerid_t &peerid)	const throw()
{
	std::list<kad_kbucket_t::bucknode_t *>::const_iterator	iter;
	// scan the whole list
	for( iter = bucknode_lru.begin(); iter != bucknode_lru.end(); iter++ ){
		const kad_kbucket_t::bucknode_t *bucknode	= *iter;
		// if this bucknode's peerid matches the queried one, return true
		if( bucknode->get_kad_addr().get_peerid() == peerid )	return true;
	}
	// if no matching bucknode has been found, return false;
	return false;
}

/** \brief return true if this peerid is present in the list
 */
kad_kbucket_t::bucknode_t* kad_kbucket_t::bucklist_t::get_bucknode(const kad_peerid_t &peerid)	throw()
{
	std::list<kad_kbucket_t::bucknode_t *>::iterator	iter;
	// scan the whole list
	for( iter = bucknode_lru.begin(); iter != bucknode_lru.end(); iter++ ){
		kad_kbucket_t::bucknode_t *bucknode	= *iter;
		// if this bucknode's peerid matches the queried one, return its pointer
		if( bucknode->get_kad_addr().get_peerid() == peerid )	return bucknode;
	}
	// if no matching bucknode has been found, return NULL;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        refreshing
// - from the paper, "Refreshing means picking a random ID in the bucket’s
//   range and performing a no de search for that ID."
// - from the paper, "To handle pathological cases in which there are no 
//   lookups for a particular ID range, each no de refreshes any bucket
//   to which it has not performed a node lookup in the past hour."
// - from the paper, "[at the end of the bootstrap] Finally, u refreshes all k-buckets
//   further away than its closest neighbor."
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Called to notify a nsearch on a target id in this bucklist_t
 */
void	kad_kbucket_t::bucklist_t::notify_nsearch()	throw()
{
	const kad_kbucket_profile_t &	profile = kad_kbucket->kad_peer->get_profile().kbucket();
	// reset the timeout delay
	refresh_timeout.start(profile.refresh_idle_bucklist_timeout(), this, NULL);
}

/** \brief callback called when the timeout_t expire
 */
bool	kad_kbucket_t::bucklist_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout) throw()
{
	// trigger a refresh
	trigger_refresh_now();
	// keep the timeout running
	return true;
}

/** \brief Trigger a refresh now
 * 
 * - from the paper, "Refreshing means picking a random ID in the bucket’s
 *   range and performing a node search for that ID."
 */
void	kad_kbucket_t::bucklist_t::trigger_refresh_now() throw()
{
	const kad_kbucket_profile_t &	profile = kad_kbucket->kad_peer->get_profile().kbucket();
	kad_err_t			kad_err;
	// if it is already in refreshing, do nothing
	if( is_refreshing() )	return;
	
	// pick a random id on this range
	size_t		bucket_idx	= kad_kbucket->get_bucket_idx(this);
	size_t		kad_id_nbit	= kad_targetid_t().size() * 8;
	kad_targetid_t	rand_id;
	do{
		rand_id	= kad_targetid_t::build_random();
		rand_id	= rand_id.bit_clear_n_highest( kad_id_nbit - (bucket_idx+1)*kad_kbucket->kbucket_width );
		// loop until the rand_id is exactly in the proper bucket_idx
	}while( bucket_idx != rand_id.bit_get_highest_set()/kad_kbucket->kbucket_width );
	
	// launch the closestnode to refresh this bucket with no timeout
	kad_closestnode	= nipmem_new kad_closestnode_t();
	kad_err		= kad_closestnode->start(kad_kbucket->kad_peer
							, rand_id, kad_kbucket->kbucket_height
							, profile.refreshing_findnode_timeout()
							, this, NULL);
	if( kad_err.failed() )	nipmem_zdelete kad_closestnode;
}

/** \brief callback notified when a kad_closestnode_t has an event to notify
 */
bool 	kad_kbucket_t::bucklist_t::neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
					, const kad_event_t &kad_event)		throw()
{
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be closestnode_ok()
	DBG_ASSERT( kad_event.is_closestnode_ok() );

	// destroy the kad_closestnode and mark it unused
	nipmem_zdelete kad_closestnode;

	// NOTE: here the result is purposely ignored
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//               handle the node list itself
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Insert a bucknode_t at the tail of the list
 */
void	kad_kbucket_t::bucklist_t::insert(kad_kbucket_t::bucknode_t *bucknode)	throw()
{
	// sanity check - the bucknode peerid MUST NOT already be in the list
	DBG_ASSERT( !peerid_is_present(bucknode->get_kad_addr().get_peerid()) );
	// sanity check - the bucket_idx of the bucknode MUST be equal of this one
	DBG_ASSERT( kad_kbucket->get_bucket_idx(bucknode->get_kad_addr().get_peerid())
					== kad_kbucket->get_bucket_idx(this) );

	// insert this bucknode at the tail of the list
	bucknode_lru.push_back(bucknode);

	// sanity check - the size of the bucknode_lry MUST NOT be greater than k
	DBG_ASSERT( bucknode_lru.size() <= kad_kbucket->kbucket_height );
}

/** \brief remove this bucknode from its position in the list and put it at the tail
 */
void	kad_kbucket_t::bucklist_t::update(kad_kbucket_t::bucknode_t *bucknode)	throw()
{
	// sanity check - the bucknode peerid MUST be in the list
	DBG_ASSERT( peerid_is_present(bucknode->get_kad_addr().get_peerid()) );
	// sanity check - the size of the bucknode_lry MUST NOT be greater than k
	DBG_ASSERT( bucknode_lru.size() <= kad_kbucket->kbucket_height );
	// sanity check - the bucket_idx of the bucknode MUST be equal of this one
	DBG_ASSERT( kad_kbucket->get_bucket_idx(bucknode->get_kad_addr().get_peerid())
					== kad_kbucket->get_bucket_idx(this) );


	// remove the bucknode from its current position
	bucknode_lru.remove(bucknode);
	// add the bucknode at the tail of the list
	bucknode_lru.push_back(bucknode);
}

/** \brief remove a bucknode_t from the list
 */
void	kad_kbucket_t::bucklist_t::remove(kad_kbucket_t::bucknode_t *bucknode)	throw()
{
	// sanity check - the bucknode peerid MUST be in the list
	DBG_ASSERT( peerid_is_present(bucknode->get_kad_addr().get_peerid()) );
	// sanity check - the bucket_idx of the bucknode MUST be equal of this one
	DBG_ASSERT( kad_kbucket->get_bucket_idx(bucknode->get_kad_addr().get_peerid())
					== kad_kbucket->get_bucket_idx(this) );
	
	// remove this bucknode
	bucknode_lru.remove(bucknode);
}

/** \brief return the least-recently seen bucknode which isnt already pinging
 * 
 * - in paper section 2.1, "If the appropriate k-bucket is full, however, 
 *   then the recipient pings the k-bucket’s least-recently seen"
 */
kad_kbucket_t::bucknode_t *kad_kbucket_t::bucklist_t::get_first_notpinging()	throw()
{
	std::list<kad_kbucket_t::bucknode_t *>::iterator	iter;
	// scan the whole list from the least-recently seen to the most-recently seen
	for( iter = bucknode_lru.begin(); iter != bucknode_lru.end(); iter++ ){
		bucknode_t *	bucknode = *iter;
		// if this bucknode is unpinging, return ie
		if( !bucknode->ping_in_progress() )	return bucknode;
	}
	// if none had been found, return NULL
	return NULL;
}

/** \brief Return a bucknode of the idx index from the begining list
 */
kad_kbucket_t::bucknode_t *kad_kbucket_t::bucklist_t::get_node_idx(size_t idx)		throw()
{
	std::list<kad_kbucket_t::bucknode_t *>::iterator	iter;
	// sanity check - the list MUST be large enougth
	DBG_ASSERT( idx < bucknode_lru.size() );
	// scan the whole list from the least-recently seen to the most-recently seen
	for( iter = bucknode_lru.begin(); iter != bucknode_lru.end(); iter++ ){
		if( idx == 0 )	break;
		idx--;
	}
	// sanity check - the iterator MUST NOT reach the end
	DBG_ASSERT( iter != bucknode_lru.end() );
	// return the result
	return *iter;
}


/** \brief copy the content of this kbucket to this kad_addr_arr
 */
void kad_kbucket_t::bucklist_t::copy_to(kad_addr_arr_t &kad_addr_arr
				, kad_addr_t::filter_fct_t kad_addr_filter)	const throw()
{
	std::list<kad_kbucket_t::bucknode_t *>::const_iterator	iter;
	// scan the whole list from the least-recently seen to the most-recently seen
	for( iter = bucknode_lru.begin(); iter != bucknode_lru.end(); iter++ ){
		bucknode_t *	bucknode = *iter;
		// if this bucknode is filtered out by the kad_addr_filter, skip it
		if( kad_addr_filter && (*kad_addr_filter)(bucknode->get_kad_addr()) )	continue;
		// append this node to the list
		kad_addr_arr.append(bucknode->get_kad_addr());
	}
}
NEOIP_NAMESPACE_END;






