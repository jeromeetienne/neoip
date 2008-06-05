/*! \file
    \brief Declaration of the nslan_listener_t

\par Brief Description
the \ref nslan_listener_t performs a record publication/query service on top on a LAN 
via UDP multicast.
The application can publish to one or more realm and to have multiple records
per realm. The records may be queried and all the found records, including
the locally published ones, are notified via callback.

\par Querying process
The query request are sent over the lan via UDP multicast and using an exponantial
backoff timer.
- Additionnaly, it implements a feature called 'get records from query'. Here
  is an example of its usefullness.
  -# A node X is sitting on the LAN without any other records online
  -# With time, the request are sent less and less frequently to avoid unnecessary
     traffic on the LAN. (up to 3min between 2 requests)
  -# Suddently, a node Y publish to this realm and start sending query for other
     records.
  -# When the node X receives the query of node Y, node X (i) replies its own
     records to the node Y and (ii) notify its local query of the records
     of the node Y. (it is possible as the query contains the recordlist if any)
  -  Thus even if a node has been sitting along for a while, it will be warned
     immediatly of any new records if they queries the realm too.

\par Possible improvement
- this nslan may be changed into a lanns with a more generic stuff

*/

/* system include */
/* local include */
#include "neoip_nslan_listener.hpp"
#include "neoip_nslan_peer.hpp"
#include "neoip_nslan_pkttype.hpp"
#include "neoip_udp.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nslan_listener_t::nslan_listener_t()		throw()
{
	// zero some fields
	nudp = NULL;
}

/** \brief Desstructor
 */
nslan_listener_t::~nslan_listener_t()		throw()
{
	// sanity check - all the nslan_peer_t MUST have been destructed
	DBG_ASSERT( peer_db.empty() );
	
	// destroy the responder if needed
	if( nudp )	nipmem_delete nudp;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief start the actions
 */
inet_err_t	nslan_listener_t::start(const ipport_addr_t &listen_addr)	throw()
{
	inet_err_t	inet_err;
	// copy the listen_addr
	this->listen_addr	= listen_addr;
	// init the nudp
	nudp = nipmem_new nudp_t();
	// bind the nudp to ANY:port of listen_addr
	ipport_addr_t	addr = ipport_addr_t("0.0.0.0", listen_addr.get_port());
	inet_err = nudp->start(addr, this, NULL );
	if( inet_err.failed() )	goto error;
	// log to debug
	KLOG_DBG("opened nudp " << *nudp);
	// if the listen_addr is multicast, make nudp subscribe to this address
	if( listen_addr.get_ipaddr().is_multicast() ){
		inet_err = nudp->subscribe(listen_addr.get_ipaddr());
		if( inet_err.failed() )	goto error;
	}
	// return no error
	return inet_err_t::OK;

error:;	// log to debug
	KLOG_ERR("Cant start nudp due to " << inet_err );
	// free the nudp and mark it unused
	nipmem_delete	nudp;
	nudp		= NULL;
	// return the error
	return inet_err;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           handle the peer_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief link a new nslan_peer_t into the nslan_listener_t
 */
void nslan_listener_t::peer_link(nslan_peer_t *nslan_peer)	throw()
{
	// insert this realm in the peer_db
	peer_db.insert(std::make_pair(nslan_peer->get_realmid(), nslan_peer));
}

/** \brief unlink a new nslan_peer_t into the nslan_listener_t
 */
void nslan_listener_t::peer_unlink(nslan_peer_t *nslan_peer)	throw()
{
	std::multimap<nslan_realmid_t,nslan_peer_t *>::iterator	iter;
	nslan_realmid_t	realmid	= nslan_peer->get_realmid();
	// get the first element >= than the realmid, or .end() if none exist
	iter	= peer_db.lower_bound( realmid );
	// sanity check - the realm MUST be in the peer_db
	DBG_ASSERT( iter != peer_db.end() );
	// scan all the elements with the proper key
	for( ; iter != peer_db.end() && iter->first == realmid; iter++ ){
		// if this element is NOT the realm to unlink, skip it
		if( iter->second != nslan_peer )	continue;
		// delete the current element 
		peer_db.erase(iter);
		// exit the loop
		break;
	}
	// sanity check - the realm MUST have been found
	DBG_ASSERT( iter != peer_db.end() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            send packet
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Send a packet toward a destination address
 */
inet_err_t nslan_listener_t::send_pkt(const ipport_addr_t &dest_addr, const pkt_t &pkt)const throw()
{
	// log to debug
	KLOG_DBG("Send to " << dest_addr << " the packet " << pkt);
	// send the packet
	return nudp->send_to(pkt, dest_addr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     nudp callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref nudp_t receive a packet
 */
bool	nslan_listener_t::neoip_inet_nudp_event_cb(void *cb_userptr, nudp_t &cb_nudp, pkt_t &pkt
					, const ipport_addr_t &local_addr
					, const ipport_addr_t &remote_addr)	throw()
{
	nslan_realmid_t	realmid;
	// log to debug
	KLOG_DBG("Received from remote_addr=" << remote_addr << " the pkt = " << pkt );

	// parse the incoming packet	
	try {	// read the realmid
		pkt >> realmid;
	} catch(serial_except_t &e) {
		KLOG_ERR("Received bogus request");
		return true;
	}

	// get the first element >= than the realmid, or .end() if none exist
	std::multimap<nslan_realmid_t,nslan_peer_t *>::iterator	iter = peer_db.lower_bound( realmid );
	// if no nslan_peer matchs this realmid, discard the packet
	if( iter == peer_db.end() )	return true;
	
	// scan all the realm with the proper realmid, and pass the packet
	for( ; iter != peer_db.end() && iter->first == realmid; iter++ ){
		nslan_peer_t *	nslan_peer	= iter->second;
		// pass the packet to this nslan_peer
		nslan_peer->handle_incoming_pkt(remote_addr, pkt);
	}

	// return tokeep	
	return true;
}
NEOIP_NAMESPACE_END


