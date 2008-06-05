/*! \file
    \brief Definition of the \ref reachpeer_t

\par Subtle interaction with the MTU
  - if the MTU is large enougth to let the probe go thru, but not large enougth
    to let the data packet go thru, the other peer is still declared reachable.
 
*/


/* system include */
/* local include */
#include "neoip_reachpeer.hpp"
#include "neoip_reachpeer_pkttype.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref reachpeer_t constant
#if 0
	const delay_t	reachpeer_t::SILENT_DELAY_DFL	= delay_t::from_sec(120);
	const delay_t	reachpeer_t::UNREACH_DELAY_DFL	= delay_t::from_sec(180);
	const delay_t	reachpeer_t::PROBE_PERIOD_DFL	= delay_t::from_sec(10);
#else
	// value for debug
	const delay_t	reachpeer_t::SILENT_DELAY_DFL	= delay_t::from_sec(5);
	const delay_t	reachpeer_t::UNREACH_DELAY_DFL	= delay_t::from_sec(10);
	const delay_t	reachpeer_t::PROBE_PERIOD_DFL	= delay_t::from_sec(1);
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
reachpeer_t::reachpeer_t()	throw()
{
	// set the default parameter
	silent_delay	= SILENT_DELAY_DFL;
	unreach_delay	= UNREACH_DELAY_DFL;
	probe_period	= PROBE_PERIOD_DFL;
	
	// set the default for reliable_outter_cnx 
	// - false is a good default because:
	//   - acting as unreliable on a reliable connection is inefficient but works
	//   - acting as reliable on a unreliable connection doesnt work
	reliable_outter_cnx	= false;	
}

/** \brief Destructor
 */
reachpeer_t::~reachpeer_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        set parameters
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief set the event callback
 */
reachpeer_err_t reachpeer_t::set_callback(reachpeer_cb_t * callback, void *userptr)	throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
	return reachpeer_err_t::OK;
}

/** \brief set the parameters
 */
reachpeer_err_t reachpeer_t::set_from_profile(const reachpeer_profile_t &profile)	throw()
{
	// check the profile
	reachpeer_err_t	err = profile.check();
	
	if( err.failed() )	return err;
	// copy the values
	silent_delay	= profile.silent_delay();
	unreach_delay	= profile.unreach_delay();
	probe_period	= profile.probe_period();
	return reachpeer_err_t::OK;
}

/** \brief define if the outter connection is reliable or not
 */
