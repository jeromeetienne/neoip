/*! \file
    \brief Header of the \ref bt_tracker_client_profile_t

*/


#ifndef __NEOIP_BT_TRACKER_CLIENT_PROFILE_HPP__ 
#define __NEOIP_BT_TRACKER_CLIENT_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// include for the fields of the profile
#include "neoip_http_sclient_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the bt_tracker_client_t
 */
class bt_tracker_client_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
public:
	/*************** ctor/dtor	***************************************/
	bt_tracker_client_profile_t()	throw();
	~bt_tracker_client_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_tracker_client_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_SPROF( http_sclient_profile_t	, http_sclient);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_CLIENT_PROFILE_HPP__  */



