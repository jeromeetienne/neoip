/*! \file
    \brief Definition of the udp_full_t

\par Implementation Notes
- the udp_full_t MUST bind the ports *DURING* the set_local_addr/set_remote_addr
  - it is required in order to bind the local ipport.
  - the ntudp_pview_pool_t requires it to avoid some race conditions in the binding
    of the nat traversal udp hole punching.
    
*/

/* system include */
#include <sys/types.h>
#ifdef __linux__
#	include <netinet/in.h>
#	include <linux/types.h>
#	include <linux/errqueue.h>
#endif
#if 1
#	define IP_MTU          14	// due to a conflict in the include netinet/in.h and linux/in.h
					// TODO to solve 
					// - is this IP_MTU stuff linux only ?
#else
#	include <linux/in.h>
#endif
#include <unistd.h>
#include <fcntl.h>

/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_udp_layer.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref udp_full_t constant
// the maximum IPv4 datagram length
const size_t	udp_full_t::MAX_IP4_DGRAM	= 64*1024;
// the minimum IPv4 header length
const size_t	udp_full_t::IP_HEADER_MIN_LEN	= 20;
// the udp header length
const size_t	udp_full_t::UDP_HEADER_LEN	= 8;
// the maximum length of a udp payload
const size_t	udp_full_t::UDP_PAYLOAD_MAX_LEN	= udp_full_t::MAX_IP4_DGRAM 
							- udp_full_t::IP_HEADER_MIN_LEN
							- udp_full_t::UDP_HEADER_LEN;
// end of constants definition



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
udp_full_t::udp_full_t()				throw()
{
	// log to debug
	KLOG_DBG("enter");
	// set some variable
	callback	= NULL;
	fdwatch		= NULL;
	recv_max_len	= UDP_PAYLOAD_MAX_LEN;
	sock_fd		= 0;
	// link this object to the udp_layer_t
	udp_layer_get()->udp_full_link(this);	
}

/** \brief destructor
 */
udp_full_t::~udp_full_t()				throw()
{
	// log to debug
	KLOG_DBG("enter fdwatch=" << fdwatch << " callback=" << neoip_cpp_typename(*callback));
	// unlink this object to the udp_layer_t
	udp_layer_get()->udp_full_unlink(this);
	// close the sock_fd if needed
	if( sock_fd )	inet_oswarp_t::close_fd(sock_fd);
	// close the fdwatch if needed
	nipmem_zdelete fdwatch;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    null function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return true is the object is null, false otherwise
 */
bool	udp_full_t::is_null()	const throw()
{
	if( local_addr.is_null() )	return true;
	if( remote_addr.is_null() )	return true;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       get/set parameter
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
inet_err_t	udp_full_t::set_callback(udp_full_cb_t *callback, void *userptr)throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
	return inet_err_t::OK;
}

/** \brief Create the sock_fd if not already done
 * 
 * - this is used by set_local_addr and set_remote_addr
 *   - the first of the two which is called first, will created the sock_fd
 * - this cant be done during the ctor because it may failed.
 */
inet_err_t	udp_full_t::create_sock_fd_if_needed()	throw()
{
	// if the sock_fd is already created, return no error now
	if( sock_fd )	return inet_err_t::OK;
	
	// create the socket
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock_fd < 0 ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "Cant create socket. errno="
								+ inet_oswarp_t::sock_strerror());
	}

	// return no error
	return inet_err_t::OK;	
}

/** \brief set the local address
 */
inet_err_t	udp_full_t::set_local_addr(const ipport_addr_t &local_addr)	throw()
{
	// copy the parameter
	this->local_addr	= local_addr;
	// create the sock_fd if needed
	inet_err_t inet_err	= create_sock_fd_if_needed();
	if( inet_err.failed() )	return inet_err;
	
#if 1	// set REUSEADDR - part of the SO_REUSEADDR kludge, see udp_resp_t comment for details
	int	opt_on	= 1;
	inet_err	= inet_oswarp_t::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR
							, &opt_on, sizeof(opt_on));
	if( inet_err.failed() )	return inet_err;
