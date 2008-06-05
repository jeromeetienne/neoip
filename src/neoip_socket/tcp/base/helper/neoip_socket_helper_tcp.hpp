/*! \file
    \brief Declaration of static helper functions for the tcp layer
    
*/


#ifndef __NEOIP_SOCKET_HELPER_TCP_HPP__ 
#define __NEOIP_SOCKET_HELPER_TCP_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	tcp_profile_t;
class	socket_profile_t;
class	socket_domain_t;
class	ipport_addr_t;
class	socket_addr_t;

/** \brief static helpers for the socket stuff with socket_domain_t::TCP
 */
class socket_helper_tcp_t {
public:	/////////////////////// constant declaration ///////////////////////////
	static const socket_domain_t	DOMAIN_VAR;
public:
	static tcp_profile_t	socket_to_tcp_profile(const socket_profile_t &socket_profile)	throw();
	static ipport_addr_t	ipport_addr(const socket_addr_t &socket_addr)	throw();
	static void		factory_domain_insert()				throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_HELPER_TCP_HPP__  */










