/*! \file
    \brief Header of the \ref tcp_itor_profile_t

*/


#ifndef __NEOIP_TCP_ITOR_PROFILE_HPP__ 
#define __NEOIP_TCP_ITOR_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_inet_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of #include for the fields
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class tcp_itor_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default timeout_delay for tcp_itor_t
	static const delay_t		TIMEOUT_DELAY;
public:
	/*************** ctor/dtor	***************************************/
	tcp_itor_profile_t()	throw();

	/*************** validity function	*******************************/
	inet_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		tcp_itor_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, timeout_delay);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_ITOR_PROFILE_HPP__  */



