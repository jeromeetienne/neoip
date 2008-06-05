/*! \file
    \brief Header of the \ref http_client_pool_profile_t

*/


#ifndef __NEOIP_HTTP_CLIENT_POOL_PROFILE_HPP__ 
#define __NEOIP_HTTP_CLIENT_POOL_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_http_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class http_client_pool_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default timeout delay
	static const delay_t		CNX_TIMEOUT_DELAY;
	//! the default maximum number of connection (0 means infinite)
	static const size_t		CNX_NB_MAX;
public:
	/*************** ctor/dtor	***************************************/
	http_client_pool_profile_t()	throw();

	/*************** validity function	*******************************/
	http_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		http_client_pool_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t	, cnx_timeout_delay);
	PROFILE_VAR_PLAIN( size_t	, cnx_nb_max);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_CLIENT_POOL_PROFILE_HPP__  */



