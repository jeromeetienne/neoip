/*! \file
    \brief Implementation of the router_acache_t

- TODO handle router_peer_t to get a better wikidbg
- TODO handle the lastseen date to determine when to reply a ICMP to the itor

- TODO is there a relation with the cache and the ACL is it possible
  to get a item in the case which isnt in the ACL ? is that important ?
  - i dunno, my memory of this code is low at the moment

*/

/* system include */
/* local include */
#include "neoip_router_acache.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief insert an element into the router_acache_t
 */
void	router_acache_t::update(const router_name_t &remote_dnsname, const ip_addr_t &local_iaddr
		, const ip_addr_t &remote_iaddr, const delay_t &expire_delay)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - remote_dnsname MUST be is_fully_qualified()
	DBG_ASSERT( remote_dnsname.is_fully_qualified() );
	// sanity check - local_iaddr MUST be is_fully_qualified()
	DBG_ASSERT( local_iaddr.is_fully_qualified() );
	// sanity check - remote_iaddr MUST be is_fully_qualified()
	DBG_ASSERT( remote_iaddr.is_fully_qualified() );
	// build a router_acache_item_t
	router_acache_item_t	acache_item;
	acache_item.remote_dnsname	(remote_dnsname);
	acache_item.local_iaddr		(local_iaddr);
	acache_item.remote_iaddr	(remote_iaddr);
	// insert the router_acache_item_t in the database
	expireset2_t<router_acache_item_t>::update(acache_item, expire_delay);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the ip_addr_t matching this router_name_t, or a null one if none match
 */
const router_acache_item_t * router_acache_t::find_by_remote_dnsname(const router_name_t &remote_dnsname) const throw()
{
	// go thru the whole item_db
	for(size_t i = 0; i < size(); i++){
		const router_acache_item_t& acache_item	= (*this)[i];
		// if this item match, return it
		if(acache_item.remote_dnsname() == remote_dnsname)	return &acache_item;
	}
	// if no matching item is found, return a null one
	return NULL;
}

/** \brief return the router_acache_item_t matching this ip_addr_t, or a null one if none match
 */
const router_acache_item_t *router_acache_t::find_by_remote_iaddr(const ip_addr_t &remote_iaddr)	const throw()
{
	// go thru the whole item_db
	for(size_t i = 0; i < size(); i++){
		const router_acache_item_t& acache_item	= (*this)[i];
		// if this item match, return it
		if(acache_item.remote_iaddr() == remote_iaddr)		return &acache_item;
	}
	// if no matching item is found, return a null one
	return NULL;
}

NEOIP_NAMESPACE_END






