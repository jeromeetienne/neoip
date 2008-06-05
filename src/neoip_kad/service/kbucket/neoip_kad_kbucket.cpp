/*! \file
    \brief Definition of the \ref kad_kbucket_t

- TODO a lot of beautifullation in this source

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_kbucket.hpp"
#include "neoip_kad_kbucknode.hpp"
#include "neoip_kad_kbucklist.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor by default
 */
kad_kbucket_t::kad_kbucket_t(kad_peer_t *kad_peer)	throw()
{
	const kad_kbucket_profile_t &	profile	= kad_peer->get_profile().kbucket();
	// copy the kad_peer backpointer
	this->kad_peer	= kad_peer;
	// init the kbucket_width
	kbucket_width	= profile.kbucket_width();
	// init the kbucket_height
	kbucket_height	= profile.nb_replication();
	// sanity check - the KBUCKET_WIDTH MUST be a multiple of the size in bit of kad_targetid_t
	size_t	kad_id_nbit = kad_targetid_t().size() * 8;
	DBG_ASSERT( (kad_id_nbit/kbucket_width) * kbucket_width == kad_id_nbit);
	// init the kbucket_db
	for( size_t i = 0; i < kad_id_nbit / kbucket_width; i++ ){
		bucklist_t *	bucklist = nipmem_new bucklist_t(this);
		bucklist_db.push_back(bucklist);
	}
		
// put the local kad_addr_t in the kbucket
	// compute the destination kad_addr_t for the local kad_peer_t to reach itself
	kad_listener_t *kad_listener	= kad_peer->get_listener();
	kad_addr_t	local_addr(kad_listener->listen_lview(), kad_peer->local_peerid());
	// if the ip address is ANY, replace it with localhost
	if( local_addr.oaddr().ipaddr().is_any() )	local_addr.oaddr().ipaddr("127.0.0.1");
	// put the local_addr in the kbucket
	notify_srckaddr(local_addr);
}

/** \brief Destruction
 */
kad_kbucket_t::~kad_kbucket_t()	throw()
{
	// delete all the bucknode_db
	while( !bucknode_db.empty() )	nipmem_delete	bucknode_db.begin()->second;
	// delete all the bucklist_db
	while( !bucklist_db.empty() ){
		// delete the bucklist itself
		nipmem_delete	bucklist_db.back();
		// unlink it from the database
		bucklist_db.pop_back();
	}
}

/** \brief return true if this peerid is in the kad_kbucket_t, false otherwise
 */
kad_kbucket_t::bucknode_t *kad_kbucket_t::get_bucknode(const kad_peerid_t &peerid)	throw()
{
	std::map<kad_targetid_t, bucknode_t *>::iterator	iter = bucknode_db.find(peerid);
	if( iter == bucknode_db.end() )	return NULL;	
	return iter->second;
}

/** \brief return true if the peerid matching a pending node
 */
bool	kad_kbucket_t::is_pending_node(const kad_peerid_t &peerid)		throw()
{
	std::map<kad_targetid_t, kad_addr_t *>::iterator	iter = pending_node_db.find(peerid);
	if( iter == pending_node_db.end() )	return false;	
	return true;
}

/** \brief return the bucket_idx for a given peerid
 * 
 * - see section 2.1 of the paper
 */
size_t	kad_kbucket_t::get_bucket_idx(const kad_peerid_t &peerid)	const throw()
{
	// compute the distance between the local peerid and the requested one
	kad_targetid_t	distance	= kad_peer->local_peerid() ^ peerid;
	// get the highest bit of the distance
	size_t		highest_bit	= distance.bit_get_highest_set();
	// the bucket_idx is the highest_bit divided by the number of bit per bucket
	size_t		bucket_idx	= highest_bit / kbucket_width;
	// return the result
	return bucket_idx;
}

/** \brief return the bucket_idx of a given bucklist pointer
 */
