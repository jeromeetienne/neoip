/*! \file
    \brief Definition of the \ref kad_kbucket_t nested class bucklist_t
    
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_kbucket.hpp"
#include "neoip_kad_kbucknode.hpp"
#include "neoip_kad_kbucklist.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_kad_ping_rpc.hpp"
#include "neoip_kad_event.hpp"
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
kad_kbucket_t::bucknode_t::bucknode_t(const kad_addr_t &kad_addr, kad_kbucket_t *kad_kbucket)	throw()
{
	// copy some parameters
	this->kad_addr		= kad_addr;
	this->kad_kbucket	= kad_kbucket;
	// zero some parameters
	pending_node		= NULL;
	ping_rpc		= NULL;
	// notify the remote_db that a new node appears in the kbucket
	kad_db_t * remote_db	= kad_kbucket->kad_peer->get_remote_db();
	remote_db->notify_new_node(kad_addr);
	// link it to kad_kbucket
	// TODO to replace this by a usual bucknode_link()
	bool	succeed = kad_kbucket->bucknode_db.insert(std::make_pair(kad_addr.get_peerid(), this)).second;
	DBG_ASSERT( succeed );
}

/** \brief Destructor
 */
kad_kbucket_t::bucknode_t::~bucknode_t()		throw()
{
	// if a ping is in progress, stop it
	if( ping_in_progress() )	ping_stop();
	// unlink it to kad_kbucket
	// TODO to replace this by a usual bucknode_link()
	kad_kbucket->bucknode_db.erase(kad_addr.get_peerid());
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     ping for bucknode
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start pinging this node
 * 
 * - in paper section 2.1, "If the appropriate k-bucket is full, however, 
 *   then the recipient pings the k-bucket’s least-recently seen"
 * - this function starts pinging to handle the above case
 */
kad_err_t kad_kbucket_t::bucknode_t::ping_start(const kad_addr_t &new_node)	throw()
{
	const kad_kbucket_profile_t &	profile = kad_kbucket->kad_peer->get_profile().kbucket();
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("start a ping on kad_addr=" << kad_addr << " with a timeout of " << profile.ping_rpc_timeout());
	// sanity check - here it MUST NOT be already pinging
	DBG_ASSERT( !ping_in_progress() );
	// start the ping rpc
	ping_rpc	= nipmem_new kad_ping_rpc_t();
	kad_err		= ping_rpc->start(kad_kbucket->kad_peer, kad_addr, profile.ping_rpc_timeout()
										, this, NULL);
	// if the start failed,
	if( kad_err.failed() ){
		nipmem_zdelete ping_rpc;
		return kad_err;
	}
	// copy the pending_node
	pending_node	= nipmem_new kad_addr_t(new_node);
	// link the pending_node to kad_kbucket
	bool	succeed = kad_kbucket->pending_node_db.insert(std::make_pair(pending_node->get_peerid(), pending_node)).second;
	DBG_ASSERT( succeed );
	// return no error	
	return kad_err_t::OK;
}

void kad_kbucket_t::bucknode_t::ping_stop()					throw()
{
	// sanity check - here it MUST be pinging
	DBG_ASSERT( ping_in_progress() );
	// destroy the kad_ping_rpc_t and mark it unused
	nipmem_zdelete	ping_rpc;
	// unlink the pending_node to kad_kbucket
	kad_kbucket->pending_node_db.erase(pending_node->get_peerid());
	// destroy the pending_node and mark it unused
	nipmem_zdelete pending_node;	
}

/** \brief callback notified when a kad_ping_rpc_t has an event to notify
 */
bool kad_kbucket_t::bucknode_t::neoip_kad_ping_rpc_cb(void *cb_userptr, kad_ping_rpc_t &cb_ping_rpc
					, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be ping_rpc_ok()
	DBG_ASSERT( kad_event.is_ping_rpc_ok() );
	
	// if the event is fatal, the current bucknode MUST be replaced by the pending node
	// - in paper section 2.1, "If the least-recently seen node fails to respond, it is
	//   evicted from the k-bucket and the new sender inserted at the tail."	
	if( kad_event.is_fatal() ){
		// log to debug
		KLOG_DBG("Pinging existing " << kad_addr << " failed due to " << kad_event 
					<< " So it is replaced by " << *pending_node);
		// create a new bucknode from the pending_node
		bucknode_t *	bucknode	= nipmem_new bucknode_t(*pending_node, kad_kbucket);
		// remove the old bucknode from the bucklist_db
		size_t		bucket_idx	= kad_kbucket->get_bucket_idx(kad_addr.get_peerid());
		kad_kbucket->bucklist_db[bucket_idx]->remove(this);
		// add the new bucknode in the bucklist
		kad_kbucket->bucklist_db[bucket_idx]->insert(bucknode);
		// delete the old bucknode itself
		nipmem_delete this;
		// return NOT tokeep is replaced by
		return false;
	}
	
	// sanity check - here the kad_event MUST be is_completed
	DBG_ASSERT( kad_event.is_completed() );

	// log to debug
	KLOG_DBG("Pinging existing " << kad_addr << " succeed." << " So discard the pending node "
								<< *pending_node);
	// - in paper section 2.1, "if the least-recently seen node responds, it is moved
	//   to the tail of the list, and the new sender’s contact is discarded."
	size_t	bucket_idx	= kad_kbucket->get_bucket_idx(kad_addr.get_peerid());
	kad_kbucket->bucklist_db[bucket_idx]->update(this);
	// discard the pending node and stop the pinging
	ping_stop();
	
	return false;
}


NEOIP_NAMESPACE_END;






