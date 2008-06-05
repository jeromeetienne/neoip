/*! \file
    \brief Header of the \ref bt_swarm_profile_t

*/


#ifndef __NEOIP_BT_SWARM_PROFILE_HPP__ 
#define __NEOIP_BT_SWARM_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_bt_swarm_sched_profile.hpp"
#include "neoip_bt_swarm_full_prec_profile.hpp"
#include "neoip_bt_pselect_policy.hpp"
#include "neoip_bt_jamrc4_type.hpp"
#include "neoip_delay.hpp"
#include "neoip_rate_estim_arg.hpp"
#include "neoip_rate_limit_arg.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the bt_swarm_t
 */
class bt_swarm_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default delay a given bt_swarm_full_t before sending keepalive if no other cmd has been sent
	static const delay_t			KEEPALIVE_PERIOD;
	//! the default delay a given bt_swarm_full_t may remain idle
	static const delay_t			FULL_IDLE_TIMEOUT;
	//! the rate_estim_t argument for the recv_rate of bt_swarm_full_t
	static const rate_estim_arg_t		FULL_RATE_ESTIM_ARG;
	//! the delay_t before a bt_swarm_itor_t is declared as timedout
	static const delay_t			ITOR_TIMEOUT_DELAY;
	//! the delay_t before trying another bt_swarm_itor_t when one already failed
	static const delay_t			ITOR_BLACKLIST_DELAY;
	//! the default bt_jamrc4_type_t to use for bt_jamrc4_itor_t
	static const bt_jamrc4_type_t		ITOR_JAMRC4_TYPE;
	//! the default maximum length for the BLOCK_REQ originated by the local peer
	static const size_t			XMIT_REQ_MAXLEN;
	//! the default maximum length for the BLOCK_REQ received by the local peer
	static const size_t			RECV_REQ_MAXLEN;
	//! the maximum length of a recved bt command, in relation with the RECV_REQ_MAXLEN
	//! - it avoid a DOS in which an attacker would use all the ram by sending illegal large commands.
	static const size_t			RECV_CMD_MAXLEN;
	//! if true, dont send PIECE_ISAVAIL to remote peer which already have this piece
	static const bool			NO_REDUNDANT_HAVE;
	//! if the number of bt_swarm_full_t is >= to NO_NEW_FULL_LIMIT, no new one will be created
	static const size_t			NO_NEW_FULL_LIMIT;
	//! if the number of bt_swarm_itor_t is >= to NO_NEW_ITOR_LIMIT, no new one will be created
	static const size_t			NO_NEW_ITOR_LIMIT;
	// the bt_pselect_policty_t to use for this bt_swarm
	static const bt_pselect_policy_t	PSELECT_POLICY;
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_profile_t()	throw();
	~bt_swarm_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_swarm_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	// TODO field related to bt_swarm_full_t - to put in a separate profile
	PROFILE_VAR_PLAIN( delay_t			, keepalive_period);
	PROFILE_VAR_PLAIN( delay_t			, full_idle_timeout);
	PROFILE_VAR_SPROF( rate_estim_arg_t		, full_rate_estim_arg);	
	PROFILE_VAR_SPROF( bt_swarm_full_prec_profile_t	, full_prec);
	PROFILE_VAR_SPROF( rate_limit_arg_t		, xmit_limit_arg);
	PROFILE_VAR_SPROF( rate_limit_arg_t		, recv_limit_arg);

	PROFILE_VAR_PLAIN( delay_t			, itor_timeout_delay	);
	PROFILE_VAR_PLAIN( delay_t			, itor_blacklist_delay	);
	PROFILE_VAR_PLAIN( bt_jamrc4_type_t		, itor_jamrc4_type	);
	PROFILE_VAR_PLAIN( size_t			, xmit_req_maxlen);
	PROFILE_VAR_PLAIN( size_t			, recv_req_maxlen);
	PROFILE_VAR_PLAIN( size_t			, recv_cmd_maxlen);
	PROFILE_VAR_PLAIN( bool				, no_redundant_have);
	PROFILE_VAR_PLAIN( size_t			, no_new_full_limit);
	PROFILE_VAR_PLAIN( size_t			, no_new_itor_limit);
	PROFILE_VAR_PLAIN( bt_pselect_policy_t		, pselect_policy);
	PROFILE_VAR_SPROF( bt_swarm_sched_profile_t	, sched);
	
	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_PROFILE_HPP__  */



