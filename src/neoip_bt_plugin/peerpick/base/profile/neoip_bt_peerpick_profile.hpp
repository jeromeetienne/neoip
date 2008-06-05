/*! \file
    \brief Header of the \ref bt_peerpick_profile_t

*/


#ifndef __NEOIP_BT_PEERPICK_PROFILE_HPP__ 
#define __NEOIP_BT_PEERPICK_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_bt_peerpick_plain_profile.hpp"
#include "neoip_bt_peerpick_relay_profile.hpp"
#include "neoip_bt_peerpick_casti_profile.hpp"
#include "neoip_bt_peerpick_casto_profile.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;

/** \brief to handle the profile for bt_peerpick_t
 */
class bt_peerpick_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
public:
	/*************** ctor/dtor	***************************************/
	bt_peerpick_profile_t()		throw();
	~bt_peerpick_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_peerpick_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_SPROF( bt_peerpick_plain_profile_t	, plain);
	PROFILE_VAR_SPROF( bt_peerpick_relay_profile_t	, relay);
	PROFILE_VAR_SPROF( bt_peerpick_casti_profile_t	, casti);
	PROFILE_VAR_SPROF( bt_peerpick_casto_profile_t	, casto);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERPICK_PROFILE_HPP__  */



