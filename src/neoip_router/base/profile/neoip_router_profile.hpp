/*! \file
    \brief Header of the \ref router_profile_t

*/


#ifndef __NEOIP_ROUTER_PROFILE_HPP__ 
#define __NEOIP_ROUTER_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_router_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_delay.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_dnsgrab_arg.hpp"
#include "neoip_router_name.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile_t of a router_peer_t
 */
class router_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the amount of time before a router_resp_cnx_t timeout
	static const delay_t		RESP_CNX_TIMEOUT;
	//! the ttl to set for the peer record
	static const delay_t		PEER_RECORD_TTL;	
	//! the default amount of time a given peerid/ip_addr_t will remain in the router_acache_t
	static const delay_t		ACACHE_TTL;
	//! the default amount of time a given remote_peerid stays in the itor_negcache
	static const delay_t		ITOR_NEGCACHE_TTL;
	//! the default amount of packet queue in a router_itor_t while waiting for cnx_established
	static const size_t		ITOR_INNERPKT_QUEUE_MAXLEN;
	//! the delay_t before a router_itor_t is considered timedout
	static const delay_t		ITOR_EXPIRE_DELAY;
	//! the delaygen_arg_t used to rxmit the address negociation packets
	static const delaygen_arg_t	ADDRNEGO_DELAYGEN;
	//! the default router_name_t domain for the selfsigned identity
	static const router_name_t	SELFSIGNED_DOMAIN;
	//! the default router_name_t domain for the selfsigned identity
	static const std::string	SELFSIGNED_DOMAIN_STR;
	//! the default domain name for the nonesigned identity
	static const std::string	NONESIGNED_DOMAIN_STR;
	//! true if the router MUST NOT forward packet or modify the box ip address/route - debug only
	static const bool		TUNNEL_STUB;
public:
	/*************** ctor/dtor	***************************************/
	router_profile_t()	throw();
	~router_profile_t()	throw();

	/*************** validity function	*******************************/
	router_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type		var_name ## _val;						\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		router_profile_t&	var_name(const var_type &value)		throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &		var_name()	throw()		{ return var_name ## _val; }	\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN(delay_t		, resp_cnx_timeout);
	PROFILE_VAR_PLAIN(delay_t		, peer_record_ttl);	
	PROFILE_VAR_PLAIN(delay_t		, acache_ttl);
	PROFILE_VAR_PLAIN(delay_t		, itor_negcache_ttl);
	PROFILE_VAR_PLAIN(size_t		, itor_innerpkt_queue_maxlen);
	PROFILE_VAR_PLAIN(delay_t		, itor_expire_delay);
	PROFILE_VAR_PLAIN(delaygen_arg_t	, addrnego_delaygen);
	PROFILE_VAR_PLAIN(router_name_t		, selfsigned_domain);
	PROFILE_VAR_PLAIN(std::string		, selfsigned_domain_str);
	PROFILE_VAR_PLAIN(std::string		, nonesigned_domain_str);
	PROFILE_VAR_PLAIN(bool			, tunnel_stub);
	PROFILE_VAR_SPROF(dnsgrab_arg_t		, dnsgrab_arg);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_PROFILE_HPP__  */



