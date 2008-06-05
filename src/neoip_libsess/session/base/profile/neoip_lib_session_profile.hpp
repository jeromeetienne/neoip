/*! \file
    \brief Header of the \ref lib_session_profile_t

*/


#ifndef __NEOIP_LIB_SESSION_PROFILE_HPP__ 
#define __NEOIP_LIB_SESSION_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_libsess_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_ndiag_watch_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile_t of a lib_session_peer_t
 */
class lib_session_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default expire_delay for the exit procedure
	static const delay_t		EXIT_EXPIRE_DELAY;
public:
	/*************** ctor/dtor	***************************************/
	lib_session_profile_t()		throw();
	~lib_session_profile_t()	throw();

	/*************** validity function	*******************************/
	libsess_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type		var_name ## _val;						\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		lib_session_profile_t&	var_name(const var_type &value)		throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &		var_name()	throw()		{ return var_name ## _val; }	\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN(delay_t		, exit_expire_delay	);
	PROFILE_VAR_SPROF(ndiag_watch_profile_t	, ndiag_watch		);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_SESSION_PROFILE_HPP__  */



