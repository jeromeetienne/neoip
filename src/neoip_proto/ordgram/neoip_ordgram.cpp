/*! \file
    \brief Definition of the \ref ordgram_t

\par NOTE
- ordgram_t supports duplicates from rdgram_t
  - obviously ordgram_t dedups the packets and deliver a given packet only once.
*/


/* system include */
/* local include */
#include "neoip_ordgram.hpp"
#include "neoip_rdgram_pkttype.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ordgram_t constant
// the default length of the receive buffer
const size_t	ordgram_t::RECVBUF_MAX_LEN_DFL	= 30*1024;
// end of constants definition


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  		  ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ordgram_t::ordgram_t()	throw()
{
	next_seqnb_to_deliver	= rdgram_t::FIRST_SEQNB;
	recvbuf_max_len		= ordgram_t::RECVBUF_MAX_LEN_DFL;
	recvbuf_used_len	= 0;
	callback		= NULL;
}

/** \brief Destructor
 */
ordgram_t::~ordgram_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       start() functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
rdgram_err_t	ordgram_t::start()		throw()
{
	// sanity check - the callback MUST be set
	DBG_ASSERT( callback );	
	// start rdgram
	rdgram_err_t	rdgram_err = rdgram.start(this, NULL);
	if( rdgram_err.failed() )	return rdgram_err;
	
	return rdgram_err_t::OK;
}

