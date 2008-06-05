/*! \file
    \brief Header of the \ref upnp_watch_profile_t

*/


#ifndef __NEOIP_UPNP_WATCH_PROFILE_HPP__ 
#define __NEOIP_UPNP_WATCH_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delaygen.hpp"
#include "neoip_upnp_disc_profile.hpp"
#include "neoip_upnp_call_profile.hpp"
#include "neoip_upnp_portcleaner_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the upnp_watch_t
 */
class upnp_watch_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default delaygen_arg_t to discover the upnp router
	static const delaygen_arg_t	DISC_DELAYGEN_ARG;
	//! the default delaygen_arg_t to discover the extipaddr (only when upnp_isavail)
	static const delaygen_arg_t	EXTIPADDR_DELAYGEN_ARG;
	//! true if the portcleaner cleaning MUST be done, false otherwise
	static const bool		PORTCLEANER_ENABLED;
public:
	/*************** ctor/dtor	***************************************/
	upnp_watch_profile_t()	throw();
	~upnp_watch_profile_t()	throw();

	/*************** validity function	*******************************/
	upnp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		upnp_watch_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delaygen_arg_t		, disc_delaygen_arg);
	PROFILE_VAR_PLAIN( delaygen_arg_t		, extipaddr_delaygen_arg);
	PROFILE_VAR_PLAIN( bool				, portcleaner_enabled);
	PROFILE_VAR_SPROF( upnp_disc_profile_t		, disc);
	PROFILE_VAR_SPROF( upnp_call_profile_t		, call);
	PROFILE_VAR_SPROF( upnp_portcleaner_profile_t	, portcleaner);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_WATCH_PROFILE_HPP__  */



