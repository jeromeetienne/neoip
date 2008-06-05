/*! \file
    \brief Header of the \ref ntudp_pserver_pool_profile_t

*/


#ifndef __NEOIP_NTUDP_PSERVER_POOL_PROFILE_HPP__ 
#define __NEOIP_NTUDP_PSERVER_POOL_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class ntudp_pserver_pool_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the amount of time before a reachable one expires
	static const delay_t		REACH_EXPIRE;
	//! the amount of time before a unreachable one expires
	static const delay_t		UNREACH_EXPIRE;
	//! the minimal amount of time before considering a timedout pserver_addr as unreachable
	static const delay_t		UNREACH_MIN_TIMEOUT;
	//! the delaygen_arg_t to use for noquery phase
	static const delaygen_arg_t	NOQUERY_DELAYGEN;
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_pool_profile_t()	throw();
	~ntudp_pserver_pool_profile_t()	throw();

	/*************** validity function	*******************************/
	ntudp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		ntudp_pserver_pool_profile_t &var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, reach_expire);
	PROFILE_VAR_PLAIN( delay_t		, unreach_expire);
	PROFILE_VAR_PLAIN( delay_t		, unreach_min_timeout);
	PROFILE_VAR_PLAIN( delaygen_arg_t	, noquery_delaygen);
	
	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_POOL_PROFILE_HPP__  */



