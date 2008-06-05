/*! \file
    \brief Definition of the \ref netif_util_t class

*/

/* system include */
#ifndef _WIN32
#	include <sys/ioctl.h>
#	include <net/if.h>
#	include <net/if_arp.h>
#	include <net/route.h>
#endif
/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_netif_util.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_ip_netaddr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       static function to centralize common action
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a struct rtentry depending on the parameter
 */
static netif_err_t netif_do_ioctl(const std::string &netif_name, int ioctl_req
					, const std::string &ioctl_req_str
					, void *ioctl_param)			throw()
{
	// init the control socket
	int		fd	= socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if( fd < 0 )	return netif_err_t(netif_err_t::ERROR, "Can't open control socket due to " + neoip_strerror(errno));
	// set the ip_addr_t
	if( ioctl(fd, ioctl_req, ioctl_param) < 0 ){
		// build the error
		netif_err_t	netif_err = netif_err_t(netif_err_t::ERROR, "Can't do " + ioctl_req_str 
						+ " on " + netif_name + " control socket due to "
						+ neoip_strerror(errno));
		// log to debug
		KLOG_ERR("netif_err=" << netif_err);		
		// close the control socket
		close( fd );
		// return the built error
		return netif_err;
	}
	// close the control socket
	close( fd );
	// return no error
	return netif_err_t::OK;
}

/** \brief Return a basic struct ifreq with the netif_name init in it
 */
static struct ifreq netif_initial_ifreq(const std::string &netif_name)	throw()
{
	struct ifreq	ifr;
	// zero the struct
	memset(&ifr, 0, sizeof(ifr));
	// set the ifr_name 
	memcpy(ifr.ifr_name, netif_name.c_str(), IFNAMSIZ);
	// return the initialized ifreq
	return ifr;
}


/** \brief Build a struct rtentry depending on the parameter
 */
