/*! \file
    \brief Header of the \ref upnp_bindport_profile_t

*/


#ifndef __NEOIP_UPNP_BINDPORT_PROFILE_HPP__ 
#define __NEOIP_UPNP_BINDPORT_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delaygen_arg.hpp"
#include "neoip_upnp_disc_profile.hpp"
#include "neoip_upnp_call_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the upnp_bindport_t
 */
class upnp_bindport_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the maximu amount of time the itor phase may last before notifying an error
	static const delay_t		ITOR_EXPIRE_DELAY;
	//! the maximum amount of retry for upnp_call_getport_t
	static const size_t		GETPORT_NBRETRY_MAX;
	//! the default delaygen_arg_t to launch upnp_call_addport
	static const delaygen_arg_t	ADDPORT_DELAYGEN_ARG;
	//! the lease delay for the upnp_portcleaner_t tagging
	static const delay_t		PORTCLEANER_LEASE_DELAY;
public:
	/*************** ctor/dtor	***************************************/
	upnp_bindport_profile_t()	throw();
	~upnp_bindport_profile_t()	throw();

	/*************** validity function	*******************************/
	upnp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		upnp_bindport_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, itor_expire_delay);
	PROFILE_VAR_PLAIN( size_t		, getport_nbretry_max);
	PROFILE_VAR_PLAIN( delaygen_arg_t	, addport_delaygen_arg);
	PROFILE_VAR_PLAIN( delay_t		, portcleaner_lease_delay);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_BINDPORT_PROFILE_HPP__  */



