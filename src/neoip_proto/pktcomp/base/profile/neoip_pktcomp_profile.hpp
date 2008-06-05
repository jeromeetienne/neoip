/*! \file
    \brief Header of the \ref pktcomp_profile_t

- see \ref neoip_pktcomp_profile.cpp
*/


#ifndef __NEOIP_PKTCOMP_PROFILE_HPP__ 
#define __NEOIP_PKTCOMP_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_pktcomp_err.hpp"
#include "neoip_compress_type_arr.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the pktcomp layer
 */
class pktcomp_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/*************** ctor/dtor	***************************************/
	pktcomp_profile_t()	throw();
	~pktcomp_profile_t()	throw();

	/*************** validity function	*******************************/
	pktcomp_err_t	check()		const throw();	

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
		pktcomp_profile_t &var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);


	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( compress_type_arr_t	, optlist);
	PROFILE_VAR_PLAIN( size_t		, max_uncompress_len);
	PROFILE_VAR_PLAIN( size_t		, nocomp_max_nb_succ);
	PROFILE_VAR_PLAIN( delay_t		, nocomp_delay_min);
	PROFILE_VAR_PLAIN( delay_t		, nocomp_delay_max);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTCOMP_PROFILE_HPP__  */



