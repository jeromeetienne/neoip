/*! \file
    \brief Header of the \ref ntudp_peer_profile_t

*/


#ifndef __NEOIP_NTUDP_PEER_PROFILE_HPP__ 
#define __NEOIP_NTUDP_PEER_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_npos_eval_profile.hpp"
#include "neoip_delay.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class ntudp_peer_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the ntudp_pserver_rec_t key in a std::string format
	static const std::string	PSERVER_KEY_STR;
	//! the number of initial ipport_aview_t needed in ntudp_aview_pool_t
	static const size_t		NB_NEEDED_AVIEW_INIT;
	//! the default ttl of the peer record
	static const delay_t		PEER_RECORD_TTL;
	//! if true the ntudp_npos_res_t IS NOT watched, but deduced from listen ipport_aview_t
	static const bool		NO_NPOS_WATCH;
public:
	/*************** ctor/dtor	***************************************/
	ntudp_peer_profile_t()	throw();
	~ntudp_peer_profile_t()	throw();

	/*************** validity function	*******************************/
	ntudp_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type		var_name##_val;							\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		ntudp_peer_profile_t &var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( std::string	, pserver_key_str);
	PROFILE_VAR_PLAIN( size_t	, nb_needed_aview_init);
	PROFILE_VAR_PLAIN( delay_t	, peer_record_ttl);
	PROFILE_VAR_PLAIN( bool		, no_npos_watch);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PEER_PROFILE_HPP__  */



