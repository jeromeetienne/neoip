/*! \file
    \brief Header of the \ref ntudp_profile_t

*/


#ifndef __NEOIP_NTUDP_PROFILE_HPP__ 
#define __NEOIP_NTUDP_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_peer_profile.hpp"
#include "neoip_ntudp_pserver_pool_profile.hpp"
#include "neoip_ntudp_pserver_profile.hpp"
#include "neoip_ntudp_relpeer_profile.hpp"
#include "neoip_ntudp_npos_watch_profile.hpp"
#include "neoip_ntudp_resp_profile.hpp"
#include "neoip_ntudp_itor_profile.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class ntudp_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/*************** ctor/dtor	***************************************/
	ntudp_profile_t()	throw();
	~ntudp_profile_t()	throw();

	/*************** validity function	*******************************/
	ntudp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type		var_name ## _val;						\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		ntudp_profile_t&	var_name(const var_type &value)		throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &		var_name()	throw()		{ return var_name ## _val; }	\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_SPROF( ntudp_peer_profile_t		, peer);
	PROFILE_VAR_SPROF( ntudp_pserver_pool_profile_t	, pserver_pool);
	PROFILE_VAR_SPROF( ntudp_pserver_profile_t	, pserver);
	PROFILE_VAR_SPROF( ntudp_relpeer_profile_t	, relpeer);
	PROFILE_VAR_SPROF( ntudp_npos_watch_profile_t	, npos_watch);
	PROFILE_VAR_SPROF( ntudp_resp_profile_t		, resp);
	PROFILE_VAR_SPROF( ntudp_itor_profile_t		, itor);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PROFILE_HPP__  */



