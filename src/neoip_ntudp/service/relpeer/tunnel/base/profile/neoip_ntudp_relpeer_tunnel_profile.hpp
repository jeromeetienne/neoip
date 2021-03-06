/*! \file
    \brief Header of the \ref ntudp_relpeer_tunnel_profile_t


*/


#ifndef __NEOIP_NTUDP_RELPEER_TUNNEL_PROFILE_HPP__ 
#define __NEOIP_NTUDP_RELPEER_TUNNEL_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class ntudp_relpeer_tunnel_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the delaygen_arg_t to use for pkt rxmit during the itor phase of the tunnel
	static const delaygen_arg_t	ITOR_PKT_RXMIT;
	//! the delaygen_arg_t to use for pkt rxmit during the full phase of the tunnel
	static const delaygen_arg_t	FULL_PKT_RXMIT;
public:
	/*************** ctor/dtor	***************************************/
	ntudp_relpeer_tunnel_profile_t()	throw();
	~ntudp_relpeer_tunnel_profile_t()	throw();

	/*************** validity function	*******************************/
	ntudp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name ## _val;							\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		ntudp_relpeer_tunnel_profile_t &var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delaygen_arg_t	, itor_pkt_rxmit);
	PROFILE_VAR_PLAIN( delaygen_arg_t	, full_pkt_rxmit);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RELPEER_TUNNEL_PROFILE_HPP__  */



