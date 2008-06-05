/*! \file
    \brief Definition of the ntudp_pserver_pool_t

\par Brief Description
- this modules allow to get the addresses of the public server. and to maintain
  their reachability state (reach/unreach/unknown)
  - It get the address from querying a underlying name service (here a kademlia dht)

\par Decription of the UNKNOWN state
- \ref ntudp_pserver_pool_t allows to discover new items of unknown state.
- a item is of unknown state if 
  -# it is NOT in the reach_db cache
  -# it is NOT in the unreach_db cache
  -# it is NOT the one issued from the local ntudp_peer_t

\par Decription of the REACH state
- an item in the reach_db implies that a packet has been recently received from it.
  - here 'recently' means in the last ntudp_pserver_pool_t::REACH_EXPIRE_DELAY
- EACH time a packet is received from a public server, its address MUST be 
  notified to the \ref ntudp_pserver_pool_t as reachable.
  
\par Explaination about the noquery phase
- timeout_noquery/delaygen_noquery are here to limit useless query on the underlying
  name service in case no new unknown are available.
- the algorithm is :
  - delaygen_noquery is the exponantial backoff delay_t generator which feed
    the timeout_noquery.
  - after a query, wait for the next delay of delaygen_noquery before asking trigerring
    a new query.
  - if a query notifies a item which is unknown, reset the delaygen_noquery
- example of usefullness:
  - if the caller of ntudp_pserver_pool_t want 4 public servers and only 3 are available
    in the underlying name server, it would be a waste of resource to continuously query
    the underlying name service.
  - so timeout_noquery regulates the delay between query of the underlying  name service.
    and delaygen_noquery ensures that this delay increases if a query fails to provide
    item of unknown state, and is reduced if it succeed.

\par About the reachability tracking
- If it is reachable, it is put in the reach_db cache with a expiration delay
  (i.e. ntudp_pserver_pool_t::REACH_EXPIRE_DELAY)
- If it is unreachable, it is put in the unreach_db cache with a expiration delay
  (i.e. ntudp_pserver_pool_t::UNREACH_EXPIRE_DELAY)
- If it is unknown, it is put in the unknown_db cache with the ttl of the record
  as expiration delay.

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_pserver_rec.hpp"
#include "neoip_kad.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_ntudp_pserver_pool_wikidbg.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_pserver_pool_t::ntudp_pserver_pool_t()		throw()
{
	// zero some fields
	ntudp_peer		= NULL;
	// initialize the event_hook_t to notify the new unknown address
	new_unknown_hook	= event_hook_t(1);
	want_more_unknown	= false;
	kad_query		= NULL;
}

/** \brief destructor
 */
