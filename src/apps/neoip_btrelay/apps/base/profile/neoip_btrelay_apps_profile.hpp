/*! \file
    \brief Header of the \ref btrelay_apps_profile_t

*/


#ifndef __NEOIP_BTRELAY_APPS_PROFILE_HPP__ 
#define __NEOIP_BTRELAY_APPS_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_delay.hpp"
#include "neoip_bt_relay_profile.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for btrelay_apps_t
 */
class btrelay_apps_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the period at which the btrelay_swarm_t with 'fully downloaded' delete all the seed
	static const delay_t	DELSEED_PERIOD;
public:
	/*************** ctor/dtor	***************************************/
	btrelay_apps_profile_t()	throw();
	~btrelay_apps_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		btrelay_apps_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, delseed_period);
	PROFILE_VAR_SPROF( bt_relay_profile_t	, bt_relay);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTRELAY_APPS_PROFILE_HPP__  */



