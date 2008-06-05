/*! \file
    \brief Header of the nlay_closure_t

*/


#ifndef __NEOIP_NLAY_CLOSURE_HPP__
#define __NEOIP_NLAY_CLOSURE_HPP__

/* system include */
/* local include */
#include "neoip_delay.hpp"
#include "neoip_timer_probing.hpp"
#include "neoip_timeout.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief nlay glue for \ref closure_t
 */
class nlay_closure_t : public zerotimer_cb_t, public timeout_cb_t, public nlay_full_api_t {
public:	/////////////////////// constant declaration ///////////////////////////
	//! default amount of time before destroying a socket without receiving CLOSE_REPLY
	static const delay_t	CLOSE_ITOR_DELAY_DFL;
	//! default amount of time before destroying a socket without receiving CLOSE_REPLYACK
	static const delay_t	CLOSE_RESP_DELAY_DFL;
	//! default amount of time between 2 probes transmition (probe may be CLOSE_REPLY or CLOSE_REQUEST)
	static const delay_t	PROBE_PERIOD_DFL;
private:
	//! the list of state used by initiator
	enum state_t {
		NONE,
		ITOR,		//!< when the closure is locally initiated
		RESP,		//!< when the closure is remotly initiated
		COMPLETED,	//!< when the closure has been acknowledged by the other peer
		MAX
	};
	
	delay_t			close_itor_delay;
	delay_t			close_resp_delay;
	delay_t			probe_period;
	timeout_t		rxmit_timeout;		//!< retransmition timer to rxmit
	timer_probing_t		rxmit_timer_policy;	//!< the timer policy for the retransmission
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();	

	state_t			state;

	std::list<pkt_t>	pkt_parse_db;
	bool			pkt_parse(pkt_t &pkt)				throw();
	zerotimer_t		pkt_parse_zerotimer;
	bool			pkt_parse_zerotimer_cb()			throw();
	
	zerotimer_t		cnx_destroyed_zerotimer;
	bool			cnx_destroyed_zerotimer_cb()			throw();
	
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)
										throw();	
	// packet reception
	bool			recv_close_request(pkt_t &pkt)			throw(serial_except_t);
	bool			recv_close_reply(pkt_t &pkt)			throw(serial_except_t);
	bool			recv_close_reply_ack(pkt_t &pkt)		throw(serial_except_t);
	bool			recv_close_now(pkt_t &pkt)			throw(serial_except_t);
	// packet building
	pkt_t			build_close_request()		throw();
	pkt_t			build_close_reply()		throw();
	pkt_t			build_close_reply_ack()		throw();
	pkt_t			build_close_now()		throw();
public:
	// ctor/dtor
	nlay_closure_t()	throw();
	~nlay_closure_t()	throw();
	
	
	void	start_closure()			throw();
	void	notify_imminent_destroy()	throw();
	
	// function inherited from nlay_full_api_t
	void		register_handler(nlay_regpkt_t *regpkt_lower)	throw();
	nlay_err_t	set_parameter(const nlay_profile_t *nlay_profile
				, const nlay_nego_result_t *nego_result)	throw();	
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_closure_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_CLOSURE_HPP__ 


