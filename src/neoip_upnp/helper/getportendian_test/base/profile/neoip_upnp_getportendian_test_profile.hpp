/*! \file
    \brief Header of the \ref upnp_getportendian_test_profile_t

*/


#ifndef __NEOIP_UPNP_GETPORTENDIAN_TEST_PROFILE_HPP__ 
#define __NEOIP_UPNP_GETPORTENDIAN_TEST_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delay.hpp"
#include "neoip_upnp_call_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the upnp_getportendian_test_t
 */
class upnp_getportendian_test_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the maximu amount of time before timing out
	static const delay_t	EXPIRE_DELAY;
	//! the maximum amount of retry for upnp_call_getport_t
	static const size_t	GETPORT_NBRETRY_MAX;
public:
	/*************** ctor/dtor	***************************************/
	upnp_getportendian_test_profile_t()	throw();
	~upnp_getportendian_test_profile_t()	throw();

	/*************** validity function	*******************************/
	upnp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		upnp_getportendian_test_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, expire_delay		);
	PROFILE_VAR_PLAIN( size_t		, getport_nbretry_max	);
	PROFILE_VAR_SPROF( upnp_call_profile_t	, call			);
	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_GETPORTENDIAN_TEST_PROFILE_HPP__  */