reachpeer_err_t reachpeer_t::set_reliable_outter_cnx( bool value )		throw()
{
	reliable_outter_cnx	= value;
	return reachpeer_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       start() functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
reachpeer_err_t	reachpeer_t::start()		throw()
{
	// init the timer_policy
	rxmit_timer_policy	= timer_probing_t(silent_delay, unreach_delay, probe_period);
	// init the rxmit stuff 
	// - with a 0 delay to test reachabitlity immediatly after the connection establishement 
	rxmit_timeout.start(delay_t::from_sec(0), this, NULL);
	// return no error
	return reachpeer_err_t::OK;
}

/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
reachpeer_err_t	reachpeer_t::start(reachpeer_cb_t *callback, void *userptr)	throw()
{
	reachpeer_err_t	reachpeer_err;
	// set the callback
	reachpeer_err = set_callback(callback, userptr);
	if( reachpeer_err.failed() )	return reachpeer_err;
	// start the action
	return start();
}

/** \brief return true if the reachpeer is running, false otherwise
 */
bool	reachpeer_t::is_running()						throw()
{
	return rxmit_timeout.is_running();
}

/** \brief Stop the actions
 */
void	reachpeer_t::stop()				throw()
{
	rxmit_timeout.stop();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        recv_proof
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle the reception of a reachpeer proof
 * 
 * - This function may be called when any authenticated remote packet is received.
 *   - this includes a received ECHO_REPLY
 */
void	reachpeer_t::recv_proof()	throw()
{
	// log to debug
	KLOG_DBG("called receive reachpeer proof");
	// if reachpeer is not running, do nothing
	// - this happen when the reachpeer has been already stop
	if( !is_running() )	return;
	// reinit the timer
	rxmit_timer_policy.reset();
	rxmit_timeout.start(rxmit_timer_policy.first(), this, NULL);
	
	// log to debug
	KLOG_DBG("Receive reachpeer proof. relaunch timer in " << rxmit_timer_policy.first());
	KLOG_DBG("timeout is runnning= " << rxmit_timeout.is_running() );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  timeout expiration to send ECHO_REQUEST
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	reachpeer_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	reachpeer_event_t	reachpeer_event;

	// if the outter connection is reliable, stop the timer
	if(reliable_outter_cnx){
		rxmit_timeout.stop();
	}else{	// if the outter connection is unreliable, init the new delay
		rxmit_timeout.change_period( rxmit_timer_policy.next() );
	}

	// if the rxmit_timer is finished, the other peer is considered unreachable
	if( rxmit_timer_policy.is_finished() ){
		reachpeer_event = reachpeer_event_t::build_peer_unreach("Remote peer unreachable");
		return callback->neoip_reachpeer_event_cb(userptr, *this, reachpeer_event);
	}
	// log to debug
	KLOG_DBG("send a reachpeer request");
	// build the probe
	pkt_t	pkt = build_echo_request();
	// notify an event to send a probe
	reachpeer_event = reachpeer_event_t::build_pkt_to_lower(&pkt);
	return callback->neoip_reachpeer_event_cb(userptr, *this, reachpeer_event);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer_reply callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - send all the ECHO_REPLY to nlay_full->notify_pkt_to_lower()
 */
bool	reachpeer_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("Send a reachpeer reply");
	// build the ECHO_REPLY
	pkt_t	reply_pkt = build_echo_reply();
	// build the event
	reachpeer_event_t reachpeer_event = reachpeer_event_t::build_pkt_to_lower(&reply_pkt);
	// notify the event
	return callback->neoip_reachpeer_event_cb(userptr, *this, reachpeer_event);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          process incoming packet
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief handle packet from the lower network layer
 */
reachpeer_err_t reachpeer_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	KLOG_DBG("enter pkt=" << pkt);
	KLOG_DBG("**********************************");

	try {
		reachpeer_pkttype_t	pkttype;
		// read the pkttype
		pkt >> pkttype;
		KLOG_DBG("received a pkttype=" << pkttype );
		
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case reachpeer_pkttype_t::ECHO_REQUEST:		return recv_echo_request(pkt);
		case reachpeer_pkttype_t::ECHO_REPLY:		return recv_echo_reply(pkt);
		default:	break;
		}
	} catch(serial_except_t &e) {
		return reachpeer_err_t(reachpeer_err_t::BOGUS_PKT, "Cant parse packet due to " + e.what());
	}
	// NOTE: if this point is reached, the pkttype is unknown
	return reachpeer_err_t(reachpeer_err_t::BOGUS_PKT, "Unknown packet type");
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Handle the reception of a ECHO_REQUEST
 */
reachpeer_err_t	reachpeer_t::recv_echo_request(pkt_t &pkt)			throw(serial_except_t)
{
	// log to debug
	KLOG_DBG("recv a reachpeer request");
	// swallow the packet
	pkt	= pkt_t();
	// queue a zerotimer event to send a reply
	zerotimer_reply.append(this, NULL);
	// return no error
	return reachpeer_err_t::OK;
}

/** \brief Handle the reception of a ECHO_REPLY
 */
reachpeer_err_t	reachpeer_t::recv_echo_reply(pkt_t &pkt)			throw(serial_except_t)
{
	// log to debug
	KLOG_DBG("recv a reachpeer reply");
	// swallow the packet
	pkt	= pkt_t();
	// as the ECHO_REPLY has been received, the remote peer is assumed reachable
	recv_proof();
	return reachpeer_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         packet build
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief build the ECHO_REQUEST
 */
pkt_t reachpeer_t::build_echo_request()	throw()
{
	pkt_t	pkt;
	pkt << reachpeer_pkttype_t(reachpeer_pkttype_t::ECHO_REQUEST);
	return pkt;
}

/** \brief build the ECHO_REPLY
 */
pkt_t reachpeer_t::build_echo_reply()	throw()
{
	pkt_t	pkt;
	pkt << reachpeer_pkttype_t(reachpeer_pkttype_t::ECHO_REPLY);
	return pkt;
}

NEOIP_NAMESPACE_END






