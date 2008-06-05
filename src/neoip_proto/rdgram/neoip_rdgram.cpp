/*! \file
    \brief Definition of the \ref rdgram_t

\par TODO
- TODO reorganize the functions order in this file
- how to handle the change of sendbuf_max_len in progress when the new
  length is < cur_len
  - currently it is a assert() to be sure nothing rely on it
- how to handle the change of maysendthreshold in progress
- BUG the size returns by the sendbuf functions doesnt includes the rdgram header
  - aka if free return 500byte, it is possible to written only 500-5byte because
    the header is 5byte long

*/


/* system include */
/* local include */
#include "neoip_rdgram.hpp"
#include "neoip_rdgram_pkttype.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref rdgram_t constant
//! the first sequence number to use
const wai32_t	rdgram_t::FIRST_SEQNB	= 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rdgram_t::rdgram_t()	throw()
{
	next_xmit_seqnb		= FIRST_SEQNB;
	sendbuf_used_len	= 0;
	sendbuf_max_len		= 0;
	maysend_threshold	= 0;
	callback		= NULL;
}

/** \brief Destructor
 */
rdgram_t::~rdgram_t()	throw()
{
	// delete all the pending packet
	while( !sendbuf_pkt_db.empty() )	nipmem_delete	sendbuf_pkt_db.begin()->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       set/get parameter
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the event callback
 */
rdgram_err_t rdgram_t::set_callback(rdgram_cb_t * callback, void *userptr)	throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
	return rdgram_err_t::OK;
}


/** \brief return the space overhead due to this brick in the mtu
 */
