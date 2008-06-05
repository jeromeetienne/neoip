/*! \file
    \brief Header of the \ref rate_limit_profile_t

*/


#ifndef __NEOIP_RATE_LIMIT_PROFILE_HPP__ 
#define __NEOIP_RATE_LIMIT_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_rate_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delay.hpp"
#include "neoip_rate_estim_arg.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class rate_limit_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default delay for the window allowed by the rate_limit_t::request
	static const delay_t		REQ_WINDOW_DELAY;
	//! the rate of randomness induced in the WINDOW_DELAY (to spread notification over time)
	static const double		REQ_WINDOW_RANDRATE;
	//! the default delay when 0 byte has been allow
	static const delay_t		FROZEN_WINDOW_DELAY;
	//! the rate of randomness induced in the WINDOW_DELAY (to spread notification over time)
	static const double		FROZEN_WINDOW_RANDRATE;
public:
	/*************** ctor/dtor	***************************************/
	rate_limit_profile_t()	throw();

	/*************** validity function	*******************************/
	rate_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		rate_limit_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);
	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, req_window_delay);
	PROFILE_VAR_PLAIN( double		, req_window_randrate);
	PROFILE_VAR_PLAIN( delay_t		, frozen_window_delay);
	PROFILE_VAR_PLAIN( double		, frozen_window_randrate);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_LIMIT_PROFILE_HPP__  */



