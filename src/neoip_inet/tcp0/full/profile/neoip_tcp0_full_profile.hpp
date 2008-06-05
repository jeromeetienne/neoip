/*! \file
    \brief Header of the \ref tcp_full_profile_t

*/


#ifndef __NEOIP_TCP_FULL_PROFILE_HPP__ 
#define __NEOIP_TCP_FULL_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_inet_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class tcp_full_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default recv_max_len
	static const ssize_t		RECV_MAX_LEN;
	//! the default sendbuf_max_len
	static const ssize_t		SENDBUF_MAX_LEN;
public:
	/*************** ctor/dtor	***************************************/
	tcp_full_profile_t()	throw();

	/*************** validity function	*******************************/
	inet_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		tcp_full_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( ssize_t	, recv_max_len);
	PROFILE_VAR_PLAIN( ssize_t	, sendbuf_max_len);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_FULL_PROFILE_HPP__  */