size_t	kad_kbucket_t::get_bucket_idx(const bucklist_t *bucklist)	const throw()
{
	// scan the whole bucklist_db
	for( size_t i = 0; i < bucklist_db.size(); i++ ){
		// if the pointer on this bucklist matches the asked one, return the index
		if( bucklist_db[i] == bucklist )	return i;
	}
	// NOTE: this point MUST never be reached
	DBG_ASSERT(0);
	return std::numeric_limits<size_t>::max();
}


/** \brief return a kad_addr_arr_t of the max_nb_addr oldest nodes on the kbucket
 * 
 * - the algo is : get the least recently seen nodes of each kbucket
 *   - pick a y = 0
 *   - for each bucklist, get the y-th nodes
 *   - increase y 
 *   - until either no more node are available or max_nb_addr is reached
 * - it works due to the kademlia policy: keeps 'least-recently seen node at the head'
 */
kad_addr_arr_t kad_kbucket_t::get_noldest_addr(size_t max_nb_addr)		const throw()
{
	kad_addr_arr_t	result;
	// go thru all the possible height
	for( size_t y = 0; y < kbucket_height; y++ ){
		// go thru each bucklist_t at this height
		for( size_t x = 0; x < bucklist_db.size(); x++ ){
			// if this bucklist is not long enougth, goto the next
			if( bucklist_db[x]->size() <= y )	continue;
			// - get the y-th of this bucklist_t elements
			bucknode_t *	bucknode	= bucklist_db[x]->get_node_idx(y);
			// add this node to the result
			result.append(bucknode->get_kad_addr());
			// if the result contains enougth nodes, return it now
			if( result.size() == max_nb_addr )	return result;
		}
	}
	// return the result
	return result;
}

/** \brief return an array of the max_nb_addr \ref kad_addr_t the closest to the peerid
 * 
 * @param kad_addr_filter the kad_addr_t filter to determine which kad_addr_t goes in the list or not.
 *                        It is for NAT friendlyness.
 * 
 * - a lot of 'weird' things as it must return the closest with the XOR metric
 *   and not the integer one.
 */
kad_addr_arr_t kad_kbucket_t::get_nclosest_addr(const kad_targetid_t &target_id, size_t max_nb_addr
						, kad_addr_t::filter_fct_t kad_addr_filter) const throw()
{
	int		bucket_idx	= (int)get_bucket_idx(target_id);
	int		nb_bucket	= bucklist_db.size();
	kad_addr_arr_t	tmp_addr_arr, result;

	// Get at least n nodes from the kbuckets the closest to bucket_idx
	int	inc_idx = bucket_idx;
	int	dec_idx = bucket_idx-1;
	// loop until the the tmp_addr_arr contains at least the asked number of node
	while( tmp_addr_arr.size() < max_nb_addr ){
		// if both end have been reached, exit
		if( dec_idx < 0 && inc_idx >= nb_bucket ){
			KLOG_DBG("both ends have been reached without providing enougth nodes. leaving");
			break;
		}
		// add the inc_idx bucket to the kad_addr_arr
		if( inc_idx < nb_bucket ){
			bucklist_db[inc_idx]->copy_to(tmp_addr_arr, kad_addr_filter);
			inc_idx++;
		}
		// add the dec_idx bucket to the kad_addr_arr
		if( dec_idx >= 0 ){
			bucklist_db[dec_idx]->copy_to(tmp_addr_arr, kad_addr_filter);
			dec_idx--;
		}
	}

	// now sort those elements according to XOR metric
	tmp_addr_arr.sort_by_dist(target_id);

	// then get the max_nb_addr first or less if not available
	for( size_t i = 0; i < max_nb_addr && i < tmp_addr_arr.size(); i++)
		result.append(tmp_addr_arr[i]);

	// return the result of the max_nb_addr nodes the closest to the target_id
	return result;
}