/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
rdgram_err_t	ordgram_t::start(ordgram_cb_t *callback, void *userptr)	throw()
{
	rdgram_err_t	rdgram_err;
	// set the callback
	rdgram_err = set_callback(callback, userptr);
	if( rdgram_err.failed() )	return rdgram_err;
	// start the action
	return start();
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    		????????????
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the event callback
 */
rdgram_err_t ordgram_t::set_callback(ordgram_cb_t * callback, void *userptr)	throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
	return rdgram_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       rdgram callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when \ref rdgram_t send an event.
 * 
 * @return true if the ordgram_t is still valid after the callback, false otherwise
 */
bool ordgram_t::neoip_rdgram_event_cb(void *cb_userptr, rdgram_t &cb_rdgram
					, const rdgram_event_t &rdgram_event )	throw()
{
	ordgram_event_t	ordgram_event;

	// handle the event depending of its type
	switch(rdgram_event.get_value()){
	case rdgram_event_t::MAYSEND_OFF:
			// simply convert the event
			ordgram_event = ordgram_event_t::build_maysend_off();
			return callback->neoip_ordgram_event_cb(userptr, *this, ordgram_event);
	case rdgram_event_t::MAYSEND_ON:
			// simply convert the event
			ordgram_event = ordgram_event_t::build_maysend_on();
			return callback->neoip_ordgram_event_cb(userptr, *this, ordgram_event);
	case rdgram_event_t::PKT_TO_LOWER:
			// simply convert the event
			ordgram_event = ordgram_event_t::build_pkt_to_lower(rdgram_event.get_pkt_to_lower());
			return callback->neoip_ordgram_event_cb(userptr, *this, ordgram_event);
	default:	DBG_ASSERT(0);
	}
	return true;
}

/** \brief callback notified when \ref rdgram_t just receive a packet and wish to know
 *         if this packet must be accepted or not.
 * 
 * - it isnt allowed to delete rdgram_t during this callback
 * 
 * @return true if the packet must be accepted, false otherwise
 */	
bool ordgram_t::neoip_rdgram_should_acked_pkt_cb(void *cb_userptr, rdgram_t &cb_rdgram
					, const pkt_t &pkt, const wai32_t &seqnb)	throw()
{
	// loggin to debug
	KLOG_DBG("should acked seqnb=" << seqnb );
 	// if the packet has already been delivered, acknowledge the packet
	if( seqnb < next_seqnb_to_deliver )				return true;
	// if the packet is already present in the recvbuf, acknowledge the packet
	if( recvbuf_pkt_db.find(seqnb) != recvbuf_pkt_db.end() )	return true;
	// if the packet is too large to be stored in the recvbuf, dont acknowledge it
	if( recvbuf_used_len + pkt.get_len() > recvbuf_max_len )	return false;
	// else acknowledge it
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer_delivery callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - delivery as many packet as possible
 * - delivery is done thru rdgram_event_t::PKT_TO_LOWER
 */
bool	ordgram_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	std::map<wai32_t, recvbuf_pkt_t>::iterator	iter;
	ordgram_event_t					ordgram_event;

// remove from the recvbuf all packets already delivered
	// - it may happen with the following scenario
	//   - a seqnb = 64 is received but 63 is expected
	//   - so 64 is put in the recvbuf and zerotimer is schedule
	//   - the 63 is received
	//   - the 64 is received thru pkt_from_lower(), BEFORE the zerotimer callback is notified.
	//   - the 64 is delivered synchronously thru the filter, so next_seqnb_to_deliver = 65
	//   - BUT 64 is still in the recvbuf.
	for( iter = recvbuf_pkt_db.begin(); iter != recvbuf_pkt_db.end(); iter = recvbuf_pkt_db.begin()){
		recvbuf_pkt_t &	recvbuf_pkt = iter->second;
		// if the recvbuf_pkt seqnb is >= to the next_seqnb_to_deliver, exit the loop
		if( recvbuf_pkt.get_seqnb() >= next_seqnb_to_deliver )	break;
		// remove the obsolete packet
		recvbuf_pkt_db.erase(iter);
	}	

// deliver more packets if possible
	// loop until the recvbuf_pkt_db is empty (or other conditions inside the loop)
	while( recvbuf_pkt_db.empty() == false ){
		// test if the recvbuf contains the next seqnb to deliver
		iter	= recvbuf_pkt_db.find(next_seqnb_to_deliver);
		
		// if next_seqnb_to_deliver is not found, return now
		if( iter == recvbuf_pkt_db.end() )	break;
		
		// get the first recvbuf_pkt of the database
		recvbuf_pkt_t &	recvbuf_pkt = iter->second;
		
		// update the recvbuf_used_len
		recvbuf_used_len -= recvbuf_pkt.get_pkt().get_len();

		// deliver the packet thru event notification
		ordgram_event = ordgram_event_t::build_pkt_to_upper(&recvbuf_pkt.get_pkt());
		bool to_keep = callback->neoip_ordgram_event_cb(userptr, *this, ordgram_event);
		if( !to_keep )	return false;

		// remove the packet from the database
		recvbuf_pkt_db.erase(iter);
		
		// update the next_seqnb_to_deliver
		next_seqnb_to_deliver++;
	}

	// return true to show zerotimer_t is still valid
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       packet from lower/upper
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief process incoming packet when it is known to be out of order
 */
void ordgram_t::pkt_from_lower_outoforder(pkt_t &pkt, wai32_t seqnb)	throw()
{
	recvbuf_pkt_t	recvbuf_pkt(seqnb, pkt);
	// sanity check - the packet MUST be outoforder
	DBG_ASSERT(seqnb != next_seqnb_to_deliver);

	// if the packet has been already delivered, just swallow the packet
	if( seqnb < next_seqnb_to_deliver )			goto swallow_pkt;

	// if the packet is already present in the recvbuf, just swallow the packet
	if( recvbuf_pkt_db.find(seqnb) != recvbuf_pkt_db.end())	goto swallow_pkt;

	// NOTE: here the packet MUST be added to the recvbuf
	// sanity check - the recvbuf MUST be large enougth to store the packet
	DBG_ASSERT(recvbuf_used_len + pkt.get_len() <= recvbuf_max_len );
	// update the recvbuf_used_len
	recvbuf_used_len += recvbuf_pkt.get_pkt().get_len();
	// insert the sendbuf_pkt in the database
	{bool	succeed = recvbuf_pkt_db.insert(std::make_pair(seqnb, recvbuf_pkt)).second;
	DBG_ASSERT( succeed );}
	
swallow_pkt:;	// swallow the packet
	pkt = pkt_t();	
}

/** \brief handle packet from the upper network layer
 */
rdgram_err_t	ordgram_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	// simply forward it to rdgram
	return rdgram.pkt_from_upper(pkt);
}

/** \brief handle packet from the lower network layer
 */
rdgram_err_t	ordgram_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	rdgram_err_t	rdgram_err;
	wai32_t		seqnb;
// go thru rdgram_t
	// filter this packet
	rdgram_err	= rdgram.pkt_from_lower(pkt, seqnb);
	// if the packet produced an error, return now
	if( !rdgram_err.succeed() )	return rdgram_err;
	// if the packet has been swallowed, return now
	if( pkt.is_null() )		return rdgram_err;

	// if this seqnb has already been delivered, just swallow the packet
	if( seqnb < next_seqnb_to_deliver ){
		pkt	= pkt_t();
		return rdgram_err_t::OK;
	}
	// if the filtered packet is not in order, store it locally, swallow it and return
	if( seqnb > next_seqnb_to_deliver ){
		pkt_from_lower_outoforder(pkt, seqnb);
		return rdgram_err_t::OK;
	}
	// NOTE: the just received packet is in order. so deliver it thru the filter

	// update the next_seqnb_to_deliver
	next_seqnb_to_deliver++;

	// if there are packet in the recvbuf_pkt_db, schedule a packet delivery
	// - via zerotimer to avoid recursive callback issues
	if( !recvbuf_pkt_db.empty() )	zerotimer_deliver.append(this, NULL);
	
	return rdgram_err_t::OK;
}
NEOIP_NAMESPACE_END






