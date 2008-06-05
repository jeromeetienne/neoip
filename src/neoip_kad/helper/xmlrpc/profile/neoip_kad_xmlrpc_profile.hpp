/*! \file
    \brief Header of the \ref kad_xmlrpc_profile_t

*/


#ifndef __NEOIP_KAD_KBUCKET_PROFILE_HPP__ 
#define __NEOIP_KAD_KBUCKET_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class kad_xmlrpc_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the timeout delay before an idle kad_xmlrpc_sess_t is timedout
	static const delay_t		SESSION_IDLE_TIMEOUT;
public:
	/*************** ctor/dtor	***************************************/
	kad_xmlrpc_profile_t()	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		kad_xmlrpc_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, session_idle_timeout);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_KBUCKET_PROFILE_HPP__  */



