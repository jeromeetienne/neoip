/*! \file
    \brief Definition of the \ref kad_db_t

\par About inter-storer republication
- the kademlia describes in the paper does a inter-storer republication for the 
  remote database every hour.
  - the only motivation i could find to do so is to keep the record published
    even when the publisher is offline. i.e. to offer some persistance in the database
    during the whole record ttl.
- in some case this feature is useless and consume unnecessary network rescource
  - For example, when the records are usefull IIF the publisher is online e.g. if 
    they shows a presence of the publisher, to provide a persistance of the record
    when the publisher is offline is pointless.
    - i2p and bittorrent dht do it this way
- this implementation of kad allows both behaviour via kad_db_profile_t tuning.

\par no inter-storer republication
- to prevent inter-storer republication, set the remote_db().rec_republish_period
  to delay_t::ALWAYS.
- to do periodic republication by the publisher, independantly of the record ttl,
  set local_db().rec_republish_period to the whished value
  - the record will be republished either when the rec_republish_period expire
    or when the record ttl is close to expire. whichever comes first

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_db_rec.hpp"
#include "neoip_kad_db_republish.hpp"
#include "neoip_kad_peer.hpp"
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
kad_db_t::rec_t::rec_t(kad_db_t *kad_db, const kad_rec_t &kad_rec)	throw()
{
	// copy parameters
	this->kad_db	= kad_db;
	this->kad_rec	= kad_rec;
	
	// log to debug
	KLOG_DBG("republish delay=" << cpu_republish_delay() << "  expire delay=" << cpu_expire_delay()
								<< " for " << kad_rec);
	// to set republish timeout
	republish_timeout.start(cpu_republish_delay(), this, NULL);	
	// to set the expire_timeout
	expire_timeout.start(cpu_expire_delay(), this, NULL);

	// do link this record in the database
	kad_db->rec_dolink(this);
}

/** brief Destructor 
 */
kad_db_t::rec_t::~rec_t()						throw()
{
	// unlink this record in the database
	kad_db->rec_unlink(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   QUERY function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return a kad_rec_t describing this rec_t (with an updated ttl)
 */
kad_rec_t	kad_db_t::rec_t::get_kad_rec()	const throw()
{
	// copy the record, in case of modification
	kad_rec_t	result	= kad_rec;	
	// on remote database, the updated_ttl is the delay between now and the expiration
	// - on the local database, the updated_ttl is the one of the original record, no update is made
	if( kad_db->is_remote_db() )	result.set_ttl( expire_timeout.get_expire_delay() );
	// return the result
	return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                        timer computation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief compute the republish delay for this record
 */
delay_t	kad_db_t::rec_t::cpu_republish_delay()		const throw()
{
	const kad_db_profile_t &	profile	= kad_db->get_profile();
	// on remote database, republish based on the kad_db->republish_period
	// - put some randomness to reduce the number of packet. in 2nd paper, section 2.5:
	//   "First, when a node receives a store RPC for a given key-value pair, it assumes
	//    the RPC was also issued to the other k − 1 closest nodes, and thus the recipient
	//    will not republish the key-value pair in the next hour. This ensures that as long
	//    as republication intervals are not exactly synchronized, only one node will
	//    republish a given key-value pair every hour."
	if( kad_db->is_remote_db() )	return profile.rec_republish_period() * neoip_rand(0.7, 1.0);


	// on local database, the record is republished at least once before ttl expiration
	// - NOTE: the republication MUST be triggered a bit before the actual expiration
	//         of the kad_rec_t ttl in all the peers.
	// - the amount of time depends on the profile.republish_store_timeout() as it is 
	//   the maximum amount of time the republication will take.
	delay_t	from_ttl= kad_rec.get_ttl().a_bit_less(profile.republish_store_timeout(), 2.0/3.0);
	
	// compute the from_db delay - the delay_t computed according to the profile.rec_republish_period()
	// independantly of the record ttl
	delay_t	from_db	= delay_t::ALWAYS;
	if( !profile.rec_republish_period().is_special() )
		from_db = profile.rec_republish_period() * neoip_rand(0.7, 1.0);
	
	// return from_ttl or from_db whichever comes first
	return std::min(from_ttl, from_db);	
}

/** \brief compute the expire delay for this record
 * 
 * - when this delay expires, the record is removed from the database
 */
delay_t	kad_db_t::rec_t::cpu_expire_delay()		const throw()
{
	// on remote database, the record expires when the ttl expire
	if( kad_db->is_remote_db() )	return kad_rec.get_ttl();

	// on local database, the record never expire - it is up to the caller to remove it
	return delay_t::NEVER;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                        timeout handling
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	kad_db_t::rec_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout) throw()
{
	if( &cb_timeout == &republish_timeout )	return republish_timeout_cb();
	else 					return expire_timeout_cb();
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );
	return true;
}

/** \brief Called when the republish_timeout expire
 */
bool	kad_db_t::rec_t::republish_timeout_cb()	throw()
{
	// launch a republication
	kad_err_t	kad_err = kad_db->db_republish->republish_rec(kad_rec);
	// if the republish failed to start, log the event
	if( kad_err.failed() )
		KLOG_INFO("Automatic republication of " << kad_rec << " failed due to " << kad_err);

	// update the timer period
	republish_timeout.change_period(cpu_republish_delay());	
	// return tokeep
	return true;
}

/** \brief Called when the expire_timeout expire
 */
bool	kad_db_t::rec_t::expire_timeout_cb()	throw()
{
	// if the record ttl expired, delete this record from the database
	nipmem_delete	this;
	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END;






