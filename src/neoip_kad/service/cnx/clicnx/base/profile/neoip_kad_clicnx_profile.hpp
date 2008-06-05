/*! \file
    \brief Header of the \ref kad_clicnx_profile_t

*/


#ifndef __NEOIP_KAD_CLICNX_PROFILE_HPP__ 
#define __NEOIP_KAD_CLICNX_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class kad_clicnx_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the argument for delay generator for the packet retransmition
	static const delaygen_arg_t		RXMIT_DELAYGEN_ARG;
public:
	/*************** ctor/dtor	***************************************/
	kad_clicnx_profile_t()	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		kad_clicnx_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delaygen_arg_t	, rxmit_delaygen_arg);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_CLICNX_PROFILE_HPP__  */



