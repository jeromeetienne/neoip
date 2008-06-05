/*! \file
    \brief Header of the \ref ndiag_watch_profile_t

*/


#ifndef __NEOIP_NDIAG_WATCH_PROFILE_HPP__ 
#define __NEOIP_NDIAG_WATCH_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ndiag_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile_t of a ndiag_watch_peer_t
 */
class ndiag_watch_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! true if the ndiag_watch_t MUST use upnp_watch_t, false otherwise
	static const bool	ENABLE_UPNP_WATCH;
public:
	/*************** ctor/dtor	***************************************/
	ndiag_watch_profile_t()	throw();
	~ndiag_watch_profile_t()	throw();

	/*************** validity function	*******************************/
	ndiag_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type		var_name ## _val;						\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		ndiag_watch_profile_t&	var_name(const var_type &value)		throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &		var_name()	throw()		{ return var_name ## _val; }	\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN(bool		, enable_upnp_watch);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NDIAG_WATCH_PROFILE_HPP__  */



