/*! \file
    \brief Header of the \ref http_sresp_cnx_profile_t

*/


#ifndef __NEOIP_HTTP_SRESP_CNX_PROFILE_HPP__ 
#define __NEOIP_HTTP_SRESP_CNX_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_http_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class http_sresp_cnx_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default timeout delay
	static const delay_t	EXPIRE_DELAY;
	//! the default maximym size which can be received into a http_method_t::POST
	static const size_t	RECV_POST_MAXLEN;
public:
	/*************** ctor/dtor	***************************************/
	http_sresp_cnx_profile_t()	throw();

	/*************** validity function	*******************************/
	http_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		http_sresp_cnx_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, expire_delay);
	PROFILE_VAR_PLAIN( size_t	, recv_post_maxlen);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SRESP_CNX_PROFILE_HPP__  */



