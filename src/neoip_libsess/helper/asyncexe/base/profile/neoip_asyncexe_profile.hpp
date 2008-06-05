/*! \file
    \brief Header of the \ref asyncexe_profile_t

*/


#ifndef __NEOIP_ASYNCEXE_PROFILE_HPP__ 
#define __NEOIP_ASYNCEXE_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_libsess_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for the asyncexe_t
 */
class asyncexe_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default expire_delay
	static const delay_t		EXPIRE_DELAY;
public:
	/*************** ctor/dtor	***************************************/
	asyncexe_profile_t()	throw();
	~asyncexe_profile_t()	throw();

	/*************** validity function	*******************************/
	libsess_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		asyncexe_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, expire_delay);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ASYNCEXE_PROFILE_HPP__  */



