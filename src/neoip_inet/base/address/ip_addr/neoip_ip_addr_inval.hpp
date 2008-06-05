/*! \file
    \brief Declaration of the ip_addr_inval_t - an interval_t of ip_addr_t
    
*/


#ifndef __NEOIP_IP_ADDR_INVAL_HPP__ 
#define __NEOIP_IP_ADDR_INVAL_HPP__ 
/* system include */
/* local include */
#include "neoip_ip_addr.hpp"
#include "neoip_interval.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

typedef	interval_t<ip_addr_t>		ip_addr_inval_t;
typedef	interval_item_t<ip_addr_t>	ip_addr_inval_item_t;

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IP_ADDR_INVAL_HPP__  */



