/*! \file
    \brief Definition of the nlay_closure_t

\par Brief Description
\ref nlay_closure_t implements a 3 packets closure (similar to tcp one).
It is usefull only on unreliable connection (reliable connection like tcp 
have there own closure mechanism). Additionnaly it allows an immediat closure
in case of emergency (e.g. an apps quitting)


\par Implementation Note
- this brick is coded directly in nlay as it is closely related to it and
  can't be used elsewhere.
- in case of imminent destruction (aka like an apps quitting), notify_imminent_destroy()
  MUST be called and send a CLOSE_NOW packets
  - on reception, a CLOSE_NOW closes immediatly and completly the connection
    with no ack required or anything.
  - if this packet is lost by the network, the remote peer is not aware the connection
    has been locally closed, so mechanism like reachpeer may detects this case after
    a timeout.
- it doesnt do anything until one the 2 following events occurs
  - either a CLOSE_REQUEST is received and the local closure is acting as responder
    - then a nlay_event_t::CNX_CLOSING is notified
    - and CLOSE_REPLY are retransmitted until
      (i) a CLOSE_REPLY_ACK is received and then a nlay_event_t::CNX_DESTROYED is notified
      or
      (ii) the timer policy is finished and the nlay_event_t::CNX_DESTROYED is notified 
           even if it isnt acknowledged
  - or start_closure() is called without any CLOSE_REQUEST previously received
    - so the local closure is acting as a initiator
    - and CLOSE_REQUEST are retransmited until:
    - if a CLOSE_REQUEST is received in this state, handle it as a CLOSE_REPLY
      - it is a simultaneous close initiation (similar to 'double open' in tcp)
    - if a CLOSE_REPLY is received:
      - a CLOSE_REPLY_ACK is replyed and a nlay_event_t::CNX_DESTROYED is notified
    - the timer policy is finished and the nlay_event_t::CNX_DESTROYED is notified 
      even if it isnt acknowledged
  
\par Algorithm Description
  
 INITIATOR                                                 RESPONDER

- start timeout with close_itor_delay
- start rxmit CLOSE_REQUEST every PROBE_PERIOD
  until the timeout expire or a CLOSE_REPLY
  is received.
  - both event occurance will cause the 
    socket to be considered fully close
      
                                     CLOSE_REQUEST
                                --------------------->  
                                                 - start timeout with close_resp_delay
                                                 - start rxmit CLOSE_REPLY every PROBE_PERIOD 
                                                   - until timeout expire or a CLOSE_REPLY_ACK is received
                                                   - both events occurance will cause the 
                                                     socket to be considered fully close
                                     CLOSE_REPLY
                                <--------------------
- send a CLOSE_REPLY_ACK            
- the connection is considered fully closed
            
                                    CLOSE_REPLY_ACK
                                -------------------->
                                                  - the connection is considered fully closed
              

*/

/* system include */
/* local include */
#include "neoip_nlay_closure.hpp"
#include "neoip_nlay_closure_pkttype.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref closure_t constant
#if 0
	const delay_t	nlay_closure_t::CLOSE_ITOR_DELAY_DFL	= delay_t::from_sec(30);
	const delay_t	nlay_closure_t::CLOSE_RESP_DELAY_DFL	= delay_t::from_sec(30);
	const delay_t	nlay_closure_t::PROBE_PERIOD_DFL	= delay_t::from_sec(5);
#else
	// value for debug
	const delay_t	nlay_closure_t::CLOSE_ITOR_DELAY_DFL	= delay_t::from_sec(10);
	const delay_t	nlay_closure_t::CLOSE_RESP_DELAY_DFL	= delay_t::from_sec(5);
	const delay_t	nlay_closure_t::PROBE_PERIOD_DFL	= delay_t::from_sec(1);
#endif
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_closure_t::nlay_closure_t()	throw()
{
	state	= NONE;
}

/** \brief Destructor
 */
