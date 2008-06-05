/*! \file
    \brief Definition of the \ref kad_peer_t
    
*/

/* system include */
#include <iomanip>
#include <fstream>
/* local include */
#include "neoip_kad_peer.hpp"
#include "neoip_kad_peer_rpc_server.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_bstrap.hpp"
#include "neoip_kad_bstrap_file.hpp"
#include "neoip_file_path.hpp"
#include "neoip_ipport_aview_helper.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_nslan.hpp"
#include "neoip_rand.hpp"
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
kad_peer_t::kad_peer_t()							throw()
{;
	// log to debug
	KLOG_DBG("enter");	
	// zero some field
	kad_listener	= NULL;
	kad_bstrap	= NULL;
	nslan_publish	= NULL;
	remote_db	= NULL;
	local_db	= NULL;
	kbucket		= NULL;
	rpc_server	= NULL;
}

/** \brief Destructor
 */
kad_peer_t::~kad_peer_t()			throw()
{
	// log to debug
	KLOG_DBG("enter");	

	// delete the rpc_server_t if needed
	nipmem_zdelete rpc_server;
	// deinitialize the kad_bstrap_src
	kad_bstrap_deinit();
	// destroy the kbucket if needed
	nipmem_zdelete kbucket;
	// destroy the remote_db if needed
	nipmem_zdelete remote_db;
	// destroy the local_db if needed
	nipmem_zdelete local_db;
	// unlink this kad_peer_t from the kad_listener
	if( kad_listener )	kad_listener->peer_unlink(this);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                start() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
kad_peer_t &	kad_peer_t::set_profile(const kad_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == kad_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}
/** \brief start the operations
 */
kad_err_t	kad_peer_t::start(kad_listener_t *kad_listener, const kad_realmid_t &realm_id
						, const kad_peerid_t &m_local_peerid)	throw()
{
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter");	
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == kad_err_t::OK );	

// TODO what if the local_peerid already exists ?
// - it should return an error

	// sanity check - the local_peerid MUST NOT be null'
	DBG_ASSERT( !m_local_peerid.is_null() );
	// copy the parameter
	this->kad_listener	= kad_listener;
	this->realm_id		= realm_id;
	this->m_local_peerid	= m_local_peerid;
	// zero some field
	this->kad_bstrap	= NULL;
	this->nslan_publish	= NULL;
	
	// link this realm to the kad_listener
	kad_listener->peer_link(this);
	// init the remote database - take its profile directly from the kad_peer_t pointer
	remote_db	= nipmem_new kad_db_t(this);
	// init the local database - take its profile directly from the kad_peer_t pointer
	// - set the replication period to NEVER, as the record are republished according to record ttl
	local_db	= nipmem_new kad_db_t(this);
	// init the kbucket - take its profile directly from the kad_peer_t pointer
	kbucket		= nipmem_new kad_kbucket_t(this);
	
	// initialize the kad_bstrap_t
	kad_err		= kad_bstrap_init();
	if( kad_err.failed() )	return kad_err;

	// create the rpc_server_t
	rpc_server	= nipmem_new rpc_server_t(this);

	// return no error
	return kad_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                bootstrapping function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Initialize the bootstrapping
 */
kad_err_t	kad_peer_t::kad_bstrap_init()	throw()
{
	nslan_peer_t *	nslan_peer	= kad_listener->nslan_peer();
	kad_err_t	kad_err;
	// create a nslan_rec_t for the peer record on the nslan_peer_t
	nslan_keyid_t	nslan_keyid	= realm_id.to_canonical_string();
	bytearray_t	payload;
	payload 	<< kad_addr_t(kad_listener->listen_lview(), local_peerid());
	nslan_rec_t	nslan_rec(nslan_keyid, payload.to_datum()
						, profile.bstrap().nslan_peer_record_ttl());
		
	// publish the peer record
	nslan_publish	= nipmem_new nslan_publish_t(nslan_peer, nslan_rec);

	// create the kad_bstrap_t
	kad_bstrap	= nipmem_new kad_bstrap_t();
	kad_err		= kad_bstrap->set_profile(profile.bstrap()).start(this, this, NULL);
	if( kad_err.failed() )	return kad_err;


	// return no error
	return kad_err_t::OK;
}

/** \brief DeInitialize the bootstrapping
 */
void	kad_peer_t::kad_bstrap_deinit()	throw()
{
	// save the dynamic file for kad_bstrap_t
	kad_err_t	kad_err;
	kad_err	= kad_bstrap_file_t::dynfile_save(this);
	if( kad_err.failed() )	KLOG_ERR("Can't save kad_bstrap_t dynfile due to " << kad_err);
	// delete the nslan_publish if needed
	nipmem_zdelete nslan_publish;
	// delete the kad_bstrap_t if needed
	nipmem_zdelete kad_bstrap;
}


/** \brief callback notified when a kad_bstrap_t is completed
 */
bool 	kad_peer_t::neoip_kad_bstrap_cb(void *cb_userptr, kad_bstrap_t &cb_kad_bstrap) throw()
{
	// log to debug
	KLOG_ERR("Bootstrapping completed");

	// delete the kad_bstrap and mark it unused
	nipmem_zdelete	kad_bstrap;
	// dont return 'tokeep'
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             Misc function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle request pkt for this kad_peer_t
 * 
 * - simply forward it to rpc_server_t
 */
pkt_t	kad_peer_t::handle_request(pkt_t &pkt, const ipport_addr_t &local_oaddr
					, const ipport_addr_t &remote_oaddr)	throw()
{
	return rpc_server->handle_request_pkt(pkt, local_oaddr, remote_oaddr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			srckaddr build/parse
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compute the 'source kad_addr_t' field for packeta sent from this kad_peer_t
 * 
 * - the srckaddr is included in each sent packet i.e. request and reply
 * - this is used by the receiver of the packet to determine the sender address
 *   - its peerid and its listen oaddr
 * - this is highly related to NAT friendlyness
 * - if the sender is inetreach_ok (aka it it possible to initiate a connection
 *   to it from anywhere on the internet), the srckaddr is NOT null
 *   - else it is null
 * - if the srckaddr is null, the receiver will not include the received peerid in its kbuckets
 *   - from paper section 2.1 "When a Kademlia node receives any message (request 
 *     or reply) from another node, it updates the appropriate k-bucket for the 
 *     sender’s node ID."
 *   - thus the kbucket of a kad_peer_t contains only other peers which are 
 *     reachable for it.
 */
kad_addr_t	kad_peer_t::srckaddr_build(const ipport_addr_t &remote_oaddr)	const throw()
{
	// get the xmit_listen_ipport for this remote_oaddr
	ipport_addr_t	listen_xmit;
	listen_xmit	= ipport_aview_helper_t::listen_aview2xmit(kad_listener->listen_aview(), remote_oaddr);
	// if the local peer is unreachable by this remote peer, return a null kad_addr_t
	if( listen_xmit.is_null() )	return kad_addr_t();
	// if the local peer is reachable by this remote_peer, return a kad_addr_t with xmit_listen_ipport
	return kad_addr_t(listen_xmit, local_peerid());
}

/** \brief Parse a received source kad_addr_t
 * 
 * - if remote_kaddr.is_null() the sender is not inetreach_ok, so do nothing
 *   - aka dont put it in the kbucket_t
 * - if remote_kaddr is not null, build an updated kad_addr_t with the remote_oaddr.ipaddr()
 *   - in short the remote_kaddr.oaddr().ipaddr() is ALWAYS ignored
 * - then it notify the kbucket_t with the updated kad_addr_t
 */
void	kad_peer_t::srckaddr_parse(const kad_addr_t &remote_kaddr
				, const ipport_addr_t &remote_oaddr) const throw()
{
	// if the remote_kaddr is null, do nothing
	if( remote_kaddr.is_null() )	return;

	// try convert the xmit_listen_ipport in the remote_kaddr, 
	ipport_addr_t	dest_oaddr;
	dest_oaddr	= ipport_aview_helper_t::listen_xmit2dest(remote_kaddr.oaddr(), remote_oaddr);
	// sanity check - dest_oaddr MUST NOT be null 
	// - as if the remote peer is unreachable, the whole remote_kaddr MUST be NULL
	DBGNET_ASSERT( !dest_oaddr.is_null() );
	// build the updated_kaddr - remote_kaddr with remote_oaddr.ipaddr(); 
	kad_addr_t	updated_kaddr(dest_oaddr, remote_kaddr.peerid());
	// sanity check - at this point, the new_srckaddr MUST be fully_qualified
	DBG_ASSERT( updated_kaddr.is_fully_qualified() );
	// - in paper section 2.1, "When a Kademlia node receives any message (re-
	//   quest or reply) from another node, it updates the appropriate k-bucket 
	//   for the sender’s node ID."
	kbucket->notify_srckaddr(updated_kaddr);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Function used to update the listen_pview *DURING* kad_listener_t run
 * 
 * - this function updates the linked kad_peer_t too
 */
void	kad_peer_t::update_listen_pview(const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter NOT YET IMPLEMENTED new_listen_pview=" << new_listen_pview);
	// TODO optimisation
	// - kad only need a port pview, so dont take action if the port pview dont change
	// - how to detect this case
	//   - how to do that ?
	//   - to call this function IIF new_listen_pview change its port ?
	//   - or simple add the old_listen_pview as parameter
#if 0
	// advertize the new_listen_pview to remote kad_peer_t's
	// - this is done by refreshing all the the kad_kbucket_t::bucklist_t
	get_kbucket()->refresh_all_bucklist();
	// TODO issue in the parsing of this new_listen_pview 
	// - see kad_kbucket_t::notify_srckaddr()
#endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::string kad_peer_t::to_string()			const throw()
{
	std::ostringstream	oss;
	oss << "[";
	oss << "realm_id=" << realm_id;
	oss << " ";
	oss << "peerid=" << local_peerid();
	oss << "]";
	return oss.str();
}


NEOIP_NAMESPACE_END;