static struct rtentry	build_rtentry(const std::string &netif_name, const ip_netaddr_t &dest_netaddr
					, const ip_addr_t &gw_ip_addr, int route_flags)	throw()
{
	struct rtentry		rtentry;
	struct sockaddr_in	sa_in;
	// zero the rtentry
	memset( &rtentry, 0, sizeof (struct rtentry) );

	// sanity check - work only for IPv4
	DBG_ASSERT( dest_netaddr.get_base_addr().is_v4() );
	
	// set the rtentry.rt_dst
	sa_in	= dest_netaddr.get_any_addr().to_sockaddr_in();
	memcpy(&rtentry.rt_dst, &sa_in, sizeof(struct sockaddr_in));
	// set the rtentry.rt_genmask
	sa_in	= dest_netaddr.get_netmask().to_sockaddr_in();
	memcpy(&rtentry.rt_genmask, &sa_in, sizeof(struct sockaddr_in));
	// set the rtentry.rt_gateway if needed
	if( !gw_ip_addr.is_null() ){
		sa_in	= gw_ip_addr.to_sockaddr_in();
		memcpy(&rtentry.rt_gateway, &sa_in, sizeof(struct sockaddr_in));
	}

	// add the RTF_HOST flags if needed
	if( dest_netaddr.get_prefix_len() == 32 )		rtentry.rt_flags |= RTF_HOST;
	// add the RTF_GATEWAY flags if needed
	if( !gw_ip_addr.is_null() && !gw_ip_addr.is_any() )	rtentry.rt_flags |= RTF_GATEWAY;

	// set the rtentry.rt_dev
	rtentry.rt_dev	= (char *)netif_name.c_str();

	// set the RTF_UP flag
	rtentry.rt_flags |= RTF_UP;
	// set additionnal flags 
	rtentry.rt_flags |= route_flags;

	// returned the build rtentry
	return rtentry;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         get/set MTU
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the MTU of the netif_name
 */
size_t	netif_util_t::get_mtu(const std::string &netif_name)	throw()
{
	struct ifreq	ifr	= netif_initial_ifreq(netif_name);;
	// do the ioctl itself
	netif_err_t	netif_err = netif_do_ioctl(netif_name, SIOCGIFMTU, "SIOCGIFMTU", &ifr);
	if( netif_err.failed() )	return 0;
	// return the MTU
	return ifr.ifr_ifru.ifru_mtu;
}

/** \brief set the MTU of the netif_name
 */
netif_err_t netif_util_t::set_mtu(const std::string &netif_name, size_t new_mtu)	throw()
{
	struct ifreq	ifr	= netif_initial_ifreq(netif_name);
	// set the ifru_mtu
	ifr.ifr_ifru.ifru_mtu	= new_mtu;
	// do the ioctl itself
	return netif_do_ioctl(netif_name, SIOCSIFMTU, "SIOCSIFMTU", &ifr);
}


/** \brief set a newname of the netif_name
 */
netif_err_t netif_util_t::set_newname(const std::string &netif_name, const std::string &newname)throw()
{
	struct ifreq	ifr	= netif_initial_ifreq(netif_name);
	// test if the newname is not toolong
	if( newname.size() > IFNAMSIZ-1 )
		return netif_err_t(netif_err_t::ERROR, "Cant rename netif as the new name is toolong");
	// set the ifr_name
	strncpy(ifr.ifr_newname, newname.c_str(), IFNAMSIZ-1);
	ifr.ifr_newname[IFNAMSIZ-1] = '\0';
	// do the ioctl itself
	return netif_do_ioctl(netif_name, SIOCSIFNAME, "SIOCSIFNAME", &ifr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         set_netif_addr()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the ip address of the netif_name
 */
netif_err_t	netif_util_t::set_netif_ip_addr(const std::string &netif_name, const ip_addr_t &ip_addr)	throw()
{
	struct ifreq		ifr	= netif_initial_ifreq(netif_name);
	struct sockaddr_in	sa_in	= ip_addr.to_sockaddr_in();
	// set the ifr.ifr_addr
	memcpy(&ifr.ifr_addr, &sa_in, sizeof(struct sockaddr_in));
	// do the ioctl itself
	return netif_do_ioctl(netif_name, SIOCSIFADDR, "SIOCSIFADDR", &ifr);
}

/** \brief Set the network mask address of the netif_name
 */
netif_err_t	netif_util_t::set_netif_netmask(const std::string &netif_name, const ip_addr_t &ip_addr)	throw()
{
	struct ifreq		ifr	= netif_initial_ifreq(netif_name);
	struct sockaddr_in	sa_in	= ip_addr.to_sockaddr_in();
	// set the ifr.ifr_addr
	memcpy(&ifr.ifr_addr, &sa_in, sizeof(struct sockaddr_in));
	// do the ioctl itself
	return netif_do_ioctl(netif_name, SIOCSIFNETMASK, "SIOCSIFNETMASK", &ifr);
}

/** \brief Set the broadcast address of the netif_name
 */
netif_err_t	netif_util_t::set_netif_bcast_addr(const std::string &netif_name, const ip_addr_t &ip_addr)	throw()
{
	struct ifreq		ifr	= netif_initial_ifreq(netif_name);
	struct sockaddr_in	sa_in	= ip_addr.to_sockaddr_in();
	// set the ifr.ifr_addr
	memcpy(&ifr.ifr_addr, &sa_in, sizeof(struct sockaddr_in));
	// do the ioctl itself
	return netif_do_ioctl(netif_name, SIOCSIFBRDADDR, "SIOCSIFBRDADDR", &ifr);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         get_index()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the index of the netif_name
 * 
 * @return the index of the network interface or -1 in case of error
 */
int	netif_util_t::get_index(const std::string &netif_name)	throw()
{
	struct ifreq	ifr	= netif_initial_ifreq(netif_name);;
	// do the ioctl itself
	netif_err_t	netif_err = netif_do_ioctl(netif_name, SIOCGIFINDEX, "SIOCGIFINDEX", &ifr);
	if( netif_err.failed() )	return -1;
	// return the index
	return ifr.ifr_ifindex;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         set_updown()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the netif_name UP or DOWN
 */
netif_err_t	netif_util_t::set_updown(const std::string &netif_name, bool upF)		throw()
{
	struct ifreq	ifr	= netif_initial_ifreq(netif_name);;
	// get the flag
	netif_err_t	netif_err = netif_do_ioctl(netif_name, SIOCGIFFLAGS, "SIOCGIFFLAGS", &ifr);
	if( netif_err.failed() )	return netif_err;
	
	// modify the IFFFLAGS according to upF parameter
	if( upF )	ifr.ifr_flags |= IFF_UP|IFF_RUNNING;
	else		ifr.ifr_flags &= ~(IFF_UP|IFF_RUNNING);

	// set back the flags
	return netif_do_ioctl(netif_name, SIOCSIFFLAGS, "SIOCSIFFLAGS", &ifr);	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         hardware address function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Get the hardware address of the network interface
 * 
 * @return a datum_t containing the hardware address, or a null datum_t if an error occurs
 */
datum_t	netif_util_t::get_hw_addr(const std::string &netif_name)		throw()
{
	struct ifreq	ifr	= netif_initial_ifreq(netif_name);;
	// do the ioctl itself
	netif_err_t	netif_err = netif_do_ioctl(netif_name, SIOCGIFHWADDR, "SIOCGIFHWADDR", &ifr);
	if( netif_err.failed() )	return datum_t();
	// convert the hardware address to a datum_t and return it
	return datum_t((unsigned char *)(&ifr.ifr_hwaddr.sa_data), IFHWADDRLEN);
}


/** \brief Set the hardware address of the network interface
 * 
 * - NOTE: apparently the interface MUST be down to succeed
 * - NOTE: some interface support it. other dont
 *   - e.g. /dev/tun doesnt support it but /dev/tap does
 */
netif_err_t	netif_util_t::set_hw_addr(const std::string &netif_name, const datum_t &hw_addr)	throw()
{
	struct ifreq		ifr	= netif_initial_ifreq(netif_name);
	// sanity check - the hw_addr parameter MUST be the exact length
    	DBG_ASSERT( hw_addr.get_len() == IFHWADDRLEN );
       	// convert the datum into the struct ifreq
    	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    	memcpy((unsigned char *)(&ifr.ifr_hwaddr.sa_data), hw_addr.get_data(), IFHWADDRLEN);
	// do the ioctl itself
	return netif_do_ioctl(netif_name, SIOCSIFHWADDR, "SIOCSIFHWADDR", &ifr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           add/del route
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a route to the netif_name
 */
netif_err_t	netif_util_t::add_route(const std::string &netif_name, const ip_netaddr_t &dest_netaddr
					, const ip_addr_t &gw_ip_addr, int route_flags)	throw()
{
	struct rtentry	rtentry	= build_rtentry(netif_name, dest_netaddr, gw_ip_addr, route_flags);
	// do the ioctl itself
	return netif_do_ioctl(netif_name, SIOCADDRT, "SIOCADDRT", &rtentry);
}

/** \brief Delete a route to the netif_name
 */
netif_err_t	netif_util_t::del_route(const std::string &netif_name, const ip_netaddr_t &dest_netaddr
					, const ip_addr_t &gw_ip_addr, int route_flags)	throw()
{
	struct rtentry	rtentry	= build_rtentry(netif_name, dest_netaddr, gw_ip_addr, route_flags);
	// do the ioctl itself
	return netif_do_ioctl(netif_name, SIOCDELRT, "SIOCDELRT", &rtentry);
}


NEOIP_NAMESPACE_END