nlay_closure_t::~nlay_closure_t()	throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   start_closure
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the closure
 * 
 * - to be called when the closure must start no matter the state
 */
void	nlay_closure_t::start_closure()					throw()
{
	// if the state is already RESP, do nothing
	// - the closure has been already started when the CLOSE_REQUEST has been received
	if( state == RESP )	return;
	
	// if the state is already COMPLETED, notify a CNX_DESTROY and exit
	// - the closure has been already COMPLETED when the CLOSE_NOW has been received
	if( state == COMPLETED ){
		// the cnx_destroyed here is sent async to avoid reccursive callback notification
		cnx_destroyed_zerotimer.append(this, NULL);
		return;
	}

	// sanity check - the state MUST be NONE
	DBG_ASSERT( state == NONE );
	
	// become ITOR in the closure
	state = ITOR;

	KLOG_DBG("Start the closure as ITOR");

	// init the rxmit timer
	rxmit_timer_policy	= timer_probing_t(delay_t(0), close_itor_delay, probe_period);
	rxmit_timeout.start(rxmit_timer_policy.first(), this, NULL);
}

/** \brief to notify to nlay_closure_t that the stack will be imminantly destroyed
 * 
 * - if the closure isn't yet acknowledged or even started, send a CLOSE_REQUEST immediatly 
 * - NOTE: this function sends a synchronous callback - to use with care
 */
void	nlay_closure_t::notify_imminent_destroy()	throw()
{
	// log to debug
	KLOG_DBG("imminent destroy in closure");
	// if the closure is already completed, do nothing
	if( state == COMPLETED )		return;
	// log to debug	
	KLOG_DBG("send CLOSE_NOW");
	// send the packet CLOSE_NOW to the lower layer
	pkt_t	pkt	= build_close_now();
	get_nlay_full()->notify_pkt_to_lower(pkt, get_full_api_lower());
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          nlay_full_api_t functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */        
void	nlay_closure_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{	
	nlay_closure_pkttype_t	pkttype;
	// register a packet type
	pkttype	= nlay_closure_pkttype_t::CLOSE_REQUEST;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
	// register a packet type
	pkttype	= nlay_closure_pkttype_t::CLOSE_REPLY;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
	// register a packet type
	pkttype	= nlay_closure_pkttype_t::CLOSE_REPLY_ACK;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
	// register a packet type
	pkttype	= nlay_closure_pkttype_t::CLOSE_NOW;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
}

/** \brief Set the parameter for this brick from the profile or the negociation result
 */
nlay_err_t	nlay_closure_t::set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result)	throw()
{
	// check the profile
	nlay_err_t	err = nlay_profile->closure().check();
	if( err.failed() )	return err;

	// set the parameter from the profile
	close_itor_delay	= nlay_profile->closure().close_itor_delay();
	close_resp_delay	= nlay_profile->closure().close_resp_delay();
	probe_period		= nlay_profile->closure().probe_period();
	return nlay_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  pkt filtering from lower/upper layer
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the lower network layer
 * 
 * - queue the received packet in a zerotimer queue to ease the event notification
 */
nlay_err_t	nlay_closure_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	KLOG_DBG("enter pkt=" << pkt);
	// put the packet in the packet to parse database
	pkt_parse_db.push_back(pkt);
	// if the packet to parse zerotimer is not running, trigger it
	if( pkt_parse_zerotimer.size() == 0 )	pkt_parse_zerotimer.append(this, NULL );
	// swallow the packet
	pkt = pkt_t();
	return nlay_err_t::OK;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  rxmit_timeout expiration
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	nlay_closure_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)			throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	pkt_t		pkt;
	KLOG_DBG("enter");

	// sanity check - the state MUST be set
	DBG_ASSERT( state == ITOR || state == RESP );

	// build a packet according to state
	if( state == ITOR )	pkt	= build_close_request();
	else			pkt	= build_close_reply();
	
	// send the packet to the lower layer
	bool	tokeep = nlay_full->notify_pkt_to_lower(pkt, get_full_api_lower());
	if( !tokeep )	return false;

	// goto the next timer
	rxmit_timer_policy.next();
	
	// if the timer policy is finished the closure is considered completed, even if not acknowledged
	if( rxmit_timer_policy.is_finished() ){
		// stop the rxmit_timeout
		rxmit_timeout.stop();
		// change the state into COMPLETED
		state = COMPLETED;
		// notify a CNX_DESTROYED
		return nlay_full->notify_event(nlay_event_t::build_cnx_destroyed("Connection closure timedout. aborting."));
	}

	// reinit the timeout period
	rxmit_timeout.change_period( rxmit_timer_policy.current() );
	// keep the timeout
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       pkt_parse_zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	nlay_closure_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	if( &pkt_parse_zerotimer == &cb_zerotimer )		return pkt_parse_zerotimer_cb();
	else if( &cnx_destroyed_zerotimer == &cb_zerotimer )	return cnx_destroyed_zerotimer_cb();
	
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return true;
}

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - the cnx_destroyed here is sent async to avoid reccursive callback notification
 */
