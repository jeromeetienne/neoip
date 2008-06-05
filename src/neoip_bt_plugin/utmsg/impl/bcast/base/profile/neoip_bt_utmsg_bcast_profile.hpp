/*! \file
    \brief Header of the \ref bt_utmsg_bcast_profile_t

*/


#ifndef __NEOIP_BT_UTMSG_BCAST_PROFILE_HPP__ 
#define __NEOIP_BT_UTMSG_BCAST_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// include for the internal field
#include "neoip_delaygen.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the bt_utmsg_bcast_t
 */
class bt_utmsg_bcast_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default delay_t for the sent nonce to expire
	static const delay_t	NONCE_EXPIRE_DELAY;
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_bcast_profile_t()	throw();
	~bt_utmsg_bcast_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_utmsg_bcast_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, nonce_expire_delay);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_BCAST_PROFILE_HPP__  */