ntudp_pserver_pool_t::~ntudp_pserver_pool_t()		throw()
{
	// delete the kad_query if needed
	nipmem_zdelete kad_query;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      Setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_pserver_pool_t &	ntudp_pserver_pool_t::set_profile(const ntudp_pserver_pool_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}
/** \brief Start the operation
 */
ntudp_err_t	ntudp_pserver_pool_t::start(ntudp_peer_t *ntudp_peer)	throw()
{
	// copy some parameter
	this->ntudp_peer	= ntudp_peer;

	// init the delaygen_noquery
	delaygen_noquery	= delaygen_t(profile.noquery_delaygen());
	// initialize the various database
	reach_db.set_default_ttl( profile.reach_expire() );
	unreach_db.set_default_ttl( profile.unreach_expire() );

	// return no error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     notify reachability of ipport_addr_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Notify a given ipport_addr_t as reachable
 */
void	ntudp_pserver_pool_t::notify_reach(const ipport_addr_t &pserver_addr)		throw()
{
	// erase it from the reach_db if it is present
	if( reach_db.contain(pserver_addr) )	reach_db.erase(pserver_addr);
	// erase it from the unreach_db if it is present
	if( unreach_db.contain(pserver_addr) )	unreach_db.erase(pserver_addr);
	// erase it from the unknown_db if it is present
	if( unknown_db.contain(pserver_addr) )	unknown_db.erase(pserver_addr);
	// insert it in the reach_db
	reach_db.insert(pserver_addr);	
}

/** \brief Notify a given ipport_addr_t as unreachable
 */
void	ntudp_pserver_pool_t::notify_unreach(const ipport_addr_t &pserver_addr)		throw()
{
	// erase it from the reach_db if it is present
	if( reach_db.contain(pserver_addr) )	reach_db.erase(pserver_addr);
	// erase it from the unreach_db if it is present
	if( unreach_db.contain(pserver_addr) )	unreach_db.erase(pserver_addr);
	// erase it from the unknown_db if it is present
	if( unknown_db.contain(pserver_addr) )	unknown_db.erase(pserver_addr);
	// insert it in the unreach_db
	unreach_db.insert(pserver_addr);	
}


/** \brief Notify a given ipport_addr_t as unreachable after a timeout delay
 * 
 * - the notified delay MUST be the one elapsed during the sending of the 
 *   first packet probe to the time of the expiration.
 */
void	ntudp_pserver_pool_t::notify_timedout(const ipport_addr_t &pserver_addr
					, const delay_t &timedout_delay)	throw()
{
	// sanity check - the timedout_delay MUST NOT be null
	DBG_ASSERT( timedout_delay.is_null() == false );
	// if the timedout_delay is below the pool limit, the pool doesnt consideres it unreachable
	if( timedout_delay < profile.unreach_min_timeout() )	return;
	// else considere it as unreachable
	notify_unreach(pserver_addr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         get_more_unknown
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Warn that more items are wished
 * 
 * - It may start a new query on the underlying name service if a noquery phase is not in 
 *   progress.
 * - When new item are available, the caller which subscribed to new_item_subscribe() will
 *   be notified.
 */
void	ntudp_pserver_pool_t::get_more_unknown()	throw()
{
	// record the wish of the caller
	want_more_unknown	= true;
	// if a kad_query is already running, dont relaunch one
	if( kad_query )		return;
	// if the timeout_noquery is running, dont ask until the timeout is expired
	// - a get_more_unknown will be triggered on the timeout expiration
	if( timeout_noquery.is_running() )		return;
	// log to debug
	KLOG_DBG("launch a new query for pserver on the underlying NS");

	// init some variable for the kad_query_t
	kad_keyid_t	record_keyid	= ntudp_peer->get_profile().peer().pserver_key_str();
	kad_peer_t *	kad_peer	= ntudp_peer->kad_peer();
	kad_err_t	kad_err;
	// create and start the kad_query_t with no expire delay
	kad_query	= nipmem_new kad_query_t();
	// TODO i dont like this delay_t::ALWAYS here.... i need a actual timeout
	kad_err		= kad_query->start(kad_peer, record_keyid, 0, delay_t::ALWAYS, this, NULL);
	// handle the error - NOTE: this error SHOULD NEVER happen
	if( kad_err.failed() ){
		// log the error
		KLOG_ERR("Cant start a kad_query due to " << kad_err);
		// assert anyway
		DBG_ASSERT( 0 );
		// delete the kad_query and mark it unused
		nipmem_zdelete	kad_query;
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_query_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a command in a kad_query_t has an event to notify
 */
bool	ntudp_pserver_pool_t::neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << kad_event);
	// sanity check - the kad_event MUST be query_ok()
	DBG_ASSERT( kad_event.is_query_ok() );
	// sanity check - the reporter cb_kad_query MUST be the local one
	DBG_ASSERT( kad_query == &cb_kad_query );
	// sanity check - if the kad_query is running, the timeout_noquery MUST NOT running
	DBG_ASSERT( !timeout_noquery.is_running() );

	// sanity check - the kad_event_t MUST be a kad_event_t::RECDUPS
	// - kad_event_t::TIMEDOUT is possible on kad_query but not here as the delay is delay_t::NEVER
	DBG_ASSERT( kad_event.is_recdups() );

	// delete the kad_query_t and mark it unused
	nipmem_zdelete kad_query;

	// handle the received kad_recdups_t			
	handle_recved_kad_recdups(kad_event.get_recdups(NULL));
	// return 'dontkeep' as the command has just been deleted
	return false;
}

/** \brief Handle the received kad_recdups
 */
void	ntudp_pserver_pool_t::handle_recved_kad_recdups(const kad_recdups_t &kad_recdups)	throw()
{
	bool	introduced_new_unknowns	= false;
	// log to debug
	KLOG_DBG("enter");
	// go thru all the received kad_rec_t
	for(size_t i = 0; i < kad_recdups.size(); i++){
		const kad_rec_t &	kad_rec	= kad_recdups[i];
		ntudp_pserver_rec_t	pserver_rec;
		bytearray_t		payload(kad_rec.get_payload());
		// parse the incoming packet
		try {
			payload	>> pserver_rec;
		}catch(serial_except_t &e){
			// log the event
			KLOG_ERR("Can't parse pserver record " << kad_rec << " due to " << e.what() );
			// goto the next record even if it is a serious error which MUST NOT happen
			continue;
		}
		// extract the pserver address from the ntudp_pserver_rec_t
		ipport_addr_t	pserver_addr	= pserver_rec.listen_addr_pview();
		ntudp_peerid_t	pserver_peerid	= pserver_rec.peerid();
		// handle this particular pserver address
		bool		is_new_unknown;
		is_new_unknown	= handle_recved_pserver_addr(pserver_addr, pserver_peerid, kad_rec.get_ttl());
		// update 'introduced_new_unknown' if this pserver_addr is a new unknown
		if( is_new_unknown )	introduced_new_unknowns	= true;
	}

	// if this kad_recdups_t introduced new unknown, reset the delaygen_noquery
	if( introduced_new_unknowns )	delaygen_noquery.reset();
	// sanity check - ensure the delaygen_noquery is not configured to timeout
	DBG_ASSERT( delaygen_noquery.get_arg().timeout_delay().is_special() );
	// start the timeout_noquery
	timeout_noquery.start(delaygen_noquery.pre_inc(), this, NULL);		
	// if this kad_recdups_t introduced new unknown, notify the registered caller
	if( introduced_new_unknowns ){
		// mark want_more_unknown as false
		want_more_unknown	= false;
		// notify the new unknown
		new_unknown_hook.notify(0);
	}
}

/** \brief Handle a received pserver address
 * 
 * @return true if this address is a new unknown, false otherwise
 */
bool	ntudp_pserver_pool_t::handle_recved_pserver_addr(const ipport_addr_t &pserver_addr
						, const ntudp_peerid_t &pserver_peerid
						, const delay_t &record_ttl)	throw()
{
	// log to debug
	KLOG_DBG("enter pserver_addr=" << pserver_addr << " ttl=" << record_ttl);
	// if this pserver is the local one, discard it and return 'notnew'
	if( pserver_peerid == ntudp_peer->local_peerid() )		return false;
	// if this pserver_addr is already in reach_db or unreach_db, return 'notnew'
	if( reach_db.contain(pserver_addr) || unreach_db.contain(pserver_addr) )	return false;
	// if this pserver_addr is already in unknown_db, update it and return 'notnew'
	if( unknown_db.contain(pserver_addr) ){
		unknown_db.erase(pserver_addr);
		unknown_db.insert(pserver_addr, record_ttl);
		return false;
	}
	// If this point is reached, this pserver_addr is a new unknown, insert it in the database
	unknown_db.insert(pserver_addr, record_ttl);
	// return 'isnew'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       noquery phase time out
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 * 
 * - when timeout_noquery expires, a new query is made on the underlying name service
 */
bool	ntudp_pserver_pool_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter want_more_unknown=" << std::boolalpha << want_more_unknown);
	// sanity check - if the timeout_noquery was running, kad_query should not
	DBG_ASSERT( kad_query == NULL );
	// stop the timer
	timeout_noquery.stop();	
	// if a caller asked for more, ask more now that noquery period is over
	if( want_more_unknown )	get_more_unknown();
	// return a 'tokeep'
	return true;
}


NEOIP_NAMESPACE_END