size_t	rdgram_t::get_mtu_overhead()				const throw()
{
	// build the header
	serial_t	serial;
	serial << rdgram_pkttype_t(rdgram_pkttype_t::DATA_PKT);
	serial << next_xmit_seqnb;
	return serial.get_len();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       start() functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
rdgram_err_t	rdgram_t::start()		throw()
{
	// NOTE: nothing to do to start the action
	// - it will react to packets from lower/upper
	
	// sanity check - the callback MUST be set
	DBG_ASSERT( callback );
	
	return rdgram_err_t::OK;
}

/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
rdgram_err_t	rdgram_t::start(rdgram_cb_t *callback, void *userptr)	throw()
{
	rdgram_err_t	rdgram_err;
	// set the callback
	rdgram_err = set_callback(callback, userptr);
	if( rdgram_err.failed() )	return rdgram_err;
	// start the action
	return start();
}

/** \brief stop the Action 
 * 
 * - NOTE: it can't be restarted!!
 */
void	rdgram_t::stop()			throw()
{
	// delete all the pending packet
	while( !sendbuf_pkt_db.empty() )	nipmem_delete	sendbuf_pkt_db.begin()->second;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    maysend_threshold function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief set the maysend_threshold
 * 
 * - this function will trigger a MAYSEND_ON or MAYSEND_OFF depending on the
 *   current length of the sendbuf. it is done via a zerotimer_t to avoid calling
 *   callback here.
 */
rdgram_err_t	rdgram_t::maysend_set_threshold(size_t len)	throw()
{
	// set the maysend_threshold
	maysend_threshold	= len;
	
	// check is a MAYSEND_ON/OFF must be sent
	if( !maysend_is_set() )	return rdgram_err_t::OK;

	// if the free space in sendbuf is < new threshold, send a MAYSEND_OFF
	// - TODO what if the rdgram isnt yet started ?
	//   - no event should be notified
	//   - anyway the callback may not be set..
	// - possible solution:
	//   - set a specific variable if the rdgram_t has been started or not
	
	// TODO is this test good ?
	// - a MAYSEND event is notified anyhow... no matter the previous state?
	if( sendbuf_get_free_len() < maysend_threshold  ){
		// zerotimer to notify a MAYSEND_OFF
		zerotimer_maysend.append(this, NULL);
	}else{	// zerotimer to notify a MAYSEND_ON
		zerotimer_maysend.append(this, NULL);
	}
	return rdgram_err_t::OK;
}

/** \brief return the maysend_threashold
 */
size_t	rdgram_t::maysend_get_threshold()	const throw()
{
	return maysend_threshold;
}

/** \brief return true if the maysend_threshold is set, false otherwise
 * 
 * - maysend_threshold is considered set if it is different than 0 and if sendbuf
 *   is limited.
 */
bool	rdgram_t::maysend_is_set()	const throw()
{
	if( maysend_threshold == 0 )		return false;
	// maysend_threshold makes sense iif sendbuf is limited
	if( sendbuf_is_limited() == false )	return false;
	return true;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    sendbuf function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief update the sendbuf when add a packet 
 * 
 * - NOTE: the sendbuf MUST have enought room to accept this pkt_len
 *   - the check must be done before
 * - this function is used inside sendbuf_pkt_t
 */
void	rdgram_t::sendbuf_add_pkt( size_t pkt_len )	throw()
{
	sendbuf_used_len += pkt_len;
	// sanity check - the sendbuf_used_len MUST be <= to sendbuf_max_len
	// TODO likely in relation with the change of the sendbuf in progress
	if( sendbuf_is_limited() )	DBG_ASSERT( sendbuf_used_len <= sendbuf_max_len );

	// check is a MAYSEND_OFF must be sent
	if( !maysend_is_set() )	return;
	DBG_ASSERT( sendbuf_is_limited() );

	// if the available room is *NOW* (after adding this packet) less than
	// the maysend_threshold *AND* was equal to or greater than it *BEFORE*, notify a MAYSEND_OFF
	size_t	free_len	= sendbuf_get_free_len();
	if( free_len < maysend_threshold && free_len + pkt_len >= maysend_threshold )
		zerotimer_maysend.append(this, NULL);
}

/** \brief update the sendbuf when removing a packet 
 */
void	rdgram_t::sendbuf_del_pkt( size_t pkt_len )	throw()
{
	// sanity check - the sendbuf_used_len MUST be >= to the packet length
	DBG_ASSERT( sendbuf_used_len >= pkt_len );

	// update the sendbuf_used_len
	sendbuf_used_len -= pkt_len;

	// check is a MAYSEND_ON must be sent
	if( !maysend_is_set() )	return;
	DBG_ASSERT( sendbuf_is_limited() );


	// if the available room is *NOW* (after adding this packet) equal to or greater than the 
	// maysend_threshold *AND* was less than it *BEFORE*, notify a MAYSEND_ON
	size_t	free_len	= sendbuf_get_free_len();

	// log to debug
	KLOG_DBG("maysend_threshold=" << maysend_threshold);
	KLOG_DBG("pkt_len=" << pkt_len);
	KLOG_DBG("sendbuf freelen=" << free_len << " freelen-pkt_len=" << free_len - pkt_len);

	if( free_len >= maysend_threshold && free_len - pkt_len < maysend_threshold )
		zerotimer_maysend.append(this, NULL);
}

/** \brief return true if the sendbuf is limited, false otherwise
 */
bool	rdgram_t::sendbuf_is_limited()	const throw()
{
	if( sendbuf_max_len == 0 )	return false;
	return true;
}

/** \brief set the maximum length of the sendbuf
 * 
 * @param len the maximum length of the sendbuf (0 == unlimited)
 */
rdgram_err_t	rdgram_t::sendbuf_set_max_len(size_t len)	throw()
{
	// TODO what is the new length is larger than the current window ?
	// current it is assert()
	DBG_ASSERT( len >= sendbuf_used_len );
	// set the send_buf_max_len
	sendbuf_max_len	= len;
	return rdgram_err_t::OK;
}

/** \brief return the maximum length of the sendbuf
 */
size_t	rdgram_t::sendbuf_get_max_len()	const throw()
{
	return sendbuf_max_len;
}

/** \brief return the current length of the sendbuf
 */
size_t	rdgram_t::sendbuf_get_used_len()	const throw()
{
	return sendbuf_used_len;
}

/** \brief return the current free length of the sendbuf
 * 
 * - if sendbuf is unlimited, it returns "infinite" (aka std::numeric_limits<size_t>::max())
 */
size_t	rdgram_t::sendbuf_get_free_len()	const throw()
{
	// if sendbuf is unlimited, it returns "infinite"
	if( !sendbuf_is_limited() )	return std::numeric_limits<size_t>::max();
	// else return the maximum length - the current length
	return sendbuf_max_len - sendbuf_used_len;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         internal functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return a pointer of a \ref rdgram_t::sendbuf_pkt_t matching the seqnb, NULL if none exist
 */
rdgram_t::sendbuf_pkt_t *	rdgram_t::find_sendbuf_pkt(wai32_t seqnb)	throw()
{
	std::map<wai32_t, rdgram_t::sendbuf_pkt_t *>::const_iterator	iter;
	// try to find this sequence number pending in the sendbuf_pkt_db
	iter = sendbuf_pkt_db.find(seqnb);
	// if this sequence number is not in the sendbuf_pkt_db, return NULL;
	if( iter == sendbuf_pkt_db.end() )	return NULL;
	// return the found sendbuf_pkt
	return	iter->second;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the zerotimer_maysend expire
 * 
 * - send a maysend off or a maysend on depending on the current sendbuf usage
 * - NOTE: it is dynamically recomputed here (even if it could be done by the zerotimer 
 *   caller) because the status of sendbuf may have changed between the zerotimer.append()
 *   and the notification of the zerotimer event
 */
bool	rdgram_t::zerotimer_maysend_process()	throw()
{
	rdgram_event_t	rdgram_event;
	// if maysend_threshold isnt set, do nothing
	if( !maysend_is_set() )	return true;

	// if maysend is set and the sendbuf used length just became less than maysend_threshold
	// => notify a MAYSEND_OFF
	if( sendbuf_get_free_len() < maysend_threshold ){
		rdgram_event = rdgram_event_t::build_maysend_off();
	}else{	// else notify a MAYSEND_ON
		rdgram_event = rdgram_event_t::build_maysend_on();
	}

	return callback->neoip_rdgram_event_cb(userptr, *this, rdgram_event);
}

/** \brief callback called when zerotimer_seqnb2ack expire
 * 
 * - ack are sent thru a zerotimer_t to avoid callback notification during external function calls
 */
bool	rdgram_t::zerotimer_seqnb2ack_process()	throw()
{
	// go thru the whole list
	while( !seqnb2ack_list.empty() ){
		// get the first element of the list
		wai32_t	seqnb	= seqnb2ack_list.front();
		// delete it from the list
		seqnb2ack_list.erase(seqnb2ack_list.begin());
		// build the packet 
		pkt_t	pkt = build_ack(seqnb);
		// build the event
		rdgram_event_t	rdgram_event = rdgram_event_t::build_pkt_to_lower(&pkt);
		// notify the event
		bool to_keep = callback->neoip_rdgram_event_cb(userptr, *this, rdgram_event);
		if( !to_keep )	return false;
	}
	return true;
}


/** \brief callback called when the \ref zerotimer_t expire
 * 
 */
bool	rdgram_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// determine which process function must be called
	if( &cb_zerotimer == &zerotimer_maysend )	return zerotimer_maysend_process();
	else if(&cb_zerotimer == &zerotimer_seqnb2ack)	return zerotimer_seqnb2ack_process();
	
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       packet from lower/upper
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 */
rdgram_err_t	rdgram_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	// build the header
	serial_t	serial;
	serial << rdgram_pkttype_t(rdgram_pkttype_t::DATA_PKT);
	serial << next_xmit_seqnb;
	// prepend the header to the packet
	pkt.head_add( serial.get_data(), serial.get_len() );

	// if the sendbuf has not enougth room to accept this packet, return SENDBUF_FULL
	if( sendbuf_is_limited() && sendbuf_used_len + pkt.get_len() > sendbuf_max_len )
		return rdgram_err_t::SENDBUF_FULL;

	// create a sendbuf_pkt_t for it
	nipmem_new	sendbuf_pkt_t(pkt, next_xmit_seqnb, this );

	// update the next_xmit_seqnb
	next_xmit_seqnb++;

	return rdgram_err_t::OK;
}

/** \brief handle packet from the lower network layer
 * 
 * @param seqnb_out is the sequence number of the filtered packet returned by this function.
 *                  It is valid iif rdgram_err_t.succeed() and !pkt.is_null() after this function.
 */
rdgram_err_t	rdgram_t::pkt_from_lower(pkt_t &pkt, wai32_t &seqnb_out)	throw()
{
	rdgram_pkttype_t	pkttype;

	try {	// read the pkttype
		pkt >> pkttype;
	}catch(serial_except_t &e){
		return rdgram_err_t(rdgram_err_t::BOGUS_PKT, "Cant read the packet type due to " + e.what());
	}

	KLOG_DBG("received a pkttype=" << pkttype );
	switch( pkttype.get_value() ){
	case rdgram_pkttype_t::DATA_PKT:	return	recv_data_pkt(pkt, seqnb_out);
	case rdgram_pkttype_t::ACK_PKT:		return	recv_ack_pkt(pkt);
	default:				break;
	}
	return rdgram_err_t(rdgram_err_t::BOGUS_PKT, "Unknown packet type");
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    packet build
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief build a ACK_PKT
 */
pkt_t rdgram_t::build_ack(wai32_t seqnb)		throw()
{
	pkt_t	pkt;
	// build the packet
	pkt << rdgram_pkttype_t(rdgram_pkttype_t::ACK_PKT);
	pkt << seqnb;
	return pkt;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    packet recv
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle the reception of a DATA_PKT
 */
rdgram_err_t	rdgram_t::recv_data_pkt(pkt_t &pkt, wai32_t &seqnb_out)	throw()
{
	wai32_t	pkt_seqnb;
	// read the sequence number
	try {
		pkt >> pkt_seqnb ;
	}catch(serial_except_t &e){
		return rdgram_err_t(rdgram_err_t::BOGUS_PKT, "Cant read incoming packet seqnb due to " + e.what());
	}

	// ask the layer user if this packet must be acked or not
	bool	ack_this_pkt = callback->neoip_rdgram_should_acked_pkt_cb(userptr, *this, pkt, pkt_seqnb);
	// if this packet MUST NOT be acked, zero the packet and return OK now
	if( !ack_this_pkt ){
		pkt = pkt_t();
		return rdgram_err_t::OK;
	}

	// schedule a ack transmission
	// - the ack is sent thru a zerotimer to avoid notifying callback here
	seqnb2ack_list.push_back(pkt_seqnb);
	// trigger the zerotimer if not yet done
	if( zerotimer_seqnb2ack.size() == 0 )	zerotimer_seqnb2ack.append(this, NULL);

	// copy the seqnb
	seqnb_out = pkt_seqnb;

// TODO here handle duplicate in reception , see \ref rdgram_t comment about it

	return rdgram_err_t::OK;
}

/** \brief handle the reception of a DATA_PKT
 */
rdgram_err_t	rdgram_t::recv_ack_pkt(pkt_t &pkt)	throw()
{
	sendbuf_pkt_t *	sendbuf_pkt;
	wai32_t		pkt_seqnb;
	// read the sequence number
	try {
		pkt >> pkt_seqnb;
	}catch(serial_except_t &e){
		return rdgram_err_t(rdgram_err_t::BOGUS_PKT, "Cant read incoming packet seqnb due to " + e.what());
	}
	// nullify the packet to mark it as swallowed
	pkt = pkt_t();
	// try to find this sequence number pending in the sendbuf_pkt_db
	sendbuf_pkt = find_sendbuf_pkt(pkt_seqnb);
	// if no sendbuf_pkt_t is found in the sendbuf_pkt_db, discard the ack without error
	// - as it may simply be a duplicate of an already received ack
	if( sendbuf_pkt == NULL )	return rdgram_err_t::OK;
	// if iter has found the packet, it has been acknowledged, so delete it
	nipmem_delete	sendbuf_pkt;
	return rdgram_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         XMIT_PKT
// - retransmit the packet when the ack has not been received soon enough
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rdgram_t::sendbuf_pkt_t::sendbuf_pkt_t(const pkt_t &opkt, const wai32_t &seqnb, rdgram_t *rdgram)	throw()
			: seqnb(seqnb), pkt(opkt), rdgram(rdgram)
{
	// init the rxmit stuff
	rxmit_timer_policy = timer_expboff_t(delay_t::from_msec(500), delay_t::from_msec(2000));	
	// start the timer
	// - the initial sent is done by external function after the packet_from_upper() filter
	rxmit_timeout.start(rxmit_timer_policy.first(), this, NULL);
	// insert the sendbuf_pkt in the database
	bool	succeed = rdgram->sendbuf_pkt_db.insert(std::make_pair(seqnb, this)).second;
	DBG_ASSERT( succeed );
	// update the sendbuf
	rdgram->sendbuf_add_pkt(pkt.get_len());
}

/** \brief Destructor
 */
rdgram_t::sendbuf_pkt_t::~sendbuf_pkt_t()						throw()
{
	// update the sendbuf
	rdgram->sendbuf_del_pkt(pkt.get_len());
	// remove it from the sendbuf_pkt database
	rdgram->sendbuf_pkt_db.erase(seqnb);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         rxmit_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	rdgram_t::sendbuf_pkt_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// a copy is needed as the notified packet will be modified, and a clean pkt
	// is required in case it needs to be retransmited.
	pkt_t	pkt_copy	= pkt;
	// send the packet thru an event
	rdgram_event_t	rdgram_event = rdgram_event_t::build_pkt_to_lower(&pkt_copy);
	bool to_keep = rdgram->callback->neoip_rdgram_event_cb(rdgram->userptr, *rdgram, rdgram_event);
	if( !to_keep )	return false;
	// init the new delay
	rxmit_timeout.change_period( rxmit_timer_policy.next() );
	return true;
}


NEOIP_NAMESPACE_END