#endif
	// bind the socket
	struct 	sockaddr_in	addr_in	= local_addr.to_sockaddr_in();
	if( bind(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) ){
		std::string	reason	= "cant bind socket to " + local_addr.to_string()
						+ " errno=" + inet_oswarp_t::sock_strerror();
		return inet_err_t(inet_err_t::SYSTEM_ERR, reason);	
	}

#if 0
{	// set the SO_RCVBUF - there only as experimentation
	size_t	recvbuf_len	= 1024*1024;
	if( setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, &recvbuf_len, sizeof(recvbuf_len)) )
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant set SO_RCVBUF socket. errno=" + inet_oswarp_t::sock_strerror());	
}
{	// set the SO_SNDBUF - there only as experimentation
	size_t	xmitbuf_len	= 1024*1024;
	if( setsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, &xmitbuf_len, sizeof(xmitbuf_len)) )
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant set SO_RCVBUF socket. errno=" + inet_oswarp_t::sock_strerror());	
}
#endif

#if 0	// unset REUSEADDR - part of the SO_REUSEADDR kludge, see udp_resp_t comment for details
	opt_on		= 0;
	inet_err	= inet_oswarp_t::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR
							, &opt_on, sizeof(opt_on));
	if( inet_err.failed() ){
		KLOG_ERR("Cant setsockopt REUSEADDR due to " << inet_err.to_string());
		DBG_ASSERT( 0 );
	}
#endif
	// log to debug
	KLOG_DBG("local_addr=" << local_addr);
	// return no error
	return inet_err_t::OK;	
}

/** \brief set the remote address
 */
inet_err_t	udp_full_t::set_remote_addr(const ipport_addr_t &remote_addr)	throw()
{
	// copy the parameter
	this->remote_addr	= remote_addr;
	// create the sock_fd if needed
	inet_err_t inet_err	= create_sock_fd_if_needed();
	if( inet_err.failed() )	return inet_err;
	// connect the socket   	
	struct 	sockaddr_in	addr_in	= remote_addr.to_sockaddr_in();
	if( connect(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0 ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant connect socket to " + remote_addr.to_string()
							+ " errno=" + inet_oswarp_t::sock_strerror());
	}
	// return no error
	return inet_err_t::OK;	
}

/** \brief return the local_addr
 */
const ipport_addr_t &	udp_full_t::get_local_addr()				const throw()
{
	return local_addr;
}

/** \brief return the remote_addr
 */
