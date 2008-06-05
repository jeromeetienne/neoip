/*! \file
    \brief Header of the \ref socket_full_profile_t

*/


#ifndef __NEOIP_SOCKET_FULL_PROFILE_HPP__ 
#define __NEOIP_SOCKET_FULL_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_socket_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of #include for the fields
#include "neoip_rate_limit_arg.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class socket_full_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default mtu discovery
	static const bool		MTU_DISCOVERY;
	//! the default rcvdata_maxlen
	static const size_t		RCVDATA_MAXLEN;
	//! the default xmitbuf_maxlen
	static const size_t		XMITBUF_MAXLEN;
public:
	/*************** ctor/dtor	***************************************/
	socket_full_profile_t()	throw();

	/*************** validity function	*******************************/
	socket_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		socket_full_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( bool			, mtu_discovery);
	PROFILE_VAR_PLAIN( size_t		, rcvdata_maxlen);
	PROFILE_VAR_PLAIN( size_t		, xmitbuf_maxlen);
	PROFILE_VAR_SPROF( rate_limit_arg_t	, xmit_limit_arg);
	PROFILE_VAR_SPROF( rate_limit_arg_t	, recv_limit_arg);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_FULL_PROFILE_HPP__  */



