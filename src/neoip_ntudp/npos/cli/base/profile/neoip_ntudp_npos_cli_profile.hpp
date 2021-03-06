/*! \file
    \brief Header of the \ref ntudp_npos_cli_profile_t

*/


#ifndef __NEOIP_NTUDP_NPOS_CLI_PROFILE_HPP__ 
#define __NEOIP_NTUDP_NPOS_CLI_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class ntudp_npos_cli_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the maximum number of INETREACH_REPLY received before giving up
	static const size_t		MAX_RECVED_INETREACH_REPLY;	
	//! the delaygen_arg_t to use for pkt rxmit during the itor phase of a client
	static const delaygen_arg_t	CLI_ITOR_PKT_RXMIT;
	//! the delaygen_arg_t to use for pkt rxmit during the full phase of a client
	static const delaygen_arg_t	CLI_FULL_PKT_RXMIT;
public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_cli_profile_t()	throw();
	~ntudp_npos_cli_profile_t()	throw();

	/*************** validity function	*******************************/
	ntudp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		ntudp_npos_cli_profile_t &var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
	
	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( size_t		, max_recved_inetreach_reply);
	PROFILE_VAR_PLAIN( delaygen_arg_t	, cli_itor_pkt_rxmit);
	PROFILE_VAR_PLAIN( delaygen_arg_t	, cli_full_pkt_rxmit);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_CLI_PROFILE_HPP__  */



