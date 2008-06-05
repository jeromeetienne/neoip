/*! \file
    \brief Declaration of the nslan_peer_t
    
*/

/* system include */
/* local include */
#include "neoip_nslan_peer.hpp"
#include "neoip_nslan_listener.hpp"
#include "neoip_nslan_publish.hpp"
#include "neoip_nslan_query.hpp"
#include "neoip_nslan_pkttype.hpp"
#include "neoip_nslan_id.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_pkt.hpp"
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
nslan_peer_t::nslan_peer_t(nslan_listener_t *nslan_listener, const nslan_realmid_t &realmid)	throw()
{
	// copy the field
	this->nslan_listener	= nslan_listener;
	this->realmid		= realmid;
	// link this object to the listener
	nslan_listener->peer_link(this);
}

/** \brief Desstructor
 */
nslan_peer_t::~nslan_peer_t()		throw()
{
	// sanity check - all the nslan_publish_t MUST have been destructed
	DBG_ASSERT( publish_db.empty() );
		
	// unlink this object from the listener
	nslan_listener->peer_unlink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  received packet queue management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handler incoming packet for this nslan_peer_t
 */
void	nslan_peer_t::handle_incoming_pkt(const ipport_addr_t &remote_addr, pkt_t &pkt)throw()
{
	nslan_keyid_t	keyid;
	nslan_pkttype_t	pkttype;
	nslan_rec_arr_t	rec_arr;
	// log to debug
	KLOG_DBG("enter");
	
	try {
		// parse the incoming packet
		pkt >> keyid;
		pkt >> pkttype;
		pkt >> rec_arr;
		
		// handle the pkttype according to its value
		switch( pkttype.get_value() ){
		case nslan_pkttype_t::RECORD_REQUEST:{
				// get nslan_rec_arr_t 
				nslan_rec_arr_t	rec_arr	= get_publish_records(keyid);
				// send back the reply packet IIF !rec_arr.empty()
				if( !rec_arr.empty() ){
					pkt_t	reply	= build_reply(keyid, rec_arr);
					nslan_listener->send_pkt(remote_addr, reply);
				}
				// go pass the rec_arr to the nslan_query_t maching the keyid
				break;}
		case nslan_pkttype_t::RECORD_REPLY:
				// go pass the rec_arr to the nslan_query_t maching the keyid
				break;
		default:	KLOG_ERR("Unexpected pkttype=" << pkttype.get_value() );
				return;
		}		
	} catch(serial_except_t &e) {
		KLOG_ERR("Received bogus request");
		return;
	}

	// if the rec_arr is empty, return now
	if( rec_arr.empty() )	return;
	// get the first element >= than the keyid, or .end() if none exist
	std::multimap<nslan_keyid_t, nslan_query_t *>::iterator	iter	= query_db.lower_bound( keyid );
	// pass the packet along to all nslan_query_t which match this keyid
	for( ; iter != query_db.end() && iter->first == keyid; iter++ ){
		nslan_query_t *	nslan_query	= iter->second;
		// pass the nslan_rec_arr_t to this nslan_peer
		nslan_query->queue_incoming_rec_arr(rec_arr, remote_addr);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      build packet
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a RECORD_REPLY pkt_t
 */
pkt_t	nslan_peer_t::build_reply(const nslan_keyid_t &keyid
				, const nslan_rec_arr_t &rec_arr)	const throw()
{
	pkt_t	pkt;
	// sanity check - this function should NOT be called for rec_arr.empty
	DBG_ASSERT( !rec_arr.empty() );
	// put the realmid which is queried
	pkt << realmid;
	// put the keyid which is queried
	pkt << keyid;
	// put the packet type
	pkt << nslan_pkttype_t(nslan_pkttype_t::RECORD_REPLY);
	// put the locally published records for this keyid
	pkt << rec_arr;
	// return the packet
	return pkt;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      get_published_record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a nslan_rec_arr_t of all the published records matching this keyid
 */
nslan_rec_arr_t	nslan_peer_t::get_publish_records(const nslan_keyid_t &keyid)	const throw()
{
	std::multimap<nslan_keyid_t, nslan_publish_t *>::const_iterator	iter;
	nslan_rec_arr_t	result;
	// get the first element >= than the keyid, or .end() if none exist
	iter	= publish_db.lower_bound( keyid );
	// scan all the elements with the proper key
	for( ; iter != publish_db.end() && iter->first == keyid; iter++ ){
		const nslan_publish_t *	nslan_publish	= iter->second;
		result.append( nslan_publish->get_record() );
	}
	// log to debug
	KLOG_DBG("nslan_rec_arr.size()="<< result.size());
	// return the result
	return result;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           handle the publish_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief link a new nslan_publish_t into the nslan_peer_t
 */
void nslan_peer_t::publish_link(nslan_publish_t *nslan_publish)	throw()
{
	// insert this realm in the publish_db
	nslan_keyid_t	keyid	= nslan_publish->get_record().get_keyid();
	publish_db.insert(std::make_pair(keyid, nslan_publish));
}

/** \brief unlink a new nslan_publish_t into the nslan_peer_t
 */
void nslan_peer_t::publish_unlink(nslan_publish_t *nslan_publish)	throw()
{
	std::multimap<nslan_keyid_t, nslan_publish_t *>::iterator	iter;
	nslan_keyid_t	keyid	= nslan_publish->get_record().get_keyid();
	// get the first element >= than the keyid, or .end() if none exist
	iter	= publish_db.lower_bound( keyid );
	// sanity check - the nslan_publish MUST be in the publish_db
	DBG_ASSERT( iter != publish_db.end() );
	// scan all the elements with the proper key
	for( ; iter != publish_db.end() && iter->first == keyid; iter++ ){
		// if this element is NOT the nslan_publish to unlink, skip it
		if( iter->second != nslan_publish )	continue;
		// delete the current element 
		publish_db.erase(iter);
		// exit the loop
		break;
	}
	// sanity check - the realm MUST have been found
	DBG_ASSERT( iter != publish_db.end() );
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           handle the query_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief link a new nslan_query_t into the nslan_peer_t
 */
void nslan_peer_t::query_link(nslan_query_t *nslan_query)	throw()
{
	// insert this realm in the query_db
	nslan_keyid_t	keyid	= nslan_query->get_keyid();
	query_db.insert(std::make_pair(keyid, nslan_query));
}

/** \brief unlink a new nslan_query_t into the nslan_peer_t
 */
void nslan_peer_t::query_unlink(nslan_query_t *nslan_query)	throw()
{
	std::multimap<nslan_keyid_t, nslan_query_t *>::iterator	iter;
	nslan_keyid_t	keyid	= nslan_query->get_keyid();
	// get the first element >= than the keyid, or .end() if none exist
	iter	= query_db.lower_bound( keyid );
	// sanity check - the nslan_query MUST be in the query_db
	DBG_ASSERT( iter != query_db.end() );
	// scan all the elements with the proper key
	for( ; iter != query_db.end() && iter->first == keyid; iter++ ){
		// if this element is NOT the nslan_query to unlink, skip it
		if( iter->second != nslan_query )	continue;
		// delete the current element 
		query_db.erase(iter);
		// exit the loop
		break;
	}
	// sanity check - the realm MUST have been found
	DBG_ASSERT( iter != query_db.end() );
}


NEOIP_NAMESPACE_END


