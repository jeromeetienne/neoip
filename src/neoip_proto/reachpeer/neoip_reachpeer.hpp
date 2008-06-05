/*! \file
    \brief Header of the \ref reachpeer_t

- see \ref neoip_reachpeer_t.cpp
*/


#ifndef __NEOIP_REACHPEER_HPP__ 
#define __NEOIP_REACHPEER_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_reachpeer_cb.hpp"
#include "neoip_reachpeer_event.hpp"
#include "neoip_reachpeer_err.hpp"
#include "neoip_reachpeer_profile.hpp"
#include "neoip_timer_probing.hpp"
#include "neoip_timeout.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief A layer to allow to test the reachability of the other peer
 * 
 */
class reachpeer_t : public zerotimer_cb_t, public timeout_cb_t, NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default amount of time before starting sending probe
	static const delay_t	SILENT_DELAY_DFL;
	//! the default amount of time before considering the other peer as unreachable
	static const delay_t	UNREACH_DELAY_DFL;
	//! the default amount of time between 2 probes
	static const delay_t	PROBE_PERIOD_DFL;
private:
	// parameter
	delay_t			silent_delay;	//!< delay to wait before starting probing
	delay_t			unreach_delay;	//!< delay to wait before declaring the remote peer unreachable
	delay_t			probe_period;	//!< the delay between probe to send during the probing period.

	// internal function to handle packet processing
	pkt_t			build_echo_request()				throw();
	pkt_t			build_echo_reply()				throw();
	reachpeer_err_t		recv_echo_request(pkt_t &pkt)			throw(serial_except_t);
	reachpeer_err_t		recv_echo_reply(pkt_t &pkt)			throw(serial_except_t);
	
	zerotimer_t		zerotimer_reply;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)
										throw();	
	
	bool			reliable_outter_cnx;	//!< true if the outter connection is reliable.
							//!< false otherwise.
	// timer handling
	timeout_t		rxmit_timeout;		//!< retransmition timer to rxmit
	timer_probing_t		rxmit_timer_policy;	//!< the timer policy for the retransmission
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();	
	
	// callback stuff
	reachpeer_cb_t *	callback;		//!< the callback to notify events
	void *			userptr;		//!< the userptr associated with the above callback
public:
	// ctor/dtor
	reachpeer_t()		throw();
	~reachpeer_t()		throw();
	
	// initialization functions
	reachpeer_err_t		set_callback(reachpeer_cb_t * callback, void *userptr )	throw();
	reachpeer_err_t		set_reliable_outter_cnx(bool value=true)		throw();
	reachpeer_err_t		set_from_profile(const reachpeer_profile_t &profile)	throw();
	reachpeer_err_t		start()							throw();
	bool			is_running()						throw();
	void			stop()							throw();

	// helper function for start()
	reachpeer_err_t		start(reachpeer_cb_t *callback, void *userptr)	throw();
		
	// packet processing
	void			recv_proof()				throw();
	reachpeer_err_t		pkt_from_lower(pkt_t &pkt)		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_REACHPEER_HPP__  */



