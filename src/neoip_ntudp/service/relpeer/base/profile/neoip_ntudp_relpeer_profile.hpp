/*! \file
    \brief Header of the \ref ntudp_relpeer_profile_t

*/


#ifndef __NEOIP_NTUDP_RELPEER_PROFILE_HPP__ 
#define __NEOIP_NTUDP_RELPEER_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_relpeer_tunnel_profile.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class ntudp_relpeer_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default number of needed tunnel
	static const size_t		NB_NEEDED_TUNNEL;
public:
	/*************** ctor/dtor	***************************************/
	ntudp_relpeer_profile_t()	throw();
	~ntudp_relpeer_profile_t()	throw();

	/*************** validity function	*******************************/
	ntudp_err_t	check()		const throw();


	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name ## _val;							\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		ntudp_relpeer_profile_t &var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( size_t				, nb_needed_tunnel);
	PROFILE_VAR_SPROF( ntudp_relpeer_tunnel_profile_t	, tunnel);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RELPEER_PROFILE_HPP__  */



