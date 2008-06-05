/*! \file
    \brief Definition of the \ref bt_swarm_profile_t

- TODO this is rather dirty
  - many fields are not directly related to bt_swarm_t but to subpart
  - would be better to get a sub profile instead then direct access here
  - sort this out
  - as an example, many fields are related to the bt_swarm_full_t
  - there are some 'protocol limit' too. those are unlikely to be changed
    - where should i put them ?


*/


/* system include */
/* local include */
#include "neoip_bt_swarm_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_swarm_profile_t constant
#if 0
	const delay_t		bt_swarm_profile_t::KEEPALIVE_PERIOD	= delay_t::from_sec(2*60);
	const delay_t		bt_swarm_profile_t::FULL_IDLE_TIMEOUT	= delay_t::from_sec(4*60);
	const rate_estim_arg_t	bt_swarm_profile_t::FULL_RATE_ESTIM_ARG	= rate_estim_arg_t()
										.total_delay(delay_t::from_sec(20))
										.slice_delay(delay_t::from_sec(2));	
	const delay_t		bt_swarm_profile_t::ITOR_TIMEOUT_DELAY	= delay_t::from_sec(60);
	const delay_t		bt_swarm_profile_t::ITOR_BLACKLIST_DELAY= delay_t::from_min(60);
	const bt_jamrc4_type_t	bt_swarm_profile_t::ITOR_JAMRC4_TYPE	= strtype_bt_jamrc4_type_t::NOJAM;
	const size_t		bt_swarm_profile_t::XMIT_REQ_MAXLEN	= 16*1024;
	const size_t		bt_swarm_profile_t::RECV_REQ_MAXLEN	= 128*1024;
	const size_t		bt_swarm_profile_t::RECV_CMD_MAXLEN	= 129*1024;
	const bool		bt_swarm_profile_t::NO_REDUNDANT_HAVE	= false;
	const size_t		bt_swarm_profile_t::NO_NEW_ITOR_LIMIT	= 40;
	const size_t		bt_swarm_profile_t::NO_NEW_FULL_LIMIT	= 60;
	const bt_pselect_policy_t bt_swarm_profile_t::PSELECT_POLICY	= bt_pselect_policy_t::FIXED;
#else	// debug only value
	const delay_t		bt_swarm_profile_t::KEEPALIVE_PERIOD	= delay_t::from_sec(2*60);
	const delay_t		bt_swarm_profile_t::FULL_IDLE_TIMEOUT	= delay_t::from_sec(4*60);
	const rate_estim_arg_t	bt_swarm_profile_t::FULL_RATE_ESTIM_ARG	= rate_estim_arg_t()
										.total_delay(delay_t::from_sec(20))
										.slice_delay(delay_t::from_sec(2));	
	const delay_t		bt_swarm_profile_t::ITOR_TIMEOUT_DELAY	= delay_t::from_sec(20);
	const delay_t		bt_swarm_profile_t::ITOR_BLACKLIST_DELAY= delay_t::from_min(60);
	const bt_jamrc4_type_t	bt_swarm_profile_t::ITOR_JAMRC4_TYPE	= strtype_bt_jamrc4_type_t::NOJAM;
	const size_t		bt_swarm_profile_t::XMIT_REQ_MAXLEN	= 16*1024;
	const size_t		bt_swarm_profile_t::RECV_REQ_MAXLEN	= 128*1024;
	const size_t		bt_swarm_profile_t::RECV_CMD_MAXLEN	= 129*1024;
	const bool		bt_swarm_profile_t::NO_REDUNDANT_HAVE	= false;
	const size_t		bt_swarm_profile_t::NO_NEW_ITOR_LIMIT	= 40;
	const size_t		bt_swarm_profile_t::NO_NEW_FULL_LIMIT	= 60;
	const bt_pselect_policy_t bt_swarm_profile_t::PSELECT_POLICY	= bt_pselect_policy_t::FIXED;
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_profile_t::bt_swarm_profile_t()	throw()
{
	keepalive_period	(KEEPALIVE_PERIOD);
	full_idle_timeout	(FULL_IDLE_TIMEOUT);
	full_rate_estim_arg	(FULL_RATE_ESTIM_ARG);
	itor_timeout_delay	(ITOR_TIMEOUT_DELAY);
	itor_blacklist_delay	(ITOR_BLACKLIST_DELAY);
	itor_jamrc4_type	(ITOR_JAMRC4_TYPE);
	xmit_req_maxlen		(XMIT_REQ_MAXLEN);
	recv_req_maxlen		(RECV_REQ_MAXLEN);
	recv_cmd_maxlen		(RECV_CMD_MAXLEN);
	no_redundant_have	(NO_REDUNDANT_HAVE);
	no_new_itor_limit	(NO_NEW_ITOR_LIMIT);
	no_new_full_limit	(NO_NEW_FULL_LIMIT);
	pselect_policy		(PSELECT_POLICY);
}

/** \brief Destructor
 */
bt_swarm_profile_t::~bt_swarm_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_swarm_profile_t::check()	const throw()
{
	// if the bt_swarm_sched_profile_t req_queue_minlen() is less than the xmit_req_maxlen()
	// return an error
	if( sched().req_queue_minlen() < xmit_req_maxlen() )
		return bt_err_t(bt_err_t::ERROR, "swarm_profile.req_queue_minlen() MUST be greated than xmit_req_maxlen()");
	// if pselect_policy() is null, return an error
	if( pselect_policy().is_null() )
		return bt_err_t(bt_err_t::ERROR, "swarm_profile.pselect_policy() MUST be explicitly set");
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

