/*! \file
    \brief Header of the \ref ntudp_pserver_profile_t

*/


#ifndef __NEOIP_NTUDP_PSERVER_PROFILE_HPP__ 
#define __NEOIP_NTUDP_PSERVER_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class ntudp_pserver_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the amount of time without any received packet before closing this extcnx_t
	static const delay_t		EXTCNX_EXPIRE;
	//! the amount of time without any received packet before closing an established tunnel
	static const delay_t		TUNNEL_IDLE_TIMEOUT;
	//! The number of pkt to transmit during to close an established tunnel
	static const size_t		TUNNEL_NB_CLOSURE_PKT;
	//! the default pserver record ttl
	static const delay_t		PSERVER_RECORD_TTL;
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_profile_t()	throw();
	~ntudp_pserver_profile_t()	throw();

	/*************** validity function	*******************************/
	ntudp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		ntudp_pserver_profile_t &var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t		, extcnx_expire);
	PROFILE_VAR_PLAIN( delay_t		, tunnel_idle_timeout);
	PROFILE_VAR_PLAIN( size_t		, tunnel_nb_closure_pkt);
	PROFILE_VAR_PLAIN( delay_t		, pserver_record_ttl);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_PROFILE_HPP__  */



