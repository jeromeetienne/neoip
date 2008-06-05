/*! \file
    \brief Header of the \ref kad_db_profile_t

*/


#ifndef __NEOIP_KAD_DB_PROFILE_HPP__ 
#define __NEOIP_KAD_DB_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief the profile of a kad_db_t
 */
class kad_db_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! true if the kad_db_t MUST NOT accept kad_keyid_t duplicates, false otherwise
	static const bool		KEYID_NODUPS_OK;
	//! the amount of time before timing out the kad_findnode_rpc_t used in replication
	static const delay_t		REPLICATE_FINDNODE_RPC_TIMEOUT;
	//! the amount of time before timing out the kad_store_rpc_t used in replication
	static const delay_t		REPLICATE_STORE_RPC_TIMEOUT;
	//! the amount of time before timint out the kad_store_t used in the republication
	static const delay_t		REPUBLISH_STORE_TIMEOUT;
	//! the maximum size of the whole local database - 0 means unlimited
	static const size_t		LOCAL_DB_MAX_LEN;
	//! the maximum acceptable length in the local record - 0 means unlimited
	static const size_t		LOCAL_REC_MAX_LEN;
	//! the maximum acceptable ttl in the local record
	static const delay_t		LOCAL_REC_MAX_TTL;
	//! the republish period for the local records
	static const delay_t		LOCAL_REC_REPUBLISH_PERIOD;
	//! the maximum size of the whole remote database - 0 means unlimited
	static const size_t		REMOTE_DB_MAX_LEN;
	//! the maximum acceptable length in the remote record - 0 means unlimited
	static const size_t		REMOTE_REC_MAX_LEN;
	//! the maximum acceptable ttl in the remote record
	static const delay_t		REMOTE_REC_MAX_TTL;
	//! the republish period for the remote records
	static const delay_t		REMOTE_REC_REPUBLISH_PERIOD;
public:
	/*************** ctor/dtor	***************************************/
	kad_db_profile_t()	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();
	
	/*************** specific builder	*******************************/
	static kad_db_profile_t	build_local_db()	throw();
	static kad_db_profile_t	build_remote_db()	throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type		var_name##_val;							\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		kad_db_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( bool		, keyid_nodups_ok);
	PROFILE_VAR_PLAIN( delay_t	, replicate_findnode_rpc_timeout);
	PROFILE_VAR_PLAIN( delay_t	, replicate_store_rpc_timeout);
	PROFILE_VAR_PLAIN( delay_t	, republish_store_timeout);
	PROFILE_VAR_PLAIN( size_t	, db_max_len);
	PROFILE_VAR_PLAIN( size_t	, rec_max_len);
	PROFILE_VAR_PLAIN( delay_t	, rec_max_ttl);
	PROFILE_VAR_PLAIN( delay_t	, rec_republish_period);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_DB_PROFILE_HPP__  */



