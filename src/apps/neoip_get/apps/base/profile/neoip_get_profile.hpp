/*! \file
    \brief Header of the \ref get_profile_t

*/


#ifndef __NEOIP_GET_PROFILE_HPP__ 
#define __NEOIP_GET_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_delay.hpp"
#include "neoip_bt_lnk2mfile_profile.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for get_t
 */
class get_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the amount of time between 2 display
	static const delay_t		DISPLAY_PERIOD;
	//! the amount of time between 2 autosave of the bt_swarm_resumedata_t
	static const delay_t		RESUMEDATA_AUTOSAVE_PERIOD;
public:
	/*************** ctor/dtor	***************************************/
	get_profile_t()	throw();
	~get_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		get_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t			, display_period);
	PROFILE_VAR_PLAIN( delay_t			, resumedata_autosave_period);
	PROFILE_VAR_SPROF( bt_lnk2mfile_profile_t 	, lnk2mfile);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_GET_PROFILE_HPP__  */



