/*! \file
    \brief Header of the netif_util_t class
    
*/


#ifndef __NEOIP_NETIF_UTIL_HPP__ 
#define __NEOIP_NETIF_UTIL_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_netif_err.hpp"
#include "neoip_ip_netaddr.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief bunch of static function to manipulate network interface
 */
class netif_util_t {
public:	//! constant to pass as parameter for netif_util_t::set_updown()
	enum updown_cst_t {
		IF_DOWN,	//!< to set the netif DOWN
		IF_UP		//!< to set the netif UP
	};
	
public:
	static int		get_index(const std::string &netif_name)		throw();
	static size_t		get_mtu(const std::string &netif_name)			throw();
	static netif_err_t	set_mtu(const std::string &netif_name, size_t new_mtu)	throw();
	static netif_err_t	set_newname(const std::string &netif_name
							, const std::string &newname)	throw();
	static datum_t		get_hw_addr(const std::string &netif_name)		throw();
	static netif_err_t	set_hw_addr(const std::string &netif_name, const datum_t &hw_addr)	throw();
	static netif_err_t	set_netif_ip_addr(const std::string &netif_name, const ip_addr_t &ip_addr) throw();
	static netif_err_t	set_netif_netmask(const std::string &netif_name, const ip_addr_t &netmask) throw();
	static netif_err_t	set_netif_bcast_addr(const std::string &netif_name, const ip_addr_t &bcast_addr) throw();
	static netif_err_t	set_updown(const std::string &netif_name, bool upF)	throw();

	static netif_err_t	add_route(const std::string &netif_name, const ip_netaddr_t &ip_netaddr
						, const ip_addr_t &gw_ip_addr = ip_addr_t()
						, int route_flag = 0)		throw();
	static netif_err_t	del_route(const std::string &netif_name, const ip_netaddr_t &ip_netaddr
						, const ip_addr_t &gw_ip_addr = ip_addr_t()
						, int route_flag = 0)		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_UTIL_HPP__  */



