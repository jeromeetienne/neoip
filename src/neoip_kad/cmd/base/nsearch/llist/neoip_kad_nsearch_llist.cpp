/*! \file
    \brief Definition of the \ref kad_nsearch_llist_t

*/

/* system include */
/* local include */
#include "neoip_kad_nsearch_llist.hpp"
#include "neoip_kad_nsearch_lnode.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_caddr.hpp"
#include "neoip_kad_caddr_arr.hpp"
#include "neoip_kad_recdups.hpp"
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
kad_nsearch_llist_t::kad_nsearch_llist_t(kad_nsearch_t *kad_nsearch)	throw()
{
	// copy the parameter
	this->kad_nsearch	= kad_nsearch;
	// zero some fields
	nb_succ_useless_query	= 0;
	findsome_max_nb_rec	= 0;
}

/** \brief Destructor by default
 */
kad_nsearch_llist_t::~kad_nsearch_llist_t()	throw()
{
	// delete all lnode_t
	while( !lnode_db.empty() )	nipmem_delete lnode_db.begin()->second;	
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       lnode_db management
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief link a lnode_t in the lnode_db indexed by its distance between its
 *         peerid and the target id
 * 
 * - it is independant of its state
 */
void	kad_nsearch_llist_t::lnode_link(kad_nsearch_lnode_t *lnode)		throw()
{
	const kad_peerid_t &peerid = lnode->get_kad_addr().get_peerid();
	// sanity check - the node MUST NOT be already present in the database
	DBG_ASSERT( !lnode_present(peerid) );

	// compute the distance between the lnode_t peerid and the target_id
	kad_targetid_t	distid	= peerid ^ kad_nsearch->target_id;
	// insert it in the database
	bool		succeed	= lnode_db.insert(std::make_pair(distid, lnode)).second;
	DBG_ASSERT( succeed );
}

/** \brief unlink a lnode_t from the lnode_db indexed by its distance between its
 *         peerid and the target id
 */
void	kad_nsearch_llist_t::lnode_unlink(kad_nsearch_lnode_t *lnode)		throw()
{
	const kad_peerid_t &peerid = lnode->get_kad_addr().get_peerid();
	// sanity check - the node MUST be present in the database
	DBG_ASSERT( lnode_present(peerid) );
	// compute the distance between the lnode_t peerid and the target_id
	kad_targetid_t	distid	= peerid ^ kad_nsearch->target_id;
	// remove this lnode_t from the database
	lnode_db.erase(distid);
}

/** \brief return true if a given nodes exists in the learnednode database
 * 
 * - it is independant of its state
 */
bool	kad_nsearch_llist_t::lnode_present(const kad_peerid_t &peerid)	const throw()
{
	// compute the distance between the lnode_t peerid and the target_id
	kad_targetid_t	distid	= peerid ^ kad_nsearch->target_id;
	// if no matching element is found, return false
	if( lnode_db.find(distid) == lnode_db.end() )	return false;
	// else return true
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                      merge a kad_addr_arr_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief update the learnednode list with a kad_addr_arr_t
 * 
 * @return false if the update did not insert "a node any closer than the closest already seen".
 *         true if the update insert a node closer than the closest already seen.
 *         NOTE: The algorithm is: if the nodes to be considered after the update aren't 
 *               the same than before the update, then return true.
 */
bool	kad_nsearch_llist_t::merge_addr_arr(const kad_addr_arr_t &kad_addr_arr)	throw()
{
	// backup the list of distid to be considered
	std::set<kad_targetid_t>	nclosest_distid_before = get_nclosest_distid();

	// log to debug
	KLOG_DBG("update with " << kad_addr_arr);

	// go thru the kad_addr_arr and update the
	for( size_t i = 0; i < kad_addr_arr.size(); i++ ){
		const kad_addr_t &	kad_addr = kad_addr_arr[i];
		// if this node is already in the learnednode, skip it
		if( lnode_present(kad_addr.get_peerid()) )	continue;
		// if this node is not in the learned node, insert it
		nipmem_new kad_nsearch_lnode_t(this, kad_addr); 
	}
	
	// return false if the update did not insert "a node any closer than the closest already seen".
	if( nclosest_distid_before == get_nclosest_distid() )	return false;
	// else return true
	return true;
}

/** \brief return a set of the kad_targetid_t of the distance between the target id and 
 *         the node_id of the nodes to be considered.
 * 
 * - This function is part of the process to determine "If a round of find nodes fails to
 *   return a node any closer than the closest already seen"
 */
std::set<kad_targetid_t>	kad_nsearch_llist_t::get_nclosest_distid()	const throw()
{
	lnode_db_t::const_iterator	iter;
	std::set<kad_targetid_t>	distid_db;	
	size_t				nb_considered = 0;
	// put the map's key of the nodes not considered offline in a set, with at most n_closest of them
	for( iter = lnode_db.begin(); iter != lnode_db.end(); iter++){
		const kad_keyid_t &		key	= iter->first;
		const kad_nsearch_lnode_t *	node	= iter->second;
		// If this node is considered offline, skip this node
		if( node->is_offline() )				continue;
		// update the nb_considered
		nb_considered++;
		// if no node matches after the n_closest have been considered, exit
		if( nb_considered > kad_nsearch->n_closest )			break;
		// put this node in the set
		bool succeed = distid_db.insert(key).second;
		DBG_ASSERT( succeed );
	}
	// return the result
	return distid_db;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Launch max_nb_query query or less if less than max_nb_query are available
 */
void	kad_nsearch_llist_t::launch_queries(size_t max_nb_query) throw()
{
	// loop until max_nb_query have been launched, or no more node are to be queried
	for( size_t i = 0; i < max_nb_query; i++ ){
		kad_nsearch_lnode_t *	node;
		do {
			// get a node to query
			node	= get_next_lnode_to_query();
			// if no node is available, exit
			if( !node )	break;
			// loop until the query_start() succeed
		} while( node->start() != kad_err_t::OK );
	}
}

/** \brief return a pointer on the lnode_t which is estimated to be the best to query
 *         only amoung n_closest. or NULL if no node is available to query
 * 
 * - Currently the algo is : return the closest node which still is in lnode_t::INIT
 *   - it may be improved later with latency information 
 *     - e.g. with vivaldi coordinates contained in kad_addr_t
 */
kad_nsearch_lnode_t *	kad_nsearch_llist_t::get_next_lnode_to_query()	const throw()
{
	lnode_db_t::const_iterator	iter;
	size_t				nb_considered	= 0;
	// got thru the learnednode_db from the closest to the farthest
	for( iter = lnode_db.begin(); iter != lnode_db.end(); iter++){
		kad_nsearch_lnode_t *	node	= iter->second;
		// If this node is considered offline, skip this node
		if( node->is_offline() )				continue;
		// update the nb_considered
		nb_considered++;
		// if no node matches after the n_closest have been considered, exit
		if( nb_considered > kad_nsearch->n_closest )		break;
		// if the NODE current_state is not is_null(), skip it
		if( node->current_state().is_null() == false  )		continue;
		// return this node
		return node;
	}
	return NULL;
}

/** \brief Return the number of lnode_t currently QUERYING among the n_closest
 */
size_t	kad_nsearch_llist_t::get_nb_querying_node()				const throw()
{
	lnode_db_t::const_iterator	iter;
	size_t				nb_considered	= 0;	
	size_t				nb_querying	= 0;
	// got thru the learnednode_db from the closest to the farthest
	for( iter = lnode_db.begin(); iter != lnode_db.end(); iter++){
		kad_nsearch_lnode_t *	node	= iter->second;
		// If this node is considered offline, skip this node
		if( node->is_offline() )			continue;
		// Increase the nb_considered
		nb_considered++;
		// if no node matches after the n_closest have been considered, exit
		if( nb_considered > kad_nsearch->n_closest )	break;		
		// if the NODE is in QUERYING, count it
		if( node->current_state() == kad_nsearch_lnode_state_t::QUERYING )	nb_querying++;
	}
	return nb_querying;
}



/** \brief return true if the kad_nsearch_t is completed, false otherwise
 * 
 * - from the paper, "The lookup terminates when the initiator has queried and
 *   gotten responses from the k closest nodes it has seen."
 */
bool	kad_nsearch_llist_t::nsearch_is_completed()			const throw()
{
	lnode_db_t::const_iterator	iter;
	size_t				nb_considered	= 0;	
	// got thru the learnednode_db from the closest to the farthest
	for( iter = lnode_db.begin(); iter != lnode_db.end(); iter++){
		kad_nsearch_lnode_t *	node	= iter->second;
		// If this node is considered offline, skip this node
		if( node->is_offline() )			continue;
		// Increase the nb_considered
		nb_considered++;
		// if n_closest node have been considered, and are all in succeed, considered it completed
		if( nb_considered > kad_nsearch->n_closest )	return true;
		// if this node is NOT in SUCCEED, return false
		if( node->current_state() != kad_nsearch_lnode_state_t::SUCCEED )	return false;
	}
	// if all non-offline nodes are all in SUCCEED, considere it completed
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 result notification
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Report the end result
 */
bool	kad_nsearch_llist_t::notify_end_result()				throw()
{
	// sanity check - here the nsearch MUST be completed
	DBG_ASSERT( nsearch_is_completed() );

	// notify the result according to the query_type
	const kad_pkttype_t &	query_type	= kad_nsearch->query_type;
	if( query_type.is_FINDNODE_REQUEST() )		return notify_findnode_result();
	else if( query_type.is_FINDSOMEVAL_REQUEST() )	return notify_findsomeval_result();

	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief Report the nsearch result for FINDNODE rpc
 * 
 * - it reports a kad_addr_arr_t containing the, at most nclosest, nodes to the target_id
 */
bool	kad_nsearch_llist_t::notify_findnode_result()			throw()
{
	lnode_db_t::const_iterator	iter;
	kad_caddr_arr_t			result;
	// sanity check - here the nsearch MUST be completed
	DBG_ASSERT( nsearch_is_completed() );
	// convert the nclosest considerable nodes from learnednode_db into a kad_addr_arr
	// - go thru the learnednode_db from the closest to the farthest
	for( iter = lnode_db.begin(); iter != lnode_db.end(); iter++){
		kad_nsearch_lnode_t *	node	= iter->second;
		// If this node is considered offline, skip this node
		if( node->is_offline() )		continue;
		// sanity check - here the node MUST be considered online 
		// - as the findnode nsearch is supposed to be completed
		DBG_ASSERT( node->is_online() );
		// put this node into the result
		result.append( kad_caddr_t(node->get_kad_addr(), node->get_cookie_id()) );
		// if learnednodenb_completed reached the n_closest, the kad_nsearch IS completed
		if( result.size() == kad_nsearch->n_closest )	break;
	}
	// notify the caller
	return kad_nsearch->notify_callback( kad_event_t::build_caddr_arr(result) );
}

/** \brief Report the nsearch result for FINDSOMEVAL rpc
 * 
 * - it reports a kad_rec_t containing a single record matching the keyid
 */
bool	kad_nsearch_llist_t::notify_findsomeval_result()			throw()
{
	kad_recdups_t	kad_recdups;
 	// this function report only a null kad_rec_t as if one record is
	// found it is reported immediatly during the reply processing
	// - due to that, has_more_record is always false
	return kad_nsearch->notify_callback( kad_event_t::build_recdups(kad_recdups, false) );
}

NEOIP_NAMESPACE_END;






