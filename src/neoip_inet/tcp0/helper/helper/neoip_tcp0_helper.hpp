/*! \file
    \brief Declaration of static helper functions for the tcp layer
    
*/


#ifndef __NEOIP_TCP_HELPER_HPP__ 
#define __NEOIP_TCP_HELPER_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_ipport_addr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief static helpers for the tcp layer
 */
class tcp_helper_t {
public:
	static std::pair<ipport_addr_t, ipport_addr_t>	get_free_listen_pair_from_conf()	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_HELPER_HPP__  */










