/*! \file
    \brief Declaration of static helper functions for the ntlay layer
    
*/


#ifndef __NEOIP_SOCKET_HELPER_NTLAY_HPP__ 
#define __NEOIP_SOCKET_HELPER_NTLAY_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_domain_t;

/** \brief static helpers for the socket stuff with socket_domain_t::NTLAY
 */
class socket_helper_ntlay_t {
public:	/////////////////////// constant declaration ///////////////////////////
	static const socket_domain_t	DOMAIN_VAR;
public:
	static void	factory_domain_insert()			throw();

};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_HELPER_NTLAY_HPP__  */










