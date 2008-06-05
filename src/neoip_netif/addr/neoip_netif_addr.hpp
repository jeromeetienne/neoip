/*! \file
    \brief Header of the neoip_string class
    
*/


#ifndef __NEOIP_NETIF_ADDR_HPP__ 
#define __NEOIP_NETIF_ADDR_HPP__ 
/* system include */
#include <string>
#include <vector>
/* local include */
#include "neoip_netif_err.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_datum.hpp"
#include "neoip_ip_netaddr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class netif_addr_arr_t;

/** \brief a network interface address... quite poor name
 * 
 * - it is used to report all the netif name and the attached addresses
 *   with netif_addr_t::get_all_addr()
 */
class netif_addr_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::string	netif_name;	//!< the network interface name
	ip_netaddr_t	ip_netaddr;	//!< the network address of this interface
public:
	/*************** ctor/dtor	***************************************/
	netif_addr_t()	throw()	{};
	netif_addr_t(const std::string &netif_name, const ip_netaddr_t &ip_netaddr)	throw()
			: netif_name(netif_name), ip_netaddr(ip_netaddr)	{}

	bool				is_null()	const throw()		{ return ip_netaddr.is_null();		}
	static	netif_addr_arr_t	get_all_netif()	throw();

	/*************** query function	***************************************/
	const std::string &	get_netif_name()	const throw()	{ return netif_name;	}
	const ip_netaddr_t &	get_ip_netaddr()	const throw()	{ return ip_netaddr;	}

	/*************** Comparison function	*******************************/
	int	compare(const netif_addr_t &other)	  const throw();
	bool 	operator == (const netif_addr_t & other) const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const netif_addr_t & other) const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const netif_addr_t & other) const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const netif_addr_t & other) const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const netif_addr_t & other) const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const netif_addr_t & other) const throw()	{ return compare(other) >= 0;	}

	/*************** display function	*******************************/
	std::string	to_string()	const throw()	{ return netif_name + ":" + ip_netaddr.to_string();	}
	friend	std::ostream & operator << (std::ostream & os, const netif_addr_t &netif_addr ) throw()
			{ return os << netif_addr.to_string();	}
};

// define the netif_addr_arr_t
NEOIP_ITEM_ARR_DECLARATION_START(netif_addr_arr_t, netif_addr_t);
NEOIP_ITEM_ARR_DECLARATION_END(netif_addr_arr_t, netif_addr_t);




NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_ADDR_HPP__  */



