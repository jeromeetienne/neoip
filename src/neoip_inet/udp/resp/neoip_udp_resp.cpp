/*! \file
    \brief Definition of the udp_resp_t

\par About the SO_REUSEADDR option and some BSD socket API issue
- the use of this option is rather kludgy and aims to work around an issue of the 
  UDP socket API
- Description of the goal: i would like to get a listening UDP socket on a 
  given ipport_addr_t, and when i receives a packet on this listening socket
  to create a connected UDP socket for this packets. src ipport/dst ipport
  - thus all further packet exchange on  happens on the connected socket and 
    the listening socket doesnt see any of it
  - the listening socket is used only to create the connected sockets, not to handle
    the data communication itself.
  - this is much like the fd behaviour of the tcp accept()
  - btw it is perfectly ok by the udp protocol, because the packets may be
    routed perfectly to one socket or the other depending on the remote ipport
- Description of the related issue: if the listening socket is bound without
  SO_REUSEADDR, it is impossible to create the connected UDP socket because of
  a glitch in the socket API. 
  - to create a connected UDP socket, one must (i) create the socket (2) bind it
    (3) connect it
  - if the port is already bound and is not SO_REUSEADDR, the step 2 will fails
    and the step 3 will never be reached.
  - all that is due to the listening socket being bound without SO_REUSEADDR,
    can we bind it with SO_REUSEADDR ?
    - if the socket remains with SO_REUSEADDR all the time, many socket will be
      able to listen on it simultaneously, and this causes troubles
    - trouble 1: if the bound address is unicast, only ONE listening socket amoung
      the many will receive the packet, and it is unpredictable to determine
      which one.
    - trouble 2: it is impossible to detect if the port is already bound.
- Possible Solution: a kludgy one as it is dirty and got a race (small one tho)
  -# create the listening socket and bind it WITHOUT SO_REUSEADDR
  -# when creating a connected socket:
     -# set the listening socket in SO_REUSEADDR
     -# create the connected socket and pass it SO_REUSEADDR too
     -# bind() the connected socket (it works as both are in SO_REUSEADDR)
     -# connect() the connected socket
     -# unset the SO_REUSEADDR for the listening socket
  - This seems to work ok on linux.
    - aka it create the connected socket AND detect if multiple apps listen on the same port
  - there is a small race if a process A is creating a connected socket, so 
    having its listening socket in SO_REUSEADDR, and another process B, is creating
    its listening socket at this moment (using the usual "try them all until one is 
    ok" strategy). 
    - the process B wont be able to detect that the port is already used and will 
      use it.
    - at this point all will fall apart :) 
  - NOTE: i tried to unset the SO_REUSEADDR for the connected socket after the 
    binding and it fails for unknown reason.
    - i dont like this.
*/

/* system include */
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_udp_resp.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_udp_layer.hpp"
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
udp_resp_t::udp_resp_t()				throw()
{
	// zero some field
	callback	= NULL;
	fdwatch		= NULL;
	// link this object to the udp_layer_t
	udp_layer_get()->udp_resp_link(this);		
}

/** \brief destructor
 */
udp_resp_t::~udp_resp_t()				throw()
{
	// unlink this object to the udp_layer_t
	udp_layer_get()->udp_resp_unlink(this);		
	// close the fdwatch if needed
	nipmem_zdelete	fdwatch;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       get/set parameter
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
inet_err_t	udp_resp_t::set_callback(udp_resp_cb_t *callback, void *userptr)throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
	return inet_err_t::OK;
}

/** \brief set the listen address
 */
