/*! \file
    \brief Header of the \ref upnp_call_profile_t

*/


#ifndef __NEOIP_UPNP_CALL_PROFILE_HPP__ 
#define __NEOIP_UPNP_CALL_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_err.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the upnp_call_t
 */
class upnp_call_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default expire_delay
	static const delay_t		EXPIRE_DELAY;
	//! true if the portcleaner generation MUST be done, false otherwise
	static const bool		PORTCLEANER_ENABLED;
	//! the delaygen_arg_t used to determine when to retry due to http error
	static const delaygen_arg_t	HTTPERR_RETRY_DELAYGEN;
	//! the maximum number of retry due to a http error
	static const size_t		HTTPERR_RETRY_NBMAX;
public:
	/*************** ctor/dtor	***************************************/
	upnp_call_profile_t()	throw();
	~upnp_call_profile_t()	throw();

	/*************** validity function	*******************************/
	upnp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		upnp_call_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, expire_delay		);
	PROFILE_VAR_PLAIN( bool			, portcleaner_enabled	);
	PROFILE_VAR_PLAIN( delaygen_arg_t	, httperr_retry_delaygen);
	PROFILE_VAR_PLAIN( size_t		, httperr_retry_nbmax	);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_CALL_PROFILE_HPP__  */



