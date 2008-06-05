/*! \file
    \brief Header of the \ref slay_profile_t

*/


#ifndef __NEOIP_SLAY_PROFILE_HPP__ 
#define __NEOIP_SLAY_PROFILE_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_profile_vapi.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_slay_domain.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// include the types for the profile fields
// ... NONE

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	slay_profile_vapi_t;

/** \ref class to store the slay profile
 */
class slay_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	// ... NONE
private:
	slay_profile_vapi_t *	m_profile_vapi;	//!< pointer on the domain specific api
public:
	/*************** ctor/dtor	***************************************/
	slay_profile_t(const slay_domain_t &slay_domain)	throw();
	slay_profile_t()	throw();
	~slay_profile_t()	throw();

	/*************** object copy stuff	*******************************/
	slay_profile_t(const slay_profile_t &other)			throw();
	slay_profile_t &operator = (const slay_profile_t & other)	throw();

	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return !m_profile_vapi;	}
	slay_profile_vapi_t *	profile_vapi()	const throw()	{ return m_profile_vapi;	}
	slay_domain_t		domain()	const throw();
	slay_err_t		check()		const throw();
	
	/*************** #define to ease the declaration	***************/
#	define NEOIP_VAR_DIRECT(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ DBG_ASSERT( !is_null() );	\
									  return var_name ## _val; }	\
		slay_profile_t &	var_name(const var_type &value)	throw()				\
						{ DBG_ASSERT( !is_null() );				\
						  var_name ## _val = value; return *this;	}
#	define NEOIP_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ DBG_ASSERT( !is_null() );		\
								  return var_name ## _val; }		\
		NEOIP_VAR_DIRECT(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	// STUB NEOIP_VAR_DIRECT( delay_t		, itor_timeout);

	/*************** #undef to ease the declaration	***********************/
#	undef NEOIP_VAR_DIRECT	
#	undef NEOIP_VAR_STRUCT
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_PROFILE_HPP__  */





