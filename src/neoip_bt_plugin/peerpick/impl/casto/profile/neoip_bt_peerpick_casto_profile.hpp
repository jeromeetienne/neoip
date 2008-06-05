/*! \file
    \brief Header of the \ref bt_peerpick_casto_profile_t

*/


#ifndef __NEOIP_BT_PEERPICK_CASTO_PROFILE_HPP__ 
#define __NEOIP_BT_PEERPICK_CASTO_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the bt_peerpick_casto_t
 */
class bt_peerpick_casto_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default delay for a bt_reqauth_type_t::COOP
	static const delay_t	REQAUTH_COOP_TIMEOUT;
	//! the default maximum number of bt_swarm_full_t with the bt_reqauth_type_t::COOP
	static const size_t	REQAUTH_COOP_MAX;
	//! the default precedence factor for bt_reqauth_type_t::COOP
	static const double	REQAUTH_COOP_PREC;
	//! the default delay for bt_reqauth_type_t::HOPE
	static const delay_t	REQAUTH_HOPE_TIMEOUT;
	//! the default maximum number of bt_swarm_full_t with the bt_reqauth_type_t::HOPE
	static const size_t	REQAUTH_HOPE_MAX;
	//! the default precedence factor for bt_reqauth_type_t::HOPE
	static const double	REQAUTH_HOPE_PREC;
	//! the default delay for bt_reqauth_type_t::FSTART
	static const delay_t	REQAUTH_FSTART_TIMEOUT;
	//! the default maximum number of bt_swarm_full_t with the bt_reqauth_type_t::FSTART
	static const size_t	REQAUTH_FSTART_MAX;
	//! the default precedence factor for bt_reqauth_type_t::FSTART
	static const double	REQAUTH_FSTART_PREC;
	//! the default delay for bt_reqauth_type_t::IDLE
	static const delay_t	REQAUTH_IDLE_TIMEOUT;
	//! the default maximum number of bt_swarm_full_t with the bt_reqauth_type_t::IDLE
	static const size_t	REQAUTH_IDLE_MAX;
	//! the default precedence factor for bt_reqauth_type_t::IDLE
	static const double	REQAUTH_IDLE_PREC;
public:
	/*************** ctor/dtor	***************************************/
	bt_peerpick_casto_profile_t()	throw();
	~bt_peerpick_casto_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_peerpick_casto_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, reqauth_coop_timeout);
	PROFILE_VAR_PLAIN( size_t	, reqauth_coop_max);
	PROFILE_VAR_PLAIN( double	, reqauth_coop_prec);
	PROFILE_VAR_PLAIN( delay_t	, reqauth_hope_timeout);
	PROFILE_VAR_PLAIN( size_t	, reqauth_hope_max);
	PROFILE_VAR_PLAIN( double	, reqauth_hope_prec);
	PROFILE_VAR_PLAIN( delay_t	, reqauth_fstart_timeout);
	PROFILE_VAR_PLAIN( size_t	, reqauth_fstart_max);
	PROFILE_VAR_PLAIN( double	, reqauth_fstart_prec);
	PROFILE_VAR_PLAIN( delay_t	, reqauth_idle_timeout);
	PROFILE_VAR_PLAIN( size_t	, reqauth_idle_max);
	PROFILE_VAR_PLAIN( double	, reqauth_idle_prec);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERPICK_CASTO_PROFILE_HPP__  */



