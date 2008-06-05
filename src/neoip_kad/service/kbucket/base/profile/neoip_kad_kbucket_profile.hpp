/*! \file
    \brief Header of the \ref kad_kbucket_profile_t

*/


#ifndef __NEOIP_KAD_KBUCKET_PROFILE_HPP__ 
#define __NEOIP_KAD_KBUCKET_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class kad_kbucket_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the timeout delay before the remote peer of the ping rpc is considered unreachable
	static const delay_t		PING_RPC_TIMEOUT;
	//! the timeout delay for the findnode rpc used to refresh a kbucklist_t
	static const delay_t		REFRESHING_FINDNODE_TIMEOUT;
	//! the timeout delay before a idle kbucklist_t get refreshed
	// TODO what is the relation ship with inter storer republish period	
	static const delay_t		REFRESH_IDLE_BUCKLIST_TIMEOUT;	
	//! the default replication factor (noted k in the paper)
	static const size_t		NB_REPLICATION;
	//! the default number of bit defining the kbucket width (MUST be a divisor of kad_peerid_t width)
	static const size_t		KBUCKET_WIDTH;
	//! if a rpc timedout after a delay larger than this, it got delete, else it isnt
	static const delay_t		DELETE_TIMEDOUT_RPC_MIN_DELAY;
public:
	/*************** ctor/dtor	***************************************/
	kad_kbucket_profile_t()	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		kad_kbucket_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, ping_rpc_timeout);
	PROFILE_VAR_PLAIN( delay_t	, refreshing_findnode_timeout);
	PROFILE_VAR_PLAIN( delay_t	, refresh_idle_bucklist_timeout);
	PROFILE_VAR_PLAIN( size_t	, nb_replication);
	PROFILE_VAR_PLAIN( size_t	, kbucket_width);
	PROFILE_VAR_PLAIN( delay_t	, delete_timedout_rpc_min_delay);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_KBUCKET_PROFILE_HPP__  */



