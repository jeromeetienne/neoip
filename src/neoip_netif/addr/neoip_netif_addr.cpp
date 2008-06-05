/*! \file
    \brief Definition of the \ref netif_addr_t class


*/

/* system include */
#include <net/route.h>
#include <asm/types.h>
#include <linux/rtnetlink.h>

/* local include */
#include "neoip_netif_addr.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_ip_netaddr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compare the 2 objects ala memcmp
 */
int	netif_addr_t::compare(const netif_addr_t &other)	  const throw()
{
	// handle the null case
	if( is_null() &&  other.is_null() )	return 0;
	if(!is_null() &&  other.is_null() )	return +1;
	if( is_null() && !other.is_null() )	return -1;

	// compare the netif_name
	if( netif_name > other.netif_name )	return +1;
	if( netif_name < other.netif_name )	return -1;
	
	// compare the ip_netaddr
	if( ip_netaddr > other.ip_netaddr )	return +1;
	if( ip_netaddr < other.ip_netaddr )	return -1;

	// here both object are considered equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         netif_addr stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/** \brief Read a nlmsg from a PF_NETLINK socket
 * 
 * @return the length of the read message, or -1 if an error occured
 */
static int netif_addr_nlmsg_read(int fd, uint32_t nlmsg_seq, char *buf, int bufLen)	throw()
{
	struct nlmsghdr *	nlHdr;
	int			msgLen	= 0;
	uint32_t		pid	= getpid();

	do{
		/* read the socket */
		int	readLen = recv(fd, buf, bufLen - msgLen, 0);
		if( readLen < 0)	return -1;

		nlHdr = (struct nlmsghdr *)buf;

		/* if the header is invalid, return error */
		if( (NLMSG_OK(nlHdr, (uint32_t)readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
			return -1;

		/* it it is the last message, go away */
		if(nlHdr->nlmsg_type == NLMSG_DONE)	break;

		/* update pointer/len */
		buf 	+= readLen;
		msgLen	+= readLen;

		if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)	break;

	} while( (nlHdr->nlmsg_seq != nlmsg_seq) || (nlHdr->nlmsg_pid != pid) );
	// return the message length
	return msgLen;
}


/** \brief Parse a nlmsg from a PF_NETLINK socket and produce a netif_addr_t
 * 
 * @return the parsed netif_addr_t, .is_null() is true if an error occured
 */
static netif_addr_t netif_addr_nlmsg_parse(struct nlmsghdr *nlHdr)			throw()
{
	struct	ifaddrmsg *	ifa	= (struct ifaddrmsg *)NLMSG_DATA(nlHdr);
	struct	rtattr *	rta	= (struct rtattr *)IFA_RTA(ifa);
	int 			rt_len	= IFA_PAYLOAD(nlHdr);
	uint32_t		addr	= 0;
	ip_netaddr_t		ip_netaddr;
	std::string		netif_name;
	// currently only ipv4 is handled
	if( ifa->ifa_family != AF_INET )		return netif_addr_t();

	for( ; RTA_OK(rta,rt_len) ; rta = RTA_NEXT(rta,rt_len) ){
		switch( rta->rta_type ){
		case IFA_ADDRESS:
			addr		= ntohl(*((long *)RTA_DATA(rta)));
			ip_netaddr	= ip_netaddr_t(ip_addr_t(addr), ifa->ifa_prefixlen);
			break;
		case IFA_LABEL:
			netif_name	= (char*)RTA_DATA(rta);
			break;
		}
	}
	// if the result is incomplete, return a null netif_addr_t
	if( ip_netaddr.is_null() || netif_name.empty() )	return netif_addr_t();
	// return the result
	return netif_addr_t(netif_name, ip_netaddr);
}


/** \brief Return all the netif_addr_t for the local network interfaces
 */
netif_addr_arr_t	netif_addr_t::get_all_netif()			throw()
{
	char			buf_nlmsg[64*1024];
	struct	nlmsghdr *	nlMsg	= (struct nlmsghdr *)buf_nlmsg;;
	int			readLen;
	static uint32_t		nlmsg_seq= 0;
	netif_addr_arr_t	addr_arr;
	
	/* open a NETLINK_ROUTE socket */
	int	fd	= socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
	if( fd < 0){
		KLOG_ERR("Cant create a netlink socket due to " << neoip_strerror(errno));
		return netif_addr_arr_t();
	}
	// unsure it is necessary, but just to be sure for now
	memset(buf_nlmsg, 0, sizeof(buf_nlmsg));

	/* build the nlMsg to retrieve interface addr */
	nlMsg->nlmsg_len	= NLMSG_LENGTH(sizeof(struct ifaddrmsg));
	nlMsg->nlmsg_type	= RTM_GETADDR;
	nlMsg->nlmsg_flags	= NLM_F_DUMP | NLM_F_REQUEST;
	nlMsg->nlmsg_seq	= nlmsg_seq++;
	nlMsg->nlmsg_pid	= getpid();
	
	// write the message to the socket
	if( write(fd, nlMsg, nlMsg->nlmsg_len) < 0){
		KLOG_ERR("Failed to write on netlink socket due to " << neoip_strerror(errno));
		goto close_fd;
	}

	// read the message from the socket
	readLen = netif_addr_nlmsg_read(fd, nlmsg_seq, buf_nlmsg, sizeof(buf_nlmsg));
	if( readLen < 0){
		KLOG_ERR("Failed to read on netlink socket.\n");
		goto close_fd;
	}

	// log to debug	
	KLOG_DBG("readLen=" << readLen);

	// parse the retrieved nlMsg
	for( ;NLMSG_OK(nlMsg, (uint32_t)readLen); nlMsg = NLMSG_NEXT(nlMsg, readLen) ){
		// parse the netlink message
		netif_addr_t	netif_addr = netif_addr_nlmsg_parse(nlMsg);
		// if the parsing fails, goto the next netif_addr
		if( netif_addr.is_null() )	continue;
		// add the item to the list if no error occur
		addr_arr += netif_addr;
	}

	// close the NETLINK_ROUTE socket
	close(fd);
	// return the addr_arr
    	return addr_arr;

close_fd:;	close(fd);
		// return nothing
		return netif_addr_arr_t();
}





NEOIP_NAMESPACE_END


