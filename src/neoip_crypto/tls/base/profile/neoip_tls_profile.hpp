/*! \file
    \brief Header of the \ref tls_profile_t

*/


#ifndef __NEOIP_TLS_PROFILE_HPP__ 
#define __NEOIP_TLS_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_crypto_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_tls_authtype.hpp"
#include "neoip_x509_privkey.hpp"
#include "neoip_x509_cert_arr.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for get_t
 */
class tls_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the authentication type to use
	static const tls_authtype_t	AUTHTYPE;
	//! the number of bit for the dh parameter
	static const size_t		DH_PARAM_NBIT;
public:
	/*************** ctor/dtor	***************************************/
	tls_profile_t()		throw();
	~tls_profile_t()	throw();

	/*************** validity function	*******************************/
	crypto_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define NEOIP_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name ## _val;						\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }	\
		tls_profile_t&	var_name(const var_type &value)		throw()			\
						{ var_name ## _val = value; return *this;	}
#	define NEOIP_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }	\
		NEOIP_VAR_DIRECT(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	NEOIP_VAR_DIRECT( tls_authtype_t	, authtype		);
	NEOIP_VAR_DIRECT( size_t		, dh_param_nbit		);
	NEOIP_VAR_STRUCT( x509_privkey_t	, local_privkey		);
	NEOIP_VAR_STRUCT( x509_cert_arr_t	, local_cert_arr	);
	NEOIP_VAR_STRUCT( x509_cert_arr_t	, trusted_cert_arr	);

	/*************** #undef to ease the declaration	***********************/
#	undef NEOIP_VAR_DIRECT
#	undef NEOIP_VAR_STRUCT
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TLS_PROFILE_HPP__  */



