/*! \file
    \brief Header of the \ref kad_bstrap_src_profile_t

*/


#ifndef __NEOIP_KAD_BSTRAP_SRC_PROFILE_HPP__ 
#define __NEOIP_KAD_BSTRAP_SRC_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class kad_bstrap_src_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the maximum amount of time before the bstrap src start using the static file
	static const delay_t		SFILE_CREATION_DELAY;
	//! the maximum amount of time a given address will stay in the negative cache
	static const delay_t		NEGCACHE_TIMEOUT;
	//!< the amount of time_t during which nslan_src wont be queried if produced a item in negcache
	static const delay_t		NSLAN_NOQUERY_TIMEOUT;
	//!< the amount of time_t during which dfile_src wont be queried if produced a item in negcache
	static const delay_t		DFILE_NOQUERY_TIMEOUT;
	//!< the amount of time_t during which sfile_src wont be queried if produced a item in negcache
	static const delay_t		SFILE_NOQUERY_TIMEOUT;	
public:
	/*************** ctor/dtor	***************************************/
	kad_bstrap_src_profile_t()	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		kad_bstrap_src_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, sfile_creation_delay);
	PROFILE_VAR_PLAIN( delay_t	, negcache_timeout);
	PROFILE_VAR_PLAIN( delay_t	, nslan_noquery_timeout);
	PROFILE_VAR_PLAIN( delay_t	, dfile_noquery_timeout);
	PROFILE_VAR_PLAIN( delay_t	, sfile_noquery_timeout);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_SRC_PROFILE_HPP__  */