const ipport_addr_t &	udp_full_t::get_remote_addr()				const throw()
{
	return remote_addr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       push_back_resp_dgram stuff
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief push the datagram read by neoip_inet_udp_resp_t
 */
void	udp_full_t::push_back_resp_dgram(const void *pkt_ptr, int pkt_len)	throw()
{
	// sanity check
	DBG_ASSERT( callback );
	// queue the pkt in the resp_dgram_db
	resp_dgram_db.push_back( pkt_t(pkt_ptr, pkt_len) );
	// launch the resp_dgram_zerotimer to deliver it in another iteration of the event loop
	// - it add one zerotimer per packet
	// - this avoid nested 'tokeep' issues.
	resp_dgram_zerotimer.append(this, NULL);
}

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	udp_full_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// sanity check - the resp_dgram_db MUST NOT be empty
	DBG_ASSERT( resp_dgram_db.empty() == false );
	// unqueue ONE packet
	pkt_t	resp_dgram	= resp_dgram_db.front();
	resp_dgram_db.pop_front();
	// notify the event
	bool	tokeep	= notify_callback( udp_event_t::build_recved_data(&resp_dgram) );
	if( !tokeep )	return false;
	// return tokeep
	return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       RECV_MAX_LEN get/set
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the maximum number of bytes notified by RECVED_DATA event
 * 
 * @param recv_max_len the maximum number of byte. if == 0, any input on this
 *                     connection will be ignored
 */
inet_err_t	udp_full_t::recv_max_len_set( size_t recv_max_len ) throw()
{
	this->recv_max_len	= recv_max_len;
	// if the recv_max_len is 0, make sure the input IS NOT watched
	if( this->recv_max_len == 0 && fdwatch->cond().is_input() )
		fdwatch->cond( fdwatch->cond() & ~fdwatch_t::INPUT );
	// if the recv_max_len is > 0, make sure the input IS watched
	if( this->recv_max_len > 0 && fdwatch->cond().is_input() == false )
		fdwatch->cond( fdwatch->cond() | fdwatch_t::INPUT );
	// return no error
	return inet_err_t::OK;
}

/** \brief return the recv_max_len
 */
size_t udp_full_t::recv_max_len_get() const throw()
{
	return recv_max_len;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      ?????
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	udp_full_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return local_addr.to_string() + " to " + remote_addr.to_string();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       start
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief start the action
 */
inet_err_t	udp_full_t::start()						throw()
{
	std::string	errstr;
	inet_err_t	inet_err;
	
	// sanity check - those parameters MUST be set
	DBG_ASSERT( callback );
	DBG_ASSERT( !remote_addr.is_null() );
	// NOTE: local_addr is not tested because udp_itor_t may not set it
	
	// sanity check - ensure the udp_full_t isnt already started
	DBG_ASSERT( fdwatch == NULL );
	DBG_ASSERT( sock_fd );

	/* NOTE:
	 * - at this point, the sock_fd is supposed to be already created
	 * - the local_addr to be bound if needed
	 * - the remote_addr to be connected
	 */

	// if the local_addr isnt yet set, get the dynamic value found by connect()
	if( !local_addr.is_fully_qualified() ){
		struct 	sockaddr_in	addr_in;
		socklen_t		namelen	= sizeof(addr_in);
		if( getsockname( sock_fd, (struct sockaddr *)&addr_in, &namelen)){
			errstr	= "cant getsockname() socket";
			goto close_socket;
		}
		local_addr = ipport_addr_t( addr_in );
	}
	// log to debug
	KLOG_DBG("local_addr=" << local_addr );
			
	// set this socket in non blocking for the connection
	inet_err	= inet_oswarp_t::set_nonblock(sock_fd);
	if( inet_err.failed() ){
		errstr = "cant put socket in O_NONBLOCK mode due to " + inet_err.failed();
		goto close_socket;
	}

	// if the local_addr has a multicast ip address, join the multicast group
	if( local_addr.get_ipaddr().is_multicast() ){
		struct ip_mreq	mreq;
		// describe the group
		mreq.imr_multiaddr.s_addr = htonl(local_addr.get_ipaddr().get_v4_addr());
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		// do the IP_ADD_MEMBERSHIP
		inet_err	= inet_oswarp_t::setsockopt(sock_fd, IPPROTO_IP
						, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
		if( inet_err.failed() ){
			errstr = "cant ADD_MEMBERSHIP to socket " + local_addr.to_string()
						+ " due to " + inet_err.to_string();
			goto close_socket;
		}
	}

#if !defined(_WIN32) && !defined(__APPLE__)
	// mark the socket as willing to do path mtu discovery
{	int	optval = 1;
	if(setsockopt(sock_fd, SOL_IP, IP_RECVERR, &optval, sizeof(optval)) < 0){
		errstr = "cant IP_RECVERR due to socket errno=" + inet_oswarp_t::sock_strerror();
		goto close_socket;
	}
}	
#endif

	// start the fdwatch
	fdwatch = nipmem_new fdwatch_t();
	fdwatch->start(sock_fd, fdwatch_t::INPUT | fdwatch_t::ERROR, this, NULL);
	// mark the sock_fd as locally unused - as it is now owned by fdwatch
	sock_fd	= 0;

	// init the mtugrow_delaygen - but not the mtugrow_timeout
	mtugrow_delaygen= delaygen_t(delaygen_regular_arg_t().period(delay_t::from_sec(3*60))
								.random_range(0.2));
	// init the m_mtu_outter with the one from the kernel
	m_mtu_outter	= mtu_outter_from_kernel();

	// some logging
	KLOG_DBG("udp_full_t created " << *this);		
	// return no error
	return inet_err_t::OK;

close_socket:;	// close the sock_fd and mark it unused
		close( sock_fd );
		sock_fd	= 0;
		// return the error
		return inet_err_t(inet_err_t::SYSTEM_ERR, errstr);
}

/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
inet_err_t udp_full_t::start(udp_full_cb_t *callback, void *userptr)		throw()
{
	inet_err_t	inet_err;
	// set the callback
	inet_err = set_callback(callback, userptr);
	if( inet_err.failed() )	return inet_err;
	// start the action
	return start();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       mtu stuff
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set or clear the path mtu discovery
 */
void	udp_full_t::mtu_pathdisc(bool onoff)	throw()
{
#ifdef __linux__
	int	optval;
	// reset the timer policy
	mtugrow_delaygen.reset();

	// if the onoff is true, set the pmtudisc else remove it
	if( onoff ){
		// mark the socket as willing to do path mtu discovery
		optval = IP_PMTUDISC_DO;
		// start the mtu_growth timeout
		mtugrow_timeout.start(mtugrow_delaygen.current(), this, NULL);
	}else{
		// mark the socket as willing to do path mtu discovery
		optval = IP_PMTUDISC_DONT;
		// stop the mtu_growth timeout
		mtugrow_timeout.stop();
	}

	// mark the socket depending on the optval
	int	res = setsockopt(fdwatch->get_fd(), SOL_IP, IP_MTU_DISCOVER, &optval, sizeof(optval));
	// if an error occur, log the event
	if( res < 0 )	KLOG_ERR("cant IP_MTU_DISCOVER due to socket errno=" << inet_oswarp_t::sock_strerror());
#else
	EXP_ASSERT(0);
#endif
}

/** \brief get the current path mtu discovery status
 * 
 * @return true if the pmtudisc is currently set, false otherwise
 */
bool	udp_full_t::mtu_pathdisc()		const throw()
{
	// if the mtugrow_timeout is running, this implies the pmtudisc is enable
	if( mtugrow_timeout.is_running() )	return true;
	// else it implies the pmtudisc is disable
	return false;
}

/** \brief Return the mtu overhead taken by this layer 
 */
size_t	udp_full_t::mtu_overhead()	const throw()
{
	size_t	overhead= 0;
	// compute the value
	overhead	+= IP_HEADER_MIN_LEN;
	overhead	+= UDP_HEADER_LEN;
	// return the just computed value
	return overhead;
}
	
/** \brief callback called when the timeout_t expire
 */
bool	udp_full_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// sanity check - this point MUST NOT be reached if the mtu_pathdisc() is disable
	DBG_ASSERT( mtu_pathdisc() );
	// log to debug
	KLOG_DBG("enter");

	// backup the m_mtu_outter value
	size_t	old_mtu_outter	= m_mtu_outter;
	// update the m_mtu_outter with the current value from the kernel 
	m_mtu_outter	= mtu_outter_from_kernel();

	// log to debug
	KLOG_DBG("m_mtu_outter=" << mtu_outter() << " but was " << old_mtu_outter);
		
	// if the mtu_outter() just became bigger, reset the mtugrow_delaygen
	// - it helps discovering the actual mtu more quickly as growth typically happen in bunch
	if( mtu_outter() > old_mtu_outter )	mtugrow_delaygen.reset();

	// if the mtu_outter() has been modified, notify the 
	if( mtu_outter() != old_mtu_outter ){
		bool	tokeep = notify_callback( udp_event_t::build_mtu_change(mtu_inner()) );
		if( !tokeep )	return false;
	}	
	
	// update the timeout period
	mtugrow_timeout.change_period( mtugrow_delaygen.pre_inc() );
	// return tokeep
	return true;
}

/** \brief get the current MTU fromt the kernel
 */
size_t	udp_full_t::mtu_outter_from_kernel()					const throw()
{
#ifdef __linux__
	size_t		kernel_mtu;
	socklen_t	optlen	 = sizeof(kernel_mtu);
	// get the outter MTU of this connected socket for the whole datagram
	int	res = getsockopt(fdwatch->get_fd(), SOL_IP, IP_MTU, &kernel_mtu, &optlen);
	// if an error occurs, log the event and 	
	if( res < 0 ){
		KLOG_ERR( "cant getsockopt IP_MTU. errno=" + inet_oswarp_t::sock_strerror() );
		DBG_ASSERT( 0 );
	}
	// return the just computed MTU
	return kernel_mtu;
#else
	KLOG_ERR("_WIN32 ignored getting IP_MTU on udp_full_t. default to 1400.look at it later");
//	EXP_ASSERT(0);
	return 1400;
#endif
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			fdwatch_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief dump the error queue
 */
void	udp_full_t::purge_msg_errqueue()				throw()
{
#ifdef __linux__
	char 			error_msg[8192];
	struct sockaddr_in	in_addr;
	struct msghdr 		msg;
	struct iovec 		iov;
	ipport_addr_t		remote_addr;
	// set teh
	msg.msg_name	= &in_addr;
	msg.msg_namelen = sizeof(in_addr);
	msg.msg_iov	= &iov;
	msg.msg_iovlen	= 1;
	iov.iov_base	= error_msg;
	iov.iov_len	= sizeof(error_msg);
	msg.msg_control = error_msg;
	msg.msg_controllen = sizeof(error_msg);
	// read all the pending message 
	while ( recvmsg(fdwatch->get_fd(), &msg, MSG_ERRQUEUE) > 0 ){
		struct cmsghdr *		cmsg;
		struct sock_extended_err *	ext_err;
		/* get the remote addr (the original destination addr of the triggering packet) */
		remote_addr = ipport_addr_t(in_addr);
		// go thru all the cmsg headers
		for( cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg,cmsg) ){
			// if the cmsg header is a IP_RECVERR
			if(cmsg->cmsg_level == SOL_IP && cmsg->cmsg_type == IP_RECVERR){
				ext_err = (sock_extended_err *)CMSG_DATA(cmsg);
				// - a lot of parsing unused... but i let the code in case i need it later
				// - ext_err contains usefull info like the icmp type/code which produced
				//   the error. see ip(7) on linux for details
				// - TODO display the stuff in ascii for readability
				KLOG_ERR("udp_full_t=" << *this << " type=" << int(ext_err->ee_type)
								<< " code=" << int(ext_err->ee_code)
								<< " data=" << ext_err->ee_info);
				return;
			}
		}
	}
#else
	KLOG_ERR("purge_msgqueue ignored because i dunno how to do it in _WIN32. is it even needed.");
	//EXP_ASSERT(0);
#endif
}

/** \brief fdwatch callback
 */
bool	udp_full_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cond )	throw()
{
	uint8_t		buffer[UDP_PAYLOAD_MAX_LEN];
	int		readlen = 0;
	// some logging
	KLOG_DBG( *this << " received event " << cond << ". watching " << fdwatch->cond());

	// if the condition is input
	if( cond.is_input() ){
		// sanity check - the recevied condition must be watched
		DBG_ASSERT( fdwatch->cond().is_input() );
		// read data in the socket
#ifndef	_WIN32
		readlen = read(fdwatch->get_fd(), buffer, sizeof(buffer));
#else
		readlen	= recv(fdwatch->get_fd(), (char *)buffer, sizeof(buffer), 0);
#endif
		// back up the errno
		int errno_copy	= inet_oswarp_t::sock_errno();
		// log to debug
		if( readlen < 0 )
			KLOG_DBG("read on udp_full_t " << *this << " returned " << readlen << " and errno=" << inet_oswarp_t::sock_strerror(errno_copy));
		// if readlen < 0, and errno_copy == EAGAIN, just ignore the error 
		if( readlen < 0 && errno_copy == EAGAIN )	return true;
		// if the error is a MTU_CHANGE, trigger the mtugrow_timeout in 0sec
#ifndef _WIN32
		if( readlen < 0 && errno_copy == EMSGSIZE && mtu_pathdisc() ){
#else
		if( readlen < 0 && errno_copy == WSAEMSGSIZE && mtu_pathdisc() ){
#endif
			// purge the error queue
			// - NOTE: not sure why but linux seems to require it
			purge_msg_errqueue();
			// schedule a mtugrow_timeout to refresh the value
			mtugrow_timeout.start(delay_t(0), this, NULL);
			return true;
		}
		// if some data have been read, notify the caller
		if( readlen > 0 ){
#if 1
			// allocate the pkt_t to notify
			pkt_t	pkt;
			pkt.work_on_data_nocopy(buffer, readlen);
#else
			// TODO useless memcpy here, avoid it
			pkt_t	pkt(buffer, readlen);
#endif
			// notify the caller
			return notify_callback(udp_event_t::build_recved_data(&pkt));
		}
	}

	/* due to glib iowatch, G_IO_ERR isnt recevied for unknown reason ?!?!!?
	 * - the work around is 'if G_IO_IN is set and no data to read, treat it as G_IO_ERR'
 	 */
	// handle a connection error	
	if( cond.is_error() || (cond.is_input() && readlen < 0) ){
		int		sockopt_val;
		socklen_t	sockopt_len	= sizeof(sockopt_val);
		std::string	reason		= "undetermined";
		inet_err_t	inet_err;
		// log to debug
		KLOG_DBG("readlen=" << readlen);
		// get the error code
		inet_err	= inet_oswarp_t::getsockopt(cb_fdwatch.get_fd(), SOL_SOCKET, SO_ERROR, &sockopt_val, &sockopt_len);
		if( inet_err.succeed() )	reason = inet_oswarp_t::sock_strerror(sockopt_val);
		// purge the error queue
		// - NOTE: not sure why but linux seems to require it
		purge_msg_errqueue();
		// log to debug
		KLOG_DBG("udp_full_t " << *this << " closed due to " << reason);
		// build the event to report
		udp_event_t	udp_event = udp_event_t::build_cnx_closed(reason);
		// notify the caller
		return notify_callback( udp_event );
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       SEND function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief write data on the neoip_inet_udp_full_t
 * 
 * @return the amount of byte written (NOTE it is between >= 0 and <= to buf_len)
 *         or 0 if an error occurs
 */
size_t udp_full_t::send(const void *pkt_data, size_t pkt_len) 	throw()
{
	// log to debug
	KLOG_DBG("try to write " << pkt_len << "-byte " << *this);
	
	// write data thru the socket
#ifndef	_WIN32
	ssize_t	written_len	= write(fdwatch->get_fd(), pkt_data, pkt_len);
#else
	ssize_t	written_len	= ::send(fdwatch->get_fd(), (char *)pkt_data, pkt_len, 0);
#endif

	int	errno_copy	= inet_oswarp_t::sock_errno();;
	// log to debug
	if( written_len != (ssize_t)pkt_len )	KLOG_ERR("written_len=" << written_len << " pkt_len=" << pkt_len);

	// if the write() resulted in a EMSGSIZE "message too long", trigger the mtugrow_timeout in 0sec
#ifndef _WIN32
	if( written_len == -1 && errno_copy == EMSGSIZE && mtu_pathdisc() ){
#else
	if( written_len == -1 && errno_copy == WSAEMSGSIZE && mtu_pathdisc() ){
#endif
		// if an error occur, purge_msg_errqueue
		purge_msg_errqueue();
		// trigger the mtugrow_timeout in 0sec
		mtugrow_timeout.start(delay_t(0), this, NULL);
	}

	// test the error
	if( written_len == -1 ){
		// set the written_len as 0, as it is not the place to detect error
		// - error are notified only by udp_event_t
		written_len = 0;
		// log to debug
		KLOG_ERR("Tried to write " << pkt_len << "-byte on " << *this 
				<< " but return errno "	<< inet_oswarp_t::sock_strerror());
	}
	// return the written_len
	return written_len;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the udp_event
 */
bool udp_full_t::notify_callback(const udp_event_t &udp_event)			throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// log to debug
	KLOG_DBG("notify udp_event=" << udp_event);
	// notify the caller
	bool tokeep = callback->neoip_inet_udp_full_event_cb( userptr, *this, udp_event );
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