bool	nlay_closure_t::cnx_destroyed_zerotimer_cb()	throw()
{
	return get_nlay_full()->notify_event(nlay_event_t::build_cnx_destroyed(""));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  pkt filtering from lower/upper layer
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	nlay_closure_t::pkt_parse_zerotimer_cb()	throw()
{
	KLOG_DBG("enter");
	// loop until there are no more packet to parse
	while( pkt_parse_db.size() ){
		// copy the first packet to parse
		pkt_t	pkt = pkt_parse_db.front();
		// remove it from the database
		pkt_parse_db.erase(pkt_parse_db.begin());
		// parse it
		bool tokeep = pkt_parse(pkt);
		if( !tokeep )	return false;
	}
	return true;
}

/** \brief handle packet from the lower network layer
 */
bool	nlay_closure_t::pkt_parse(pkt_t &pkt)		throw()
{
	KLOG_DBG("enter pkt=" << pkt);

	try {
		nlay_closure_pkttype_t	pkttype;
		// read the pkttype
		pkt >> pkttype;
		KLOG_DBG("received a pkttype=" << pkttype );
		
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case nlay_closure_pkttype_t::CLOSE_REQUEST:	return recv_close_request(pkt);
		case nlay_closure_pkttype_t::CLOSE_REPLY:	return recv_close_reply(pkt);
		case nlay_closure_pkttype_t::CLOSE_REPLY_ACK:	return recv_close_reply_ack(pkt);
		case nlay_closure_pkttype_t::CLOSE_NOW:		return recv_close_now(pkt);
		default:	break;
		}
	} catch(serial_except_t &e) {
		KLOG_DBG("error=" << nlay_err_t(nlay_err_t::BOGUS_PKT, "Cant parse packet due to " + e.what()));
	}
	// NOTE: this point MUST NEVER be reached
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Handle the reception of a CLOSE_REQUEST
 */
bool	nlay_closure_t::recv_close_request(pkt_t &pkt)			throw(serial_except_t)
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	KLOG_DBG("enter");
	// if the state is RESP, do nothing
	if( state == RESP )	return true;
	
	// if the state is ITOR, handle the CLOSE_REQUEST as a CLOSE_REPLY
	// - this handle the case of synchronous close initiation (similar to 'double open' in tcp)
	if( state == ITOR )	return recv_close_reply(pkt);

	// sanity check - here the state MUST be NONE
	DBG_ASSERT( state == NONE );
	
	// NOTE: here the closure has been initiated by the other peer, start assuming the resp state
	state = RESP;
	KLOG_DBG("Start the closure as RESP");

	// Notify a nlay_event_t::CNX_CLOSING
	bool tokeep = nlay_full->notify_event(nlay_event_t::build_cnx_closing("Closed by remote peer"));
	if( !tokeep )	return false;

	// init the rxmit timer
	rxmit_timer_policy	= timer_probing_t(delay_t(0), close_resp_delay, probe_period);
	rxmit_timeout.start(rxmit_timer_policy.first(), this, NULL);
	
	// return 'tokeep'
	return true;
}