/** \brief update the \ref kad_kbucket_t with \ref kad_addr_t
 * 
 * - in paper section 2.1, "When a Kademlia node receives any message (re-
 *   quest or reply) from another node, it updates the appropriate k -bucket 
 *   for the sender’s node ID."
 * - TODO some issue here with the kad_peer_t->update_listen_pview()
 *   - this is here that the srckaddr is handled, so if the kad_peer->peerid()
 *     is already in the kad_kbucket_t but with a different kad_addr.oaddr()
 *   - the latest kad_addr_oaddr() should be updated
 *   - some issue with a DOS
 *   - aka anybody could DOS a given peerid by pushing a fake kad_addr.oaddr()
 *   - additionnaly if the kad_addr.oaddr is_null, this is MUST be removed
 * 
 */
void	kad_kbucket_t::notify_srckaddr(const kad_addr_t &kad_addr)	throw()
{
	// sanity check - kad_addr MUST be fully qualified
	// - it is up to the caller to do any required translation (likely kad_peer_t::srckaddr_parse()) 
	DBG_ASSERT( kad_addr.is_fully_qualified() );
	
	bucknode_t *	bucknode	= get_bucknode(kad_addr.peerid());
	size_t		bucket_idx	= get_bucket_idx(kad_addr.peerid());
	// log to debug
	KLOG_DBG("update the kad_addr " << kad_addr);
	
	// if the node to update is already a pending node, discard it immediatly
	if( is_pending_node(kad_addr.get_peerid()) ){
		// log to debug
		KLOG_DBG("kad_addr_t " << kad_addr << " is already a pending node in the bucket "
					<< bucket_idx << ". discard it without doing nothing.");		
		return;
	}
	
	// in paper section 2.1, "If the sending node already exists in the 
	// recipient’s k-bucket, the recipient moves it to the tail of the list."
	if( bucknode ){
		// log to debug
		KLOG_DBG("kad_addr_t " << kad_addr << " was already in the bucket " << bucket_idx 
					<< ". put it at the tail of the list.");
		// move the node to the tail of the bucklist
		bucklist_db[bucket_idx]->update(bucknode);
		return;
	}
	
	// in paper section 2.1, "If the node is not already in the appropriate 
	// k-bucket and the bucket has fewer than k entries, then the recipient 
	// just inserts the new sender at the tail of the list."
	if( bucklist_db[bucket_idx]->size() < kbucket_height ){
		// log to debug
		KLOG_DBG("kad_addr_t " << kad_addr << " was NOT in the bucket " << bucket_idx 
					<< ". put it at the tail of the list.");
		// create the bucknode_t
		bucknode = nipmem_new bucknode_t(kad_addr, this);
		// put it at the tail of the bucklist
		bucklist_db[bucket_idx]->insert(bucknode);
		return;		
	}
	
	// - in paper section 2.1, "If the appropriate k-bucket is full, however, 
	//   then the recipient pings the k-bucket’s least-recently seen"
	
	// find the least-recently node not yet pinging
	bucknode = bucklist_db[bucket_idx]->get_first_notpinging();
	// if all nodes are pinging, this update is discarded
	if( !bucknode ){
		// log to debug
		KLOG_DBG("kad_addr " << kad_addr << " was NOT in the bucket "
				<< bucket_idx << " which is full."
				<< " But discard it as all nodes are currently pinged.");	
		return;
	}
	// log to debug
	KLOG_DBG("kad_addr " << kad_addr << " was NOT in the bucket " << bucket_idx << " which is full"
				<< " So start pinging existing node " << bucknode->get_kad_addr() );
	// start to ping it
	bucknode->ping_start(kad_addr);
}


/** \brief Perform the kbucket refreshing after the bootstrapping
 * 
 * - from the paper, "Finally, u refreshes all k-buckets further away than its closest
 *   neighbor."
 */
