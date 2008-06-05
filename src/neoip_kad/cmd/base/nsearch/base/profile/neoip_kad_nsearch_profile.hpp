/*! \file
    \brief Header of the \ref kad_nsearch_profile_t

*/


#ifndef __NEOIP_KAD_NSEARCH_PROFILE_HPP__ 
#define __NEOIP_KAD_NSEARCH_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class kad_nsearch_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the maximum amount of time before a kad_rpccli_t is to be considered too old to be considered
	static const delay_t		RPCCLI_SOFT_TIMEOUT;
	//! the maximum amount of time before a kad_rpccli_t timeout
	static const delay_t		RPCCLI_HARD_TIMEOUT;
	//! the default number of concurrent requests sent during a node search (noted alpha in the paper)
	static const size_t		NB_CONCURRENT_REQ;
	/** \brief the default value for caching enable - if true, once a FINDSOMEVAL is completed, the
	 *         record is replicated at the closest node it observed to the key that did not 
	 *         return the value.
	 * 
	 * - This is used only for FINDSOMEVAL, as caching in FINDALLVALUE is pointless
	 */
	static const bool		CACHING_ENABLE;	
public:
	/*************** ctor/dtor	***************************************/
	kad_nsearch_profile_t()	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		kad_nsearch_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, rpccli_soft_timeout);
	PROFILE_VAR_PLAIN( delay_t	, rpccli_hard_timeout);
	PROFILE_VAR_PLAIN( size_t	, nb_concurrent_req);
	PROFILE_VAR_PLAIN( size_t	, caching_enable);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_NSEARCH_PROFILE_HPP__  */



