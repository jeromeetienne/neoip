/*! \file
    \brief Header for handling the convertion of netif_stat_t from/to torrent file
    
*/


#ifndef __NEOIP_NETIF_STAT_HELPER_HPP__ 
#define __NEOIP_NETIF_STAT_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_netif_stat.hpp"
#include "neoip_netif_stat_arr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief static helpers to manipulate netiof
 */
class netif_stat_helper_t {
public:
	static netif_stat_t		one_netif_stat(const std::string &netif_name)	throw();
	static netif_stat_arr_t		all_netif_stat()				throw();
	static std::vector<std::string>	all_netif_name()				throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_STAT_HELPER_HPP__  */










