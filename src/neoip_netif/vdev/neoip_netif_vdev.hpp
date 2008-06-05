/*! \file
    \brief Declaration of the udp_resp_t
    
*/


#ifndef __NEOIP_NETIF_VDEV_HPP__ 
#define __NEOIP_NETIF_VDEV_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_netif_vdev_cb.hpp"
#include "neoip_netif_vdev_wikidbg.hpp"
#include "neoip_netif_err.hpp"
#include "neoip_netif_util.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Handle a virtual device acting a network interface
 * 
 * - aka /dev/net/tun on linux
 */
class netif_vdev_t : NEOIP_COPY_CTOR_DENY, public fdwatch_cb_t
					, private wikidbg_obj_t<netif_vdev_t, netif_vdev_wikidbg_init> {
public:	//! constant to pass as parameter for netif_util_t::set_updown()
	enum vdev_type_t {
		NONE,
		TYPE_TUN,	//!< to set a /dev/tun - point to point device
		TYPE_TAP,	//!< to set a /dev/tap - ethernet like device
		MAX
	};
	enum ethertype_t {
		TYPE_IP4	= 0x800,	//!< the ethertype for IPv4
		TYPE_ARP	= 0x806		//!< the ethertype for ARP
	};
private:
	std::string	netif_name;	//!< the name of the network interface
	
	/*************** Internal function	*******************************/
	netif_err_t	vdev_allocopen(vdev_type_t type, const std::string &wished_name)	throw();

	/*************** callback stuff	***************************************/
	netif_vdev_cb_t*callback;	//!< the callback to notify when a packet is received 
	void *		userptr;	//!< the userptr associated with this callback

	/*************** fdwatch stuff	***************************************/
	fdwatch_t *	fdwatch;	//!< fdwatch to watch the socket fd
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();
public:
	/*************** ctor/dtor	***************************************/
	netif_vdev_t()	throw();
	~netif_vdev_t()	throw();
		
	/*************** setup function	***************************************/
	netif_err_t	start(netif_vdev_cb_t *callback, void *userptr, vdev_type_t type
						, const std::string &wished_name = std::string()) throw();

	/*************** Query function	***************************************/
	const std::string &	get_name()	const throw()	{ return netif_name;	}

	/*************** send packet	***************************************/
	netif_err_t	send_pkt(uint16_t ethertype, pkt_t &pkt)		const throw();

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const netif_vdev_t &netif_vdev)	throw()
			{ return os << netif_vdev.to_string();	}	

	/*************** helper/shortcut to netif_util_t	***************/
	// NOTE: those are just helpers - the authoritative stuff is in netif_util_t
	// TODO why the netif_ip_addr/netmask/bcast_addr are not accessible from here ?
	size_t		get_mtu()				const throw()
			{ return netif_util_t::get_mtu(get_name());			}
	netif_err_t	set_mtu(size_t new_mtu)			const throw()
			{ return netif_util_t::set_mtu(get_name(), new_mtu);		}
	int		get_index()				const throw()
			{ return netif_util_t::get_index(get_name());			}
	netif_err_t	set_newname(const std::string &newname)	throw();
	netif_err_t	set_updown(bool upF)			const throw()
			{ return netif_util_t::set_updown(get_name(), upF);		}
	netif_err_t	set_ip_addr(const ip_addr_t &ip_addr)			const throw()
			{ return netif_util_t::set_netif_ip_addr(get_name(), ip_addr);	}
	netif_err_t	add_route(const ip_netaddr_t &ip_netaddr
						, const ip_addr_t &gw_ip_addr = ip_addr_t()
						, int route_flag = 0)		const throw()
			{ return netif_util_t::add_route(get_name(), ip_netaddr, gw_ip_addr, route_flag);}
	netif_err_t	set_netaddr(const ip_netaddr_t ip_netaddr)	const throw();

	/*************** List of friend class	*******************************/
	friend class	netif_vdev_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_NETIF_VDEV_HPP__ 



