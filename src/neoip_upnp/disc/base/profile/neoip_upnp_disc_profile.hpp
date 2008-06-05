/*! \file
    \brief Header of the \ref upnp_disc_profile_t

*/


#ifndef __NEOIP_UPNP_DISC_PROFILE_HPP__ 
#define __NEOIP_UPNP_DISC_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_err.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the upnp_disc_t
 */
class upnp_disc_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default delaygen_arg_t to restransmit nudp packet
	static const delaygen_arg_t	NUDPRXMIT_DELAYGEN_ARG;
	//! the default expire_delay
	static const delay_t		EXPIRE_DELAY;
	//! true if the getportendian_test MUST be done, false otherwise
	static const bool		GETPORTENDIAN_TEST_ENABLED;
public:
	/*************** ctor/dtor	***************************************/
	upnp_disc_profile_t()	throw();
	~upnp_disc_profile_t()	throw();

	/*************** validity function	*******************************/
	upnp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		upnp_disc_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delaygen_arg_t	, nudprxmit_delaygen_arg);
	PROFILE_VAR_PLAIN( delay_t		, expire_delay);
	PROFILE_VAR_PLAIN( bool			, getportendian_test_enabled);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_DISC_PROFILE_HPP__  */



