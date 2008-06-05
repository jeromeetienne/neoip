/*! \file
    \brief Header of the \ref kad_profile_t

- see \ref neoip_kad_profile.cpp
*/


#ifndef __NEOIP_KAD_PROFILE_HPP__ 
#define __NEOIP_KAD_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the subprofile and direct constants 
#include "neoip_pkttype.hpp"
#include "neoip_kad_rpc_profile.hpp"
#include "neoip_kad_bstrap_profile.hpp"
#include "neoip_kad_kbucket_profile.hpp"
#include "neoip_kad_nsearch_profile.hpp"
#include "neoip_kad_db_profile.hpp"
#include "neoip_kad_clicnx_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class kad_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! true if it is the ping rpc must be answered, false otherwise
	static const pkttype_profile_t	PKTTYPE_PROFILE;
public:
	/*************** ctor/dtor	***************************************/
	kad_profile_t()			throw();
	~kad_profile_t()		throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type	var_name ## _val;							\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
		kad_profile_t&	var_name(const var_type &value)		throw()				\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &		var_name()	throw()		{ return var_name ## _val; }	\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of constant fields	***********************/
	PROFILE_VAR_PLAIN( pkttype_profile_t	, pkttype);
	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_SPROF( kad_rpc_profile_t	, rpc);
	PROFILE_VAR_SPROF( kad_bstrap_profile_t	, bstrap);
	PROFILE_VAR_SPROF( kad_kbucket_profile_t, kbucket);
	PROFILE_VAR_SPROF( kad_nsearch_profile_t, nsearch);
	PROFILE_VAR_SPROF( kad_db_profile_t	, local_db);
	PROFILE_VAR_SPROF( kad_db_profile_t	, remote_db);
	PROFILE_VAR_SPROF( kad_clicnx_profile_t	, clicnx);
	
	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_PROFILE_HPP__  */



