/*! \file
    \brief Declaration of static helper functions for ipport_addr_t
    
*/


#ifndef __NEOIP_IPPORT_ADDR_HELPER_HPP__ 
#define __NEOIP_IPPORT_ADDR_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_ipport_addr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief static helpers for ipport_addr_t
 */
class ipport_addr_helper_t {
public:
	static ipport_addr_t	find_avail_listen_addr(const ip_addr_t &ip_addr
						, const uint16_t &min_port
						, const uint16_t &max_port)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPPORT_ADDR_HELPER_HPP__  */










