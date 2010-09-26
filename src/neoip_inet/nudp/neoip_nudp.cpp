/*! \file
    \brief Definition of the nudp_t
    
*/

/* system include */
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_nudp.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief the maximum length of a udp payload
 * 
 * - it is estimated as udp/ipv4
 * - udp/ipv6 has longer headers (40-byte) but allow jumbo packets
 */
const size_t NEOIP_UDP_MAX_PAYLOAD_LEN	=	(64*1024 -  20 - 8);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nudp_t::nudp_t()				throw()
{
	// zero some fields
	callback	= NULL;
	fdwatch		= NULL;	
}

/** \brief destructor
 */
nudp_t::~nudp_t()				throw()
{
	// close the fdwatch if needed
	nipmem_zdelete	fdwatch;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    null function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return true is the object is null, false otherwise
 */
bool	nudp_t::is_null()	const throw()
{
	if( callback == NULL )	return true;
	return false;
}

void	nudp_t::nullify()	throw()
{
	// close the fdwatch if needed
	nipmem_zdelete	fdwatch;
	callback	= NULL;
	listen_addr	= ipport_addr_t();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       get/set parameter
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
inet_err_t	nudp_t::set_callback(nudp_cb_t *callback, void *userptr)throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
	return inet_err_t::OK;
}

/** \brief set the listen address
 */
inet_err_t	nudp_t::set_listen_addr(const ipport_addr_t &listen_addr)	throw()
{
	this->listen_addr	= listen_addr;
	return inet_err_t::OK;	
}

/** \brief return the listen_addr
 */
const ipport_addr_t &	nudp_t::get_listen_addr()				const throw()
{
	return listen_addr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      ?????
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	nudp_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return listen_addr.to_string();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       start
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief start the action
 */
inet_err_t	nudp_t::start()						throw()
{
	int			opt_on	= 1;
	struct 	sockaddr_in	addr_in;
	std::string		errstr;
	inet_err_t		inet_err;
	// check the parameter
	DBG_ASSERT( callback );
	
	// create the socket
	int	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock_fd < 0 ){
		errstr = "Cant create socket. errno=" + inet_oswarp_t::sock_strerror();
		goto error;
	}
	// set SO_REUSEADDR - to allow the socket to bind an address already bound
	inet_err= inet_oswarp_t::set_reuseaddr(sock_fd);
	if( inet_err.failed() ){
		errstr = "setsockopt SO_REUSEPORT failed due to " + inet_err.to_string();
		goto close_socket;
	}

	// set BROADCAST - to allow the socket to send packet to broadcast address
	inet_err= inet_oswarp_t::setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &opt_on, sizeof(opt_on));
	if( inet_err.failed() ){
		errstr = "setsockopt SO_BROADCAST failed due to " + inet_err.to_string();
		goto close_socket;
	}	

#if !defined(_WIN32) && !defined(__APPLE__)
	// set IP_PKTINFO - to get the source address of the incoming packet
	// - getsockname() returns the listen address and if it listen on ip ANY, it fails
	inet_err= inet_oswarp_t::setsockopt(sock_fd, IPPROTO_IP, IP_PKTINFO, &opt_on, sizeof(opt_on));
	if( inet_err.failed() ){
		errstr = "setsockopt IP_PKTINFO failed due to " + inet_err.to_string();
		goto close_socket;
	}
#endif

	// set MULTICAST_LOOP to 1
	inet_err= inet_oswarp_t::setsockopt(sock_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &opt_on, sizeof(opt_on));
	if( inet_err.failed() ){
		errstr = "setsockopt IP_MULTICAST_LOOP failed due to " + inet_err.to_string();
		goto close_socket;
	}

	// set this socket in non blocking for the connection
	inet_err	= inet_oswarp_t::set_nonblock(sock_fd);
	if( inet_err.failed() ){
		errstr = inet_err.to_string();
		goto close_socket;
	}
	
	if( !listen_addr.is_null() ){
		// bind the socket   	
		addr_in = listen_addr.to_sockaddr_in();
		if( bind(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) ){
			errstr = "cant bind socket to " + listen_addr.to_string() 
							+ " errno=" + inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
	
		// if the listen port was undefined, read the one assigned by the system
		if( listen_addr.get_port() == 0 ){
			socklen_t	addrlen = sizeof(addr_in);
			if( getsockname(sock_fd, (struct sockaddr *)&addr_in, &addrlen)){
				errstr = "cant getsocketname errno=" + inet_oswarp_t::sock_strerror();
				goto close_socket;
			}
			listen_addr = ipport_addr_t(addr_in);
		}
	}


	// some logging
	KLOG_DBG("nudp_t created on " << listen_addr);		

	// start the fdwatch
	fdwatch = nipmem_new fdwatch_t();
	fdwatch->start(sock_fd, fdwatch_t::NONE, this, NULL);
	if( !listen_addr.is_null() )	fdwatch->cond( fdwatch_t::INPUT );

	// return no error
	return inet_err_t::OK;
	// error handling
close_socket:;	close( sock_fd );
error:;		return inet_err_t(inet_err_t::SYSTEM_ERR, errstr);
}

/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
inet_err_t nudp_t::start(const ipport_addr_t &listen_addr, nudp_cb_t *callback, void *userptr)	throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("listen_addr="<< listen_addr);
	
	// set the remote address
	inet_err = set_listen_addr(listen_addr);
	if( inet_err.failed() )	return inet_err;
	// set the callback
	inet_err = set_callback(callback, userptr);
	if( inet_err.failed() )	return inet_err;
	// start the action
	return start();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  subscribe/unsubcribe
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief subscribe to a multicast group
 */
inet_err_t nudp_t::subscribe(const ip_addr_t &ip_addr)	throw()
{
	struct ip_mreq	mreq;
	inet_err_t	inet_err;
	// describe the group
	mreq.imr_multiaddr.s_addr = htonl(ip_addr.get_v4_addr());
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	// do the IP_ADD_MEMBERSHIP
	inet_err	= inet_oswarp_t::setsockopt(fdwatch->get_fd(), IPPROTO_IP
					, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if( inet_err.failed() ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant ADD_MEMBERSHIP to socket " 
				+ this->to_string() + " due to " + inet_err.to_string() );
	}
	// return no error
	return inet_err_t::OK;
}

/** \brief subscribe to a multicast group
 */
inet_err_t nudp_t::unsubscribe(const ip_addr_t &ip_addr)	throw()
{
	struct ip_mreq	mreq;
	inet_err_t	inet_err;
	// describe the group
	mreq.imr_multiaddr.s_addr = htonl(ip_addr.get_v4_addr());
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	// do the IP_DROP_MEMBERSHIP
	inet_err	= inet_oswarp_t::setsockopt(fdwatch->get_fd(), IPPROTO_IP
					, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
	if(inet_err.failed() ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant DROP_MEMBERSHIP to socket "
					+ this->to_string() + " due to " + inet_err.to_string());
	}	
	// return no error
	return inet_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       packet transmition
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Send a datagram to a given remote_addr
 */
inet_err_t nudp_t::send_to(const void *buf_ptr, size_t buf_len, const ipport_addr_t &remote_addr) throw()
{
	struct 	sockaddr_in	addr_in	= remote_addr.to_sockaddr_in();
	ssize_t	written_len	= sendto(fdwatch->get_fd(), (char *)buf_ptr, buf_len, 0
						, (struct sockaddr *)&addr_in, sizeof(addr_in));
	if( written_len != (ssize_t)buf_len ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant send to socket " 
					+ this->to_string() + " errno=" + inet_oswarp_t::sock_strerror() );
	}
	// return no error
	return inet_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       packet reception
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Read a datagram returning the local_addr and remote_addr
 * 
 * - the local_addr is the destination address of the datagram (even if the listen_addr
 *   is ANY)
 */
ssize_t	nudp_t::recvfromto(void *buf_ptr, int buf_len, ipport_addr_t &local_addr
						, ipport_addr_t &remote_addr)	throw()
{
#if !defined(_WIN32) && !defined(__APPLE__)
	struct 	sockaddr_in	addr_in;
	ssize_t			read_len;
	
	struct msghdr		msg;
	struct iovec		iov;
	struct cmsghdr *	cmptr;
	struct in_pktinfo	pktinfo;
	union {
		struct cmsghdr cm;
		char control[CMSG_SPACE(sizeof(struct in_addr)) + CMSG_SPACE(sizeof(struct in_pktinfo))];
	} control_un;
	
	// set up the msghdr for recvmsg()
	bzero(&msg, sizeof msg);
	msg.msg_control		= control_un.control;
	msg.msg_controllen	= sizeof(control_un.control);
	msg.msg_flags		= 0;
	msg.msg_name		= (struct sockaddr *)&addr_in;
	msg.msg_namelen		= sizeof(addr_in);
	iov.iov_base		= buf_ptr;
	iov.iov_len		= buf_len;
	msg.msg_iov		= &iov;
	msg.msg_iovlen		= 1;
	
	// do the recvmsg
	read_len	= recvmsg(fdwatch->get_fd(), &msg, 0);
	if( read_len < 0 ){
		KLOG_ERR("recvmsg len=" << read_len << " error=" << inet_oswarp_t::sock_strerror());
		return read_len;
	}
	
	// get the remote addr
	remote_addr = ipport_addr_t(addr_in);


	if( msg.msg_controllen < sizeof(struct cmsghdr) || (msg.msg_flags & MSG_CTRUNC) )
		return read_len;
	// get the local address
	for( cmptr = CMSG_FIRSTHDR(&msg); cmptr ;cmptr = CMSG_NXTHDR(&msg, cmptr) ){
		if( cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_PKTINFO ){
			memcpy(&pktinfo, CMSG_DATA(cmptr), sizeof(struct in_pktinfo));
			ip_addr_t	local_ip = ip_addr_t(ntohl(pktinfo.ipi_addr.s_addr));
			local_addr	= ipport_addr_t(local_ip, listen_addr.get_port());
			break;
		}
	}
	// return the read length
	return read_len;
#else
	struct 	sockaddr_in	addr_in;
	ssize_t			read_len;
	socklen_t		fromlen	= sizeof(addr_in);

	// do the recvmsg
	read_len	= recvfrom(fdwatch->get_fd(), (char *)buf_ptr, buf_len, 0
						, (struct sockaddr *)&addr_in, &fromlen);
	if( read_len < 0 )	return read_len;

	// get the remote addr
	remote_addr	= ipport_addr_t(addr_in);

	// NOTE: workaround the fact i dunno how to get local_addr on win32
	local_addr	= listen_addr; 

	// TODO there is a WSARecvMsg
	// - WSARecvMsg	http://msdn2.microsoft.com/en-us/library/ms741687.aspx
	// - IP_PKTINFO http://msdn2.microsoft.com/en-us/library/ms738586.aspx
	// - only by XP and above ? http://simplesamples.info/Networking/WSARecvMsg.php
	//   - it has example of coding too with IP_PKTINFO!!!!
	// - http://lia.net.ru/winsock/wsarecvmsg_2.html
	//   - Windows Vista or Windows XP or Windows Server "Longhorn" or Windows Server 2003
	//   - so possible to do but limited to those OS
	// - libneoip_upnp rely on this feature to get the local ip addr toward the upnp
	//   server.
	//   - i coded a workaround specific in libneoip_upnp
	//   - as just after the UDP packet, a tcp connection is established, i 
	//     use this established connection to discover the local ip_addr

	// TODO IIF this remains so, maybe set local_addr to listen_addr.
	// - better check who use this features. apparently nslan doesnt
	// - but neoip_upnp_disc.cpp does but i got a workaround
	KLOG_DBG("ignoring the local_addr setting because unclear how to do it on WIN32.");
//	EXP_ASSERT(0);

	return read_len;
#endif
}


/** \brief callback notified with fdwatch_t has an condition to notify
 */
bool	nudp_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond )	throw()
{
	ipport_addr_t		local_addr, remote_addr;
	uint8_t			data[NEOIP_UDP_MAX_PAYLOAD_LEN];

	// sanity check
	DBG_ASSERT( cb_fdwatch_cond.is_input() );

	// read the data (required to get the remote_addr)
	ssize_t	readlen	= recvfromto(data, sizeof(data), local_addr, remote_addr);
	if( readlen < 0 && inet_oswarp_t::sock_errno() == EAGAIN )	return true;
	DBG_ASSERT( readlen >= 0 );

	// log to debug
	KLOG_DBG("received udp dgram of " << readlen << "-byte. remote_addr=" << remote_addr
						<< " local_addr=" << local_addr );	

	// notify the recevied packet
	pkt_t	pkt(data, readlen);
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_inet_nudp_event_cb(userptr, *this, pkt, local_addr, remote_addr);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