/** \brief Handle the reception of a CLOSE_REPLY
 */
bool	nlay_closure_t::recv_close_reply(pkt_t &pkt)				throw(serial_except_t)
{
	nlay_full_t *	nlay_full	= get_nlay_full();	
	KLOG_DBG("enter");
	// if the state is NOT ITOR, do nothing
	if( state != ITOR )	return true;

	// pass the state in COMPLETED
	state = COMPLETED;

	// send a CLOSE_REPLY_ACK to the lower layer
	pkt_t	pkt_reply	= build_close_reply_ack();
	bool	tokeep = nlay_full->notify_pkt_to_lower(pkt_reply, get_full_api_lower());
	if( !tokeep )	return false;

	// notify a nlay_event_t::CNX_DESTROYED
	return nlay_full->notify_event(nlay_event_t::build_cnx_destroyed("Connection closure acked by remote peer"));	
}

/** \brief Handle the reception of a CLOSE_REPLY_ACK
 */
bool	nlay_closure_t::recv_close_reply_ack(pkt_t &pkt)			throw(serial_except_t)
{
	KLOG_DBG("enter");
	// if the state is NOT RESP, do nothing
	if( state != RESP )	return true;
	// pass the state in COMPLETED
	state = COMPLETED;
	// notify a nlay_event_t::CNX_DESTROYED
	return get_nlay_full()->notify_event(nlay_event_t::build_cnx_destroyed("Connection closure acked by remote peer"));
}


/** \brief Handle the reception of a CLOSE_NOW
 * 
 * - so the other peer warns the local peer that the socket is completely unusable
 *   from now on, even for the closure protocol
 *   - so the closure is declared COMPLETED whatever the previous state
 */
bool	nlay_closure_t::recv_close_now(pkt_t &pkt)				throw(serial_except_t)
{
	KLOG_DBG("enter");
	// pass the state in COMPLETED immediatly as it is a CLOSE_NOW
	state = COMPLETED;
	// stop the rxmit_timeout if needed
	if( rxmit_timeout.is_running() )	rxmit_timeout.stop();
	// notify a nlay_event_t::CNX_CLOSING
	return get_nlay_full()->notify_event(nlay_event_t::build_cnx_closing("Connection reset by remote peer"));
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the CLOSE_REQUEST
 */
pkt_t nlay_closure_t::build_close_request()	throw()
{
	KLOG_DBG("enter");
	pkt_t	pkt;
	pkt << nlay_closure_pkttype_t(nlay_closure_pkttype_t::CLOSE_REQUEST);
	return pkt;
}

/** \brief build the CLOSE_REPLY
 */
pkt_t nlay_closure_t::build_close_reply()	throw()
{
	KLOG_DBG("enter");
	pkt_t	pkt;
	pkt << nlay_closure_pkttype_t(nlay_closure_pkttype_t::CLOSE_REPLY);
	return pkt;
}

/** \brief build the CLOSE_REPLY_ACK
 */
pkt_t nlay_closure_t::build_close_reply_ack()	throw()
{
	KLOG_DBG("enter");
	pkt_t	pkt;
	pkt << nlay_closure_pkttype_t(nlay_closure_pkttype_t::CLOSE_REPLY_ACK);
	return pkt;
}

/** \brief build the CLOSE_NOW
 */
pkt_t nlay_closure_t::build_close_now()	throw()
{
	KLOG_DBG("enter");
	pkt_t	pkt;
	pkt << nlay_closure_pkttype_t(nlay_closure_pkttype_t::CLOSE_NOW);
	return pkt;
}


NEOIP_NAMESPACE_END


