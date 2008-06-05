/*! \file
    \brief Header of the \ref tcp_profile_t

*/


#ifndef __NEOIP_TCP_PROFILE_HPP__ 
#define __NEOIP_TCP_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_inet_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of #include for the internal fields
#include "neoip_tcp_itor_profile.hpp"
#include "neoip_tcp_full_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class tcp_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
public:
	/*************** ctor/dtor	***************************************/
	tcp_profile_t()	throw();

	/*************** validity function	*******************************/
	inet_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		tcp_profile_t &		var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_SPROF( tcp_itor_profile_t	, itor);
	PROFILE_VAR_SPROF( tcp_full_profile_t	, full);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_PROFILE_HPP__  */



