/*! \file
    \brief Header of the \ref scnx_profile_t class
*/


#ifndef __NEOIP_SCNX_PROFILE_HPP__ 
#define __NEOIP_SCNX_PROFILE_HPP__ 

/* system include */
#include <iostream>

/* local include */
#include "neoip_scnx_err.hpp"
#include "neoip_skey_auth_type_arr.hpp"
#include "neoip_skey_ciph_type_arr.hpp"
#include "neoip_dh.hpp"
#include "neoip_x509.hpp"
#include "neoip_scnx_auth_ftor.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to store the static parameters of the neoip_scnx layer
 */
class scnx_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	
	/*************** ctor/dtor	***************************************/
	scnx_profile_t()		throw();
	~scnx_profile_t()		throw();

	/*************** validity function	*******************************/
	scnx_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
		scnx_profile_t &var_name(const var_type &value)	throw()					\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);


	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( dh_param_t		, dh_param);
	PROFILE_VAR_PLAIN( skey_auth_type_arr_t	, skey_auth_optlist);
	PROFILE_VAR_PLAIN( skey_ciph_type_arr_t	, skey_ciph_optlist);
	PROFILE_VAR_PLAIN( x509_privkey_t	, ident_privkey);
	PROFILE_VAR_PLAIN( x509_cert_t		, ident_cert);
	PROFILE_VAR_PLAIN( scnx_auth_ftor_t	, scnx_auth_ftor);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_PROFILE_HPP__  */



