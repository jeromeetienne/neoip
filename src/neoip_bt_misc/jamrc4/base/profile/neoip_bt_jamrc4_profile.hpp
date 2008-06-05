/*! \file
    \brief Header of the \ref bt_jamrc4_profile_t

*/


#ifndef __NEOIP_BT_JAMRC4_PROFILE_HPP__ 
#define __NEOIP_BT_JAMRC4_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_bt_jamrc4_type_arr.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for get_t
 */
class bt_jamrc4_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default bt_jamrc4_type_t to use for bt_jamrc4_itor_t
	static const bt_jamrc4_type_t	ITOR_TYPE_DEFAULT;
public:
	/*************** ctor/dtor	***************************************/
	bt_jamrc4_profile_t()		throw();
	~bt_jamrc4_profile_t()		throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_jamrc4_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( bt_jamrc4_type_t	, itor_type_default);
	PROFILE_VAR_SPROF( bt_jamrc4_type_arr_t	, resp_type_accept_arr);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMRC4_PROFILE_HPP__  */



