/*! \file
    \brief Declaration of static helper functions for the udp layer
    
*/


#ifndef __NEOIP_SOCKET_HELPER_UDP_HPP__ 
#define __NEOIP_SOCKET_HELPER_UDP_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_domain_t;
class	ipport_addr_t;
class	socket_addr_t;

/** \brief static helpers for the socket stuff with socket_domain_t::UDP
 */
class socket_helper_udp_t {
public:	/////////////////////// constant declaration ///////////////////////////
	static const socket_domain_t	DOMAIN_VAR;
public:
	static ipport_addr_t	ipport_addr(const socket_addr_t &socket_addr)	throw();
	static void		factory_domain_insert()				throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_HELPER_UDP_HPP__  */