inet_err_t	udp_resp_t::set_listen_addr(const ipport_addr_t &m_listen_addr)	throw()
{
	this->m_listen_addr	= m_listen_addr;
	return inet_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief start the action
 */
inet_err_t	udp_resp_t::start()						throw()
{
	struct 	sockaddr_in	addr_in;
	std::string		errstr;
	inet_err_t		inet_err;
	// check the parameter
	DBG_ASSERT( !listen_addr().is_null() );
		
	// create the socket
	int	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock_fd < 0 ){
		errstr = "Cant create socket. errno=" + inet_oswarp_t::sock_strerror();
		goto error;
	}

	// bind the socket (with no SO_REUSEADDR)
	addr_in = listen_addr().to_sockaddr_in();
	if( bind(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) ){
		errstr = "cant bind socket to " + listen_addr().to_string() 
						+ " errno=" + inet_oswarp_t::sock_strerror();
		goto close_socket;
	}
	// if the listen port was undefined, read the one assigned by the system
	if( listen_addr().port() == 0 ){
		socklen_t	addrlen = sizeof(addr_in);
		if( getsockname(sock_fd, (struct sockaddr *)&addr_in, &addrlen)){
			errstr = "cant getsocketname errno=" + inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
		m_listen_addr = ipport_addr_t(addr_in);
	}

	// set this socket in non blocking for the connection
	inet_err	= inet_oswarp_t::set_nonblock(sock_fd);
	if( inet_err.failed() ){
		errstr = inet_err.to_string();
		goto close_socket;
	}


	// if the local_ipport has a multicast ip address, join the multicast group
	if( listen_addr().ipaddr().is_multicast() ){
		struct ip_mreq	mreq;
		// describe the group
		mreq.imr_multiaddr.s_addr = htonl(listen_addr().ipaddr().get_v4_addr());
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		// do the IP_ADD_MEMBERSHIP
		inet_err	= inet_oswarp_t::setsockopt(fdwatch->get_fd(), IPPROTO_IP
						, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
		if( inet_err.failed() ){
			errstr = "cant ADD_MEMBERSHIP to socket " + listen_addr().to_string()
						+ " due to " + inet_err.to_string();
			goto close_socket;
		}
	}

	// some logging
	KLOG_DBG("udp_resp_t created on " << listen_addr());		

	// start the fdwatch
	fdwatch = nipmem_new fdwatch_t();
	fdwatch->start(sock_fd, fdwatch_t::INPUT, this, NULL);
	// return no error
	return inet_err_t::OK;
	
close_socket:;	inet_oswarp_t::close_fd( sock_fd );
error:;		return inet_err_t(inet_err_t::SYSTEM_ERR, errstr);
}

/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
inet_err_t udp_resp_t::start(const ipport_addr_t &m_listen_addr, udp_resp_cb_t *callback, void *userptr)
										throw()
{
	inet_err_t	inet_err;
	// set the remote address
	inet_err = set_listen_addr(m_listen_addr);
	if( inet_err.failed() )	return inet_err;
	// set the callback
	inet_err = set_callback(callback, userptr);
	if( inet_err.failed() )	return inet_err;
	// start the action
	return start();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       fdwatch callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool	udp_resp_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond )	throw()
{
	struct 	sockaddr_in	addr_in;
	ipport_addr_t		local_addr, remote_addr;
	uint8_t			data[NEOIP_UDP_MAX_PAYLOAD_LEN];
	// sanity check
	DBG_ASSERT( cb_fdwatch_cond.is_input() );

	// read the first packet	
	socklen_t 	addrlen = sizeof(addr_in);
	// read the data (required to get the remote_addr)
	ssize_t		readlen	= recvfrom(cb_fdwatch.get_fd(), (char *)data, sizeof(data), 0, 
        						(struct sockaddr *)&addr_in, &addrlen);
	// back up the errno
	int 	errno_copy	= inet_oswarp_t::sock_errno();
	// log to debug
	if( readlen < 0 )	KLOG_ERR("read on udp_resp_t " << *this << " returned " << readlen << " and errno=" << inet_oswarp_t::sock_strerror(errno_copy));
	// if readlen is <= 0, leave the function now
	if( readlen <= 0 )	return true;

	// get the remote addr
	remote_addr = ipport_addr_t(addr_in);

	// get the local address
	// - It is impossible to simply read the listen_addr variable as it may be ANY ipaddr
	addrlen = sizeof(addr_in);
	if( getsockname(cb_fdwatch.get_fd(), (struct sockaddr *)&addr_in, &addrlen)){
		KLOG_ERR("cant getsockname() socket");
		return true;
	}
	local_addr = ipport_addr_t(addr_in);

	// some logging
	KLOG_DBG("received udp dgram of " << readlen << "-byte from " << remote_addr
					<< " to " << local_addr);	

	// create the udp_full_t
	udp_full_t *	udp_full = nipmem_new udp_full_t();
	inet_err_t	inet_err;
	
#if 1	// set REUSEADDR - part of the SO_REUSEADDR kludge, see udp_resp_t comment for details
	int	opt_on	= 1;
	inet_err	= inet_oswarp_t::setsockopt(fdwatch->get_fd(), SOL_SOCKET, SO_REUSEADDR
							, &opt_on, sizeof(opt_on));
	if( inet_err.failed() ){
		KLOG_ERR("Cant setsockopt REUSEADDR due to " << inet_err );
		DBG_ASSERT( 0 );
	}
#endif
	// set the local address
	inet_err	= udp_full->set_local_addr(local_addr);
	// TODO instread of assert, it should discard the packet
	DBG_ASSERT( inet_err.succeed() );

#if 1	// unset REUSEADDR - part of the SO_REUSEADDR kludge, see udp_resp_t comment for details
	opt_on	= 0;	
	inet_err	= inet_oswarp_t::setsockopt(fdwatch->get_fd(), SOL_SOCKET, SO_REUSEADDR
							, &opt_on, sizeof(opt_on));
	if( inet_err.failed() ){
		KLOG_ERR("Cant setsockopt REUSEADDR due to " << inet_err );
		DBG_ASSERT( 0 );
	}
#endif
	// set the remote address
	inet_err	= udp_full->set_remote_addr(remote_addr);
	DBG_ASSERT( inet_err.succeed() );


	// backup the object_slotid of the udp_full_t
	slot_id_t	udp_full_slotid	= udp_full->get_object_slotid();

	// notify the caller
	udp_event_t	udp_event	= udp_event_t::build_cnx_established(udp_full);
	bool		tokeep		= notify_callback(udp_event);
	if( !tokeep )	return false;

	// push back the datagram in the udp_full_t - iif it still exist, aka if not deleted by callback
	if( object_slotid_exist(udp_full_slotid) )
		udp_full->push_back_resp_dgram(data, readlen);

	// keep the fdwatch running
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			to_string() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	udp_resp_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return listen_addr().to_string();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	udp_resp_t::notify_callback(const udp_event_t &udp_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_inet_udp_resp_event_cb( userptr, *this, udp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;	
}
NEOIP_NAMESPACE_END



