/*! \file
    \brief Declaration of the ip_netaddr_arr_t
    
*/


#ifndef __NEOIP_IP_NETADDR_ARR_HPP__ 
#define __NEOIP_IP_NETADDR_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_ip_netaddr.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ITEM_ARR_DECLARATION_START(ip_netaddr_arr_t, ip_netaddr_t);
NEOIP_ITEM_ARR_DECLARATION_END(ip_netaddr_arr_t, ip_netaddr_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IP_NETADDR_ARR_HPP__  */



