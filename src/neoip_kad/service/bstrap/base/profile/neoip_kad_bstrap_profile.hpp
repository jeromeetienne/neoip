/*! \file
    \brief Header of the \ref kad_bstrap_profile_t

*/


#ifndef __NEOIP_KAD_BSTRAP_PROFILE_HPP__ 
#define __NEOIP_KAD_BSTRAP_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// include the types for the profile fields
#include "neoip_kad_bstrap_src_profile.hpp"
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class kad_bstrap_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//!< the maximum number of address stored in the dynamic file
	static const size_t		DFILE_MAX_NB_ADDR;
	//!< the ttl of the peer record published in nslan
	static const delay_t		NSLAN_PEER_RECORD_TTL;
	//! the amount of time before timeout the kad_ping_rpc_t used for cand reachability
	static const delay_t		CAND_PING_RPC_TIMEOUT;
	//! the amount of time before timeout the kad_closestnode_t used for querying self peerid
	static const delay_t		CAND_CLOSESTNODE_TIMEOUT;
	//! the maximum number of concurrent candidat
	static const size_t		CAND_NB_CONCURRENT;
	//! max delay for the stop_multpub stage
	static const delay_t		STOP_MULTPUB_DELAY;
	//! min number of peer for the stop_multpub stage
	static const size_t		STOP_MULTPUB_NPEER;
	//! max delay for the stop_multany stage
	static const delay_t		STOP_MULTANY_DELAY;
	//! min number of peer for the stop_multany stage
	static const size_t		STOP_MULTANY_NPEER;
	//! max delay for the stop_singant stage
	static const delay_t		STOP_SINGANY_DELAY;
	//! min number of peer for the stop_singany stage
	static const size_t		STOP_SINGANY_NPEER;
public:
	/*************** ctor/dtor	***************************************/
	kad_bstrap_profile_t()	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		kad_bstrap_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( size_t	, dfile_max_nb_addr);
	PROFILE_VAR_PLAIN( delay_t	, nslan_peer_record_ttl);
	PROFILE_VAR_PLAIN( delay_t	, cand_ping_rpc_timeout);
	PROFILE_VAR_PLAIN( delay_t	, cand_closestnode_timeout);
	PROFILE_VAR_PLAIN( size_t	, cand_nb_concurrent);
	PROFILE_VAR_PLAIN( delay_t	, stop_multpub_delay);
	PROFILE_VAR_PLAIN( size_t	, stop_multpub_npeer);
	PROFILE_VAR_PLAIN( delay_t	, stop_multany_delay);
	PROFILE_VAR_PLAIN( size_t	, stop_multany_npeer);
	PROFILE_VAR_PLAIN( delay_t	, stop_singany_delay);
	PROFILE_VAR_PLAIN( size_t	, stop_singany_npeer);
	PROFILE_VAR_SPROF( kad_bstrap_src_profile_t	, src);	

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_PROFILE_HPP__  */



