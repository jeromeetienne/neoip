/*! \file
    \brief Header of the \ref bt_swarm_sched_profile_t

*/


#ifndef __NEOIP_BT_SWARM_SCHED_PROFILE_HPP__ 
#define __NEOIP_BT_SWARM_SCHED_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the bt_swarm_sched_t
 */
class bt_swarm_sched_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! delay beforebt_swarm_sched_request_t before timeing out when in normal mode (aka not endgame)
	static const delay_t	REQUEST_TIMEOUT_NORMAL;
	//! delay beforebt_swarm_sched_request_t before timeing out when in endgame
	static const delay_t	REQUEST_TIMEOUT_ENDGAME;
	//! the delay for which request are queued
	//! - it is a manual and fixed estimation of the latency between the 2 peers. 
	//! - tcp doesnt allow to measure the latency and it is not possible in the bt protocol
	//! - additionnaly sending a BLOCK_REQ is not possible if a BLOCK_REP is currently
	//!   being sent, increasing the 'latency' even more
	static const delay_t	REQ_QUEUE_DELAY;
	//! the minimum length of the request queue
	//! - it should be >= to bt_swarm_profile_t::xmit_req_maxlen()
	static const size_t	REQ_QUEUE_MINLEN;
	//! the maximum length of the request queue
	static const size_t	REQ_QUEUE_MAXLEN;
	//! if seq_pselect_ifequ is set, pselect will try to select pieceidx in sequence
	//! if if doesnt affect the 'importance' of the piece selected (pieceprec + remote_pwish)
	static const bool	SEQ_PSELECT_IFEQU;
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_sched_profile_t()	throw();
	~bt_swarm_sched_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_swarm_sched_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, request_timeout_normal);
	PROFILE_VAR_PLAIN( delay_t	, request_timeout_endgame);
	PROFILE_VAR_PLAIN( delay_t	, req_queue_delay);
	PROFILE_VAR_PLAIN( size_t	, req_queue_minlen);
	PROFILE_VAR_PLAIN( size_t	, req_queue_maxlen);
	PROFILE_VAR_PLAIN( bool		, seq_pselect_ifequ);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_SCHED_PROFILE_HPP__  */