void	kad_kbucket_t::refresh_post_bstrap()			throw()
{
	// go thru the bucklist_t from the farthest to the closest
	for( int i = bucklist_db.size() - 1; i >= 0; i-- ){
		// if this bucklist is not empty, all the "k-buckets further 
		// away than its closest neighbor" has been went thru
		if( bucklist_db[i]->size() )	break;
		// start refreshing this bucklist_t
		bucklist_db[i]->trigger_refresh_now();
	}
}

/** \brief Perform the kbucket refreshing for ALL bucklist_t
 * 
 * - this is used in case of change in the kad_peer->update_listen_pview
 *   to update the remote peer with the new address
 */
void	kad_kbucket_t::refresh_all_bucklist()			throw()
{
	// go thru the bucklist_t from the farthest to the closest
	for(size_t i = 0; i < bucklist_db.size(); i++ ){
		// start refreshing this bucklist_t
		bucklist_db[i]->trigger_refresh_now();
	}
}

/** \brief Called by kad_nsearch_t to notify a nsearch for this target_id
 * 
 * - from the paper "To handle pathological cases in which there are no 
 *   lookups for a particular ID range, each no de refreshes any bucket
 *   to which it has not performed a node lookup in the past hour."
 */
void	kad_kbucket_t::notify_nsearch(const kad_targetid_t &targetid)	throw()
{
	size_t	bucket_idx	= get_bucket_idx(targetid);
	// forward the nsearch to the proper kbucket
	bucklist_db[bucket_idx]->notify_nsearch();
}

/** \brief Called when a RPC failed toward a given peerid
 * 
 * @param peerid	the peerid of the destination node
 * @param kad_event	the event which has reported the faillure
 * @param expire_delay	the expiration delay of the failed RPC (used in case of kad_event.is_timedout())
 * 
 */
void	kad_kbucket_t::notify_failed_rpc(const kad_peerid_t &peerid, const kad_event_t &kad_event
						, const delay_t &expire_delay)	throw()
{
	const kad_kbucket_profile_t &	profile	= kad_peer->get_profile().kbucket();
	// sanity check - the kad_event_t MUST be fatal
	DBG_ASSERT( kad_event.is_fatal() );
	// log to debug
	KLOG_ERR("kbucket being notified that peerid = " << peerid 
			<< " failed with a kad_event=" << kad_event << " after " << expire_delay);

	// if the kad_event is a timedout but the expire_delay is too short, do nothing
	if(kad_event.is_timedout() && expire_delay < profile.delete_timedout_rpc_min_delay())	return;

	// NOTE: This mechanism deletes bucknodes by their peerid asynchronously via zerotimer.
	//       It is used by the notify_failed_rpc() to avoid any race or weird nested issue.
		
	// put the peerid to delete in the database
	peerid_todelete_db.push_back( peerid );
	// launch the zerotimer if not already done
	if( peerid_todelete_zerotimer.empty() )
		peerid_todelete_zerotimer.append(this, NULL);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	kad_kbucket_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw()
{
	kad_kbucket_t::bucknode_t *	bucknode;
	// loop until the peerid_todelete_db is empty
	while( !peerid_todelete_db.empty() ){
		// get the first peerid of the list
		kad_peerid_t	peerid	= peerid_todelete_db.front();
		// remove it from the list
		peerid_todelete_db.pop_front();
		// find the bucket_idx for this peerid
		size_t	bucket_idx	= get_bucket_idx(peerid);
		// get the bucknode matching this peerid
		bucknode		= bucklist_db[bucket_idx]->get_bucknode(peerid);
		// if the bucknode is nomore there, skip it
		if( !bucknode )		continue;
		// log to debug
		KLOG_ERR("remove bucknode with peerid=" << bucknode->get_kad_addr().get_peerid());
		// remove this bucknode from the bucklist
		bucklist_db[bucket_idx]->remove(bucknode);
		// delete the object itself
		nipmem_delete bucknode;
	}
	// return 'tokeep'
	return true;
}
NEOIP_NAMESPACE_END;






