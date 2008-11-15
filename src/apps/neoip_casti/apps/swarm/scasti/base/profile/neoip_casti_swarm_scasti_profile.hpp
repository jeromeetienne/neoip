/*! \file
    \brief Header of the \ref casti_swarm_scasti_profile_t

*/


#ifndef __NEOIP_CASTI_SWARM_SCASTI_PROFILE_HPP__ 
#define __NEOIP_CASTI_SWARM_SCASTI_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_rate_estim_arg.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for casti_swarm_t
 */
class casti_swarm_scasti_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the rate_estim_t argument for the recv_rate of bt_cast_httpi_t
	static const rate_estim_arg_t	RATE_ESTIM_ARG;
public:
	/*************** ctor/dtor	***************************************/
	casti_swarm_scasti_profile_t()	throw();
	~casti_swarm_scasti_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		casti_swarm_scasti_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_SPROF( rate_estim_arg_t	, rate_estim_arg);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_SWARM_SCASTI_PROFILE_HPP__  */



