/*! \file
    \brief Header of the \ref bt_http_ecnx_pool_profile_t

*/


#ifndef __NEOIP_BT_HTTP_ECNX_POOL_PROFILE_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_POOL_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delaygen_arg.hpp"
#include "neoip_bt_http_ecnx_cnx_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile for bt_http_ecnx_pool_t
 */
class bt_http_ecnx_pool_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the delaygen_arg_t used by the bt_http_ecnx_herr_t
	static const delaygen_arg_t	HERR_DELAYGEN;
	//! the maximum amount of bt_http_ecnx_cnx_t for a given hostport_str at any given time
	static const size_t		CONCURRENT_CNX_MAX;

public:
	/*************** ctor/dtor	***************************************/
	bt_http_ecnx_pool_profile_t()	throw();
	~bt_http_ecnx_pool_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_http_ecnx_pool_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delaygen_arg_t		, herr_delaygen);
	PROFILE_VAR_PLAIN( size_t			, concurrent_cnx_max);
	PROFILE_VAR_SPROF( bt_http_ecnx_cnx_profile_t	, ecnx_cnx);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_POOL_PROFILE_HPP__  */



