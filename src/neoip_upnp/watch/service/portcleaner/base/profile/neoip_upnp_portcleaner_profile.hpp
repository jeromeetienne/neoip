/*! \file
    \brief Header of the \ref upnp_portcleaner_profile_t

*/


#ifndef __NEOIP_UPNP_PORTCLEANER_PROFILE_HPP__ 
#define __NEOIP_UPNP_PORTCLEANER_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delaygen_arg.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the upnp_portcleaner_t
 */
class upnp_portcleaner_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default delaygen_arg_t to probe the current list of allocated ports
	static const delaygen_arg_t	PROBE_DELAYGEN_ARG;
public:
	/*************** ctor/dtor	***************************************/
	upnp_portcleaner_profile_t()	throw();
	~upnp_portcleaner_profile_t()	throw();

	/*************** validity function	*******************************/
	upnp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		upnp_portcleaner_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delaygen_arg_t	, probe_delaygen_arg);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_PORTCLEANER_PROFILE_HPP__  */



