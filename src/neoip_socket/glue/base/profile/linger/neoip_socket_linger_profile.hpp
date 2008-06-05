/*! \file
    \brief Header of the \ref socket_linger_profile_t

- see \ref neoip_socket_linger_profile.cpp
*/


#ifndef __NEOIP_SOCKET_LINGER_PROFILE_HPP__ 
#define __NEOIP_SOCKET_LINGER_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class socket_linger_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! to determine if the linger is on or off
	static const bool	ONOFF;
	//! the default amount of time to wait for the linger to complete before timing out
	static const delay_t	TIMEOUT;
public:
	/*************** ctor/dtor	***************************************/
	socket_linger_profile_t()	throw();
	~socket_linger_profile_t()	throw();

	/*************** validity function	*******************************/
	socket_err_t	check()	const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		socket_linger_profile_t&var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( bool			, onoff);
	PROFILE_VAR_PLAIN( delay_t		, timeout);	

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_LINGER_PROFILE_HPP__  */



