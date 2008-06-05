/*! \file
    \brief Header of the \ref pktfrag_profile_t

- see \ref neoip_pktfrag_profile.cpp
*/


#ifndef __NEOIP_PKTFRAG_PROFILE_HPP__ 
#define __NEOIP_PKTFRAG_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_pktfrag_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class pktfrag_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/*************** ctor/dtor	***************************************/
	pktfrag_profile_t()	throw();

	/*************** validity function	*******************************/
	pktfrag_err_t	check()		const throw();	

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
		pktfrag_profile_t &var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( size_t		, outter_mtu);
	PROFILE_VAR_PLAIN( size_t		, pool_max_size);
	PROFILE_VAR_PLAIN( delay_t		, pending_dgram_expire);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTFRAG_PROFILE_HPP__  */



