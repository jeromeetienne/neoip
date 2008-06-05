/*! \file
    \brief Declaration of the neoip_slay_tls_daddr
*/


#ifndef __NEOIP_SLAY_PROFILE_BTJAMSTD_HPP__
#define __NEOIP_SLAY_PROFILE_BTJAMSTD_HPP__
/* system include */
/* local include */
#include "neoip_slay_profile_vapi.hpp"
#include "neoip_slay_helper_btjamstd.hpp"
#include "neoip_namespace.hpp"

// list of include for the various fields
#include "neoip_bt_jamstd_profile.hpp"
#include "neoip_bt_id.hpp"


NEOIP_NAMESPACE_BEGIN


// list of forward declaration
class	slay_profile_t;

/** \brief slay_domain_t::BTJAMSTD implementation of the slay_profile_vapi_t
 */
class slay_profile_btjamstd_t : public slay_profile_vapi_t {
private:
public:
	/*************** ctor/dtor	***************************************/
	slay_profile_btjamstd_t()	throw();

	/*************** query function	***************************************/
	slay_err_t		check()		const throw();
	const slay_domain_t &	domain()	const throw()	{ return slay_helper_btjamstd_t::DOMAIN_VAR;	}

	/*************** helper	***********************************************/
	static slay_profile_btjamstd_t &		from_slay(slay_profile_t &slay_profile)		throw();
	static const slay_profile_btjamstd_t &	from_slay(const slay_profile_t &slay_profile)	throw();
	
	/*************** #define to ease the declaration	***************/
#	define NEOIP_VAR_DIRECT(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; 	}	\
		slay_profile_btjamstd_t &var_name(const var_type &value)	throw()				\
					{  var_name ## _val = value; return *this;	}
#	define NEOIP_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val;	}	\
		NEOIP_VAR_DIRECT(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	NEOIP_VAR_STRUCT( bt_id_t		, shared_secret	);
	NEOIP_VAR_STRUCT( bt_jamstd_profile_t	, bt_jamstd	);

	/*************** #undef to ease the declaration	***********************/
#	undef NEOIP_VAR_DIRECT	
#	undef NEOIP_VAR_STRUCT
	

	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(slay_profile_vapi_t, slay_profile_btjamstd_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_PROFILE_BTJAMSTD_HPP__ */



