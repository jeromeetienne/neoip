/*! \file
    \brief Declaration of the netif_vdev_t

\par Note about changing the netif_vdev_t name
- it has been implemented using the TUNSEFIFF ifr.ifr_name
- this feature may be interesting for neoip-router which got several of them
- using  netif_err	= netif_util_t::set_newname("/dev/tun0", "slota_if");
  return reason=Can't do SIOCSIFNAME on eth0 control socket due to Device or resource busy
  - not sure about the reason
  - why do i need to put /dev/tun0 ?
  - putting only tun0 return FROM_NETIF (reason=ERROR (reason=Can't do SIOCGIFFLAGS on
    tun0 control socket due to No such device)
- But it is possible using strncpy(ifr.ifr_name, "moremeaningfullname", sizeof(ifr.ifr_name));
  before ioctl(fd, TUNSETIFF, (void *) &ifr)

*/

/* system include */
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_tun.h>

/* local include */
#include "neoip_netif_vdev.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_pkt.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
netif_vdev_t::netif_vdev_t()		throw()
{
	// zero some fields
	fdwatch	= NULL;
}

/** \brief Desstructor
 */
netif_vdev_t::~netif_vdev_t()		throw()
{
	// close the fdwatch if needed
	if( fdwatch )	nipmem_delete	fdwatch;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        Misc function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Allocate and open the vdev, then setup the fdwatch
 * 
 * @return the file descriptor of the virtual device
 */
netif_err_t	netif_vdev_t::vdev_allocopen(vdev_type_t type, const std::string &wished_name)	throw()
{
	struct	ifreq	ifr;
	// open the device
	int	fd	= open("/dev/net/tun", O_RDWR);
	if( fd < 0 )	return netif_err_t(netif_err_t::ERROR, "Cant open /dev/net/tun due to " + neoip_strerror(errno));

	// allocate it
	memset(&ifr, 0, sizeof(ifr));
	// if there is a wished_name, copy it in the struct ifreq
	if( !wished_name.empty() )	strncpy(ifr.ifr_name, wished_name.c_str(), sizeof(ifr.ifr_name));
	// convert the vdev_type_t
	switch(type){
	case TYPE_TUN:	ifr.ifr_flags = IFF_TUN;	break;
	case TYPE_TAP:	ifr.ifr_flags = IFF_TAP;	break;
	default:	DBG_ASSERT(0);	
	}
#if 0	// TODO only to experiment with TUN_ONE_QUEUE as queue scheduler for the netif_vdev_t
	ifr.ifr_flags	|= IFF_ONE_QUEUE;
#endif
	// set the /dev/net/tun device type
	if( ioctl(fd, TUNSETIFF, (void *) &ifr) < 0 ){
		// build the error
		netif_err_t	netif_err = netif_err_t(netif_err_t::ERROR, "Cant do TUNSETIFF due to " + neoip_strerror(errno));
		// log to debug
		KLOG_ERR("Cant do TUNSETIFF due to " << neoip_strerror(errno) );
		// close the socket
		close(fd);
		// return the error
		return netif_err;
	}

	// copy the netif_name
	netif_name = ifr.ifr_name;
	
	// start the fdwatch
	fdwatch = nipmem_new fdwatch_t();
	fdwatch->start(fd, fdwatch_t::INPUT, this, NULL);
	// return no error
	return netif_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        Start() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
netif_err_t	netif_vdev_t::start(netif_vdev_cb_t *callback, void *userptr, vdev_type_t type
						, const std::string &wished_name) throw()
{
	netif_err_t	netif_err;
	// copy some parameters
	this->callback	= callback;
	this->userptr	= userptr;

	// allocate and open the virtual device itself
	netif_err	= vdev_allocopen(type, wished_name);
	if( netif_err.failed() )	return netif_err;
	
	// return no error
	return netif_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       packet reception
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback to notify when the fdwatch has events to report
 * 
 * @return return false object has been removed
 */
bool	netif_vdev_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond )	throw()
{
	// log to debug
	KLOG_DBG("enter with cond = " << cb_fdwatch_cond);
	// sanity check
	DBG_ASSERT( cb_fdwatch_cond.is_input() );

	// read the incoming packet
	uint8_t		buffer[64*1024];		// TODO constant are BAD
	struct tun_pi *	tun_hd	= (struct tun_pi *)buffer;
	ssize_t readlen = read(fdwatch->get_fd(), buffer, sizeof(buffer));

	// if the read() failed, log the event and return tokeep
	if( readlen < 0 ){
		KLOG_ERR("Can't read on netif_vdev_t " << *this << " due to " << neoip_strerror(errno));
		return true;
	}
	// if the read packet is shorted than the /dev/tun header, discard this packet
	if( readlen < (ssize_t)sizeof(*tun_hd) ){
		KLOG_ERR("read data shorted than MINIMUM header !?!?! on " << *this << " due to " << neoip_strerror(errno));
		return true;
	}

	// extract the ethertype from the tun_hd
	uint16_t	ethertype = ntohs(tun_hd->proto);
	// build a pkt_t for the received packet
	// - TODO this is a useless copy of the data :)
	pkt_t		pkt(buffer+sizeof(*tun_hd), readlen-sizeof(*tun_hd));
	// log to debug
	KLOG_DBG("Notifying ethertype=0x" << std::hex << ethertype << " pkt=" << pkt);

	// notify the event
	return callback->neoip_netif_vdev_cb(userptr, *this, ethertype, pkt);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Convert the objet into a string
 */
std::string	netif_vdev_t::to_string()	const throw()
{
	return netif_name;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       send_pkt() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Send a packet thru this virtual device
 */
netif_err_t	netif_vdev_t::send_pkt(uint16_t ethertype, pkt_t &pkt)	const throw()
{
	// log to debug
	KLOG_DBG("xmit thru " << netif_name << " ethertype=0x" << std::hex<< ethertype << std::dec
							<< " pkt=" << pkt);
	/* build the tunnel header */
	struct	tun_pi	tun_hd;
	memset(&tun_hd, 0, sizeof(tun_hd));
	tun_hd.flags = 0;
	tun_hd.proto = htons(ethertype);

	// prepend the tun_hd to the packet
	pkt.head_add(&tun_hd, sizeof(tun_hd));
	
	// send the packet
	ssize_t	written_len	= write(fdwatch->get_fd(), pkt.get_data(), pkt.get_len() );
	// if an error occured, report it
	if( written_len != (ssize_t)pkt.get_len() )
		return netif_err_t(netif_err_t::ERROR, "Cant write due to " + neoip_strerror(errno));

	// return no error
	return netif_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set a new name for this netif_vdev_t
 */
netif_err_t	netif_vdev_t::set_newname(const std::string &newname)		throw()
{
	netif_err_t	netif_err;
	// try to change name 
	netif_err	= netif_util_t::set_newname(get_name(), newname);
	// if the name change succeed, copy the new name into the local copy of it
	if( netif_err.succeed() )	netif_name	= newname;
	// return the netif_err_t
	return netif_err;
}
		
netif_err_t	netif_vdev_t::set_netaddr(const ip_netaddr_t ip_netaddr)	const throw()
{
	netif_err_t	netif_err;
	// set the ip address
	netif_err = netif_util_t::set_netif_ip_addr(get_name(), ip_netaddr.get_first_addr());
	if( netif_err.failed() )	return netif_err;
	// set the netmask
	netif_err = netif_util_t::set_netif_netmask(get_name(), ip_netaddr.get_netmask());
	if( netif_err.failed() )	return netif_err;
	// set the netmask
	netif_err = netif_util_t::set_netif_bcast_addr(get_name(), ip_netaddr.get_bcast_addr());
	if( netif_err.failed() )	return netif_err;
	// return no error
	return netif_err_t::OK;
}
NEOIP_NAMESPACE_END


