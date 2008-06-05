/*! \file
    \brief Declaration of the nslan_query_t

*/

/* system include */
/* local include */
#include "neoip_nslan_query.hpp"
#include "neoip_nslan_peer.hpp"
#include "neoip_nslan_listener.hpp"
#include "neoip_nslan_event.hpp"
#include "neoip_nslan_pkttype.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nslan_query_t::nslan_query_t(nslan_peer_t *nslan_peer, nslan_keyid_t &keyid, delay_t expire_delay
				, nslan_query_cb_t *callback, void *userptr)	throw()
{
	// copy the parameters
	this->nslan_peer	= nslan_peer;
	this->keyid		= keyid;
	this->callback		= callback;
	this->userptr		= userptr;	
	// link this object to the nslan_peer
	nslan_peer->query_link(this);
	// start the expire timeout
	expire_timeout.start(expire_delay, this, NULL);
	// configure the rxmit_delaygen
	// TODO this delaygen should be in the nslan_profile_t
	rxmit_delaygen	= delaygen_t(delaygen_expboff_arg_t().min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(3*60))
								.random_range(0.2));
	rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
}

/** \brief Desstructor
 */
nslan_query_t::~nslan_query_t()		throw()
{
	// unlink this object from the nslan_peer
	nslan_peer->query_unlink(this);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool nslan_query_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	if( &cb_timeout == &rxmit_timeout )	return rxmit_timeout_cb();
	else					return expire_timeout_cb();
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
}

/** \brief callback called when the neoip_timeout expire
 */
bool nslan_query_t::rxmit_timeout_cb()	throw()
{	
	// log to debug
	KLOG_DBG("Send a request for the realmid=" << nslan_peer->get_realmid()
						<< " and keyid=" << keyid);
	// send a request
	nslan_listener_t*nslan_listener	= nslan_peer->get_nslan_listener();
	ipport_addr_t	dest_addr	= nslan_listener->get_listen_addr();
	pkt_t		request		= build_request();
	inet_err_t	inet_err	= nslan_listener->send_pkt(dest_addr, request);
	// update the rxmit_timeout
	rxmit_timeout.change_period( rxmit_delaygen.pre_inc() );
	// return tokeep
	return true;
}

/** \brief callback called when the neoip_timeout expire
 */
bool nslan_query_t::expire_timeout_cb()	throw()
{
	// log to debug
	KLOG_DBG("nslan_query_t timedout");
	// notify the event
	nslan_event_t	nslan_event = nslan_event_t::build_timedout("");
	// stop the timeout
	expire_timeout.stop();
	// notify the caller
	return notify_callback(nslan_event);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     packet building
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Build a RECORD_REQUEST pkt_t
 */
pkt_t	nslan_query_t::build_request()	const throw()
{
	pkt_t		pkt;
	// put the realmid which is queried
	pkt << nslan_peer->get_realmid();
	// put the keyid which is queried
	pkt << keyid;
	// put the packet type
	pkt << nslan_pkttype_t(nslan_pkttype_t::RECORD_REQUEST);
	// put the locally published records for this keyid
	pkt << nslan_peer->get_publish_records(keyid);
	// return the packet
	return pkt;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  incoming nslan_rectlist_t queue management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief queue incoming nslan_rec_arr_t
 * 
 * - NOTE: this is important to queue it and process it after a zerotimer
 *   - if it was processed immediatly, some tokeep issue would occurs as the processing
 *     trigger some callback notification
 */
void	nslan_query_t::queue_incoming_rec_arr(const nslan_rec_arr_t &nslan_rec_arr
					, const ipport_addr_t &src_addr)	throw()
{
	// log to debug
	KLOG_DBG("enter nb_rec=" << nslan_rec_arr.size() << " src_addr=" << src_addr);
	// put the whole nslan_rec_arr in the recved_record_queue
	for(size_t i = 0; i < nslan_rec_arr.size(); i++ )
		recved_record_queue.push_back(std::make_pair(nslan_rec_arr[i], src_addr));
	// launch the recved_pkt zerotimer is not already done
	if( deliver_zerotimer.empty() )	deliver_zerotimer.append(this, NULL);
	
}

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - this zerotimer_t is done to decouple the incoming packet delivery and the 
 *   callback notification.
 *   - as the callback notification may delete any object, nslan_peer_t and
 *     nslan_listener_t included, it could create issues about the 'tokeep' return
 */
bool	nslan_query_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	nslan_event_t	nslan_event;
	bool		tokeep;	
	// log to debug
	KLOG_DBG("enter");
	// unqueue all the nslan_rec_t one by one
	while( !recved_record_queue.empty() ){
		// copy the front element 
		nslan_rec_t	nslan_rec	= recved_record_queue.front().first;
		ipport_addr_t	src_addr	= recved_record_queue.front().second;
		// unqueue the front element
		recved_record_queue.pop_front();
		// build the event
		nslan_event	= nslan_event_t::build_got_record(nslan_rec, src_addr);
		// notify the event
		tokeep		= notify_callback(nslan_event);
		if( !tokeep )	return false;
	}
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	nslan_query_t::notify_callback(const nslan_event_t &nslan_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_nslan_query_cb(userptr, *this, nslan_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}
NEOIP_NAMESPACE_END


