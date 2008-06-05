/*! \file
    \brief Header of the \ref http_sresp_profile_t

*/


#ifndef __NEOIP_HTTP_SRESP_PROFILE_HPP__ 
#define __NEOIP_HTTP_SRESP_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_http_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal field
#include "neoip_http_sresp_cnx_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class http_sresp_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
public:
	/*************** ctor/dtor	***************************************/
	http_sresp_profile_t()	throw();

	/*************** validity function	*******************************/
	http_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		http_sresp_profile_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);
		
	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_SPROF( http_sresp_cnx_profile_t	, cnx);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SRESP_PROFILE_HPP__  */



