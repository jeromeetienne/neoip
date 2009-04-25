/*! \file
    \brief Definition of the tcp_full_t

\par BUG missing error detection when INPUT is not watched
- apparently the connection close is NOT detected IF rcvdata_maxlen(0)
- possible solution: out of the blue:
  - what about always watching input and reading 0 if rcvdata_maxlen is 0 ?
  - i think it should work but im not sure if there are other drawback

\par About error detection and recv_limit
- using the recv_limit disable watching the fdwatch_t::INPUT when the recv rate_limit_t
  is used.
- during this time it is impossible to detect an error in the connection, as a consequence
  the rate_limit_t window delay is directly proportionnal to the detection latency of the 
  error.
  - as long as the window delay is below 1second, this effect is negligible because we
    dont care about detecting an error 1second after it occurs

\par Possible Improvement - simulate a RCVLOWAT
- linux doesnt provides the bsd option RCVLOWAT
  - as consequence, as soon as a single byte is available, it is notified to the apps
- as bittorrent mainly deals with 16kbyte or more, and that usual tcp packet are around
  1500byte. the process is woke up 10 time more than necessary!!!!!
- this is likely to waste a lot of CPU
- experiment with an emulation of RCVLOWAT base on a rate_estim_t
  - see how much CPU may be gained on that
  - a very shallow experimentation seems to say "quite a lot of CPU may be gained
    on fast connection"

\par Possible Improvement - no copy write
- currently any writing on the socket will do a memcpy
  - it is likely to impact CPU performance
- to measure the CPU performance in this case
- experiment with a write directly when it is possible
  - and copy the rest in the queue when required
- see how much CPU is gained by that
- at a given point i though about the idea of avoiding a userspace buffer
  - aka use ioctl SIOCOUTQ to get the freelen in the sendbuf 

\par Possible Improvement - use tcp rtt to estimate the network coordinate
- i think it may be possible to extract the tcp latency from the TCP_INFO
  getsocketopt
  - NOTE: this is linux ONLY
  - tcpi_rtt ? SEEMS THIS ONE IS OK
    - seems not in http://groups.google.bg/group/mlist.linux.kernel/tree/browse_frm/month/2003-02?_done=%2Fgroup%2Fmlist.linux.kernel%2Fbrowse_frm%2Fmonth%2F2003-02%3F&
    - seems yes in http://heron.ucsd.edu/family/dave/tcphealth/
      - they use tp->srtt which is the tcpi_rtt
  - tcpi_recv_rtt ?
    - seems yes in http://dev.laptop.org/git.do?p=projects/linux-mm-cc;a=commitdiff;h=2babe1f6fea717c36c008c878fe095d1ca5696c1
    - but they use a special protocol, which is not tcp  
  - well experiment and see
- if so, this information may feed a network coordinate system

*/

/* system include */
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
/* local include */
#include "neoip_tcp_full.hpp"
#include "neoip_inet_oswarp.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_rate_limit.hpp"
#include "neoip_rate_limit_arg.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref tcp_full_t constant
const size_t	tcp_full_t::UNLIMITED	= tcp_full_t::UNLIMITED_VAL;
// end of constants definition

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
tcp_full_t::tcp_full_t(const ipport_addr_t &p_local_addr, const ipport_addr_t &p_remote_addr
					, fdwatch_t *fdwatch)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - check the parameters properly set
	DBG_ASSERT( fdwatch );
	DBG_ASSERT( p_local_addr.is_fully_qualified() );
	DBG_ASSERT( p_remote_addr.is_fully_qualified() );
	// copy the parameter
	this->m_local_addr	= p_local_addr;
	this->m_remote_addr	= p_remote_addr;
	this->fdwatch		= fdwatch;
	// reset the fdwatch_cond_t
	fdwatch->cond( fdwatch_t::NONE );
	// zero some field
	callback		= NULL;
	m_recv_limit		= NULL;
	m_xmit_limit		= NULL;
}

/** \brief destructor
 */
tcp_full_t::~tcp_full_t()				throw()
{
	// if there still are some data to write, try to xmit
	// - TODO this doesnt handle the xmit_limit. code it it for regularity	
	if( xmitbuf.length() ){
		// assigned this variable to avoid a compiler warning
		ssize_t	len 	= write(fdwatch->get_fd(), xmitbuf.void_ptr(), xmitbuf.length());
	}
	
	// delete the fdwatch_t if needed
	nipmem_zdelete	fdwatch;
	// delete the m_recv_limit if needed
	nipmem_zdelete	m_recv_limit;
	// delete the m_xmit_limit if needed
	nipmem_zdelete	m_xmit_limit;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
tcp_full_t &	tcp_full_t::set_callback(tcp_full_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// return the object itself
	return *this;
}


/** \brief Set the profile for this object
 */
tcp_full_t &	tcp_full_t::profile(const tcp_full_profile_t &p_profile)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == inet_err_t::OK );
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       start
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief start the action
 */
inet_err_t	tcp_full_t::start()						throw()
{
	inet_err_t	inet_err;
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( m_profile.check() == inet_err_t::OK );
	// sanity check - check the parameters properly set
	DBG_ASSERT( callback );	
	// sanity check - it MUST NOT have been already started
	DBG_ASSERT( !is_started() );

	// set default parameters from the tcp_full_profile
	m_rcvdata_maxlen	= profile().rcvdata_maxlen();
	m_xmitbuf_maxlen	= profile().xmitbuf_maxlen();
	m_maysend_tshold	= profile().maysend_tshold();
	if( profile().xmit_limit_arg().is_valid() )	xmit_limit(profile().xmit_limit_arg());
	if( profile().recv_limit_arg().is_valid() )	recv_limit(profile().recv_limit_arg());

	// set this socket in non blocking for the connection
	inet_err	= inet_oswarp_t::set_nonblock(fdwatch->get_fd());
	if( inet_err.failed() )	return inet_err;

#if 0
	// set SO_LINGER (keept just to show an example of how to put a tcp in linger)
	// - TODO unsure it is the good thing to put that ?
	// - clearly linked with the local linger
	// - i have no real understanding of what it is really doing
	//   - to understand before doing anything
	struct linger	linger_opt;
	linger_opt.l_onoff	= 1;
	linger_opt.l_linger	= 60;	// wait for 60second (or any other delay :)
	if( setsockopt(fdwatch->get_fd(), SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt)) < 0 ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant setsockopt SO_LINGER. due to " + inet_oswarp_t::sock_strerror() );
	}
#endif

#if 0	// TODO to remove - experimentation with TCP_NODELAY
	int	on = 1;
	if( setsockopt(fdwatch->get_fd(), IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "cant setsockopt SO_NODELAY. due to " + inet_oswarp_t::sock_strerror() );
	}
#endif

	// set the fdwatch to change the callback and set the condition
	fdwatch->change_callback(this, NULL);
	fdwatch->cond( fdwatch_t::ERROR );
	// if the rcvdata_maxlen is > 0, add the fdwatch_t::INPUT
	if( rcvdata_maxlen() > 0 )	fdwatch->cond( fdwatch->cond() | fdwatch_t::INPUT );
	
	// log to debug
	KLOG_DBG("tcp_full_t started " << *this);		

	// return no error
	return inet_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the tcp_full_t has been already started, false otherwise
 */
bool	tcp_full_t::is_started()		const throw()
{
	// if the fdwatch_cond_t is still NONE, it has not yet been started, return false
	if( fdwatch->cond() == fdwatch_cond_t::NONE )	return false;
	// else return true
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rate_limit_t ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief ctor/dtor for the xmit_limit
 */
void	tcp_full_t::xmit_limit(const rate_limit_arg_t &limit_arg)	throw()
{
	rate_err_t	rate_err;
	// log to debug
	KLOG_DBG("enter rate_sched=" << limit_arg.rate_sched());
	// delete m_xmit_limit if needed
	nipmem_zdelete m_xmit_limit;
	// if limit_arg.is_valid() is false, the caller mean to delete, return now
	if( !limit_arg.is_valid() )	return;
	// start the m_xmit_limit
	m_xmit_limit	= nipmem_new rate_limit_t();
	rate_err	= m_xmit_limit->start(limit_arg, this, NULL);
	DBG_ASSERT( rate_err.succeed() );	// seems poor error handling, but in fact it never fails
}

/** \brief ctor/dtor for the recv_limit
 */
void	tcp_full_t::recv_limit(const rate_limit_arg_t &limit_arg)	throw()
{
	rate_err_t	rate_err;
	// log to debug
	KLOG_DBG("enter rate_sched=" << limit_arg.rate_sched());
	// delete m_recv_limit if needed
	nipmem_zdelete m_recv_limit;
	// if limit_arg.is_valid() is false, the caller mean to delete, return now
	if( !limit_arg.is_valid() )	return;
	// start the m_recv_limit
	m_recv_limit	= nipmem_new rate_limit_t();
	rate_err	= m_recv_limit->start(limit_arg, this, NULL);
	DBG_ASSERT( rate_err.succeed() );	// seems poor error handling, but in fact it never fails
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       rcvdata_maxlen
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the maximum number of bytes notified by RECVED_DATA event
 * 
 * @param recv_max_len the maximum number of byte. if == 0, no input will be delivered
 */
void	tcp_full_t::rcvdata_maxlen(size_t new_rcvdata_maxlen) throw()
{
	// copy the parameter
	m_rcvdata_maxlen	= new_rcvdata_maxlen;

	// DONT watch fdwatch_t::INPUT if rcvdata_maxlen() is 0 or m_xmit_limit is set and blocked
	if( rcvdata_maxlen() == 0 || (m_recv_limit && m_recv_limit->is_used()) ){
		fdwatch->cond( fdwatch->cond() & ~fdwatch_t::INPUT );
	}else{	// DO do watch the fdwatch_t::INPUT in all other cases
		fdwatch->cond( fdwatch->cond() | fdwatch_t::INPUT );
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       maysend_tshold get/set
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the maysend_threshold
 */
void	tcp_full_t::maysend_tshold(size_t new_maysend_tshold)		throw()
{
	m_maysend_tshold	= new_maysend_tshold;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OUTQUEUE_MAX_LEN get/set
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the xmitbuf_max_len
 * 
 * @param xmitbuf_max_len the new xmitbuf maximum length. 
 *                        WARNING: It MUST NOT be less than the current used length.
 */
void	tcp_full_t::xmitbuf_maxlen(size_t new_xmitbuf_maxlen)		throw()
{
	// copy the parameter
	m_xmitbuf_maxlen	= new_xmitbuf_maxlen;
	// sanity check - the cur xmitbuf_maxlen MUST NOT be >= the used length
	if( xmitbuf_maxlen() != UNLIMITED )	DBG_ASSERT( xmitbuf.length() <= xmitbuf_maxlen() );
}

/** \brief return the used length of the xmitbuf
 */
size_t tcp_full_t::xmitbuf_usedlen() const throw()
{
	// sanity check - the cur xmitbuf_maxlen MUST NOT be >= the used length
	if( xmitbuf_maxlen() != UNLIMITED )	DBG_ASSERT( xmitbuf.length() <= xmitbuf_maxlen() );
	// return the current size
	return xmitbuf.length();
}

/** \brief return the amount of free space in the xmitbuf
 * 
 * @return the amount of free space in the xmitbuf. if the xmitbuf is not limited, return -1
 */
size_t tcp_full_t::xmitbuf_freelen()	const throw()
{
	// if the xmitbuf_maxlen is unlimited, the freelen is unlimited too
	if( xmitbuf_maxlen() == UNLIMITED )	return UNLIMITED;
	// if it is not unlimited, return the difference between the usedlen and the maxlen
	return xmitbuf_maxlen() - xmitbuf_usedlen();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       fdwatch_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool	tcp_full_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cond )	throw()
{
	// some logging
	KLOG_DBG( *this << " received event " << cond << ". watching " << fdwatch->cond());

	// if the condition is input
	if( cond.is_input() ){
		// sanity check - the fdwatch_t::INPUT condition must be watched
		DBG_ASSERT( fdwatch->cond().is_input() );
		// handle the input
		bool	tokeep	= try_to_recv();
		if( !tokeep )	return false;
	}

	// if the condition is output
	if( cond.is_output() ){
		// sanity check - the fdwatch_t::OUTPUT condition must be watched
		DBG_ASSERT( fdwatch->cond().is_output() );
		// handle the output
		bool	tokeep	= try_to_xmit();
		if( !tokeep )	return false;
	}

	// handle a connection error
	if( cond.is_error() )	return handle_error();

	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Try to read the socket fd with as much data as possible
 * 
 * - if m_recv_limit is set and if some data have been read, m_recv_limit will be blocked
 *   after this.
 * 
 * @return a tokeep for the tcp_full_t
 */
bool	tcp_full_t::try_to_recv()						throw()
{
	size_t			buf_len;
	rate_limit_tmp_t	rate_tmp;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - rcvdata_maxlen() MUST NOT be 0 - as it mean no input to be delivered
	DBG_ASSERT( rcvdata_maxlen() > 0 );
	// sanity check - m_recv_limit is set, it MUST NOT be blocked
	if( m_recv_limit )	DBG_ASSERT( !recv_limit().is_used() );
	// compute the buf_len
	if( m_recv_limit )	buf_len	= m_recv_limit->data_request(rcvdata_maxlen(), rate_tmp);
	else			buf_len	= rcvdata_maxlen();
		
	// allocate the buffer to store read data - in stack for better speed
	void *	buf_ptr	= nipmem_alloca(buf_len);
	// read data in the socket
	// - if readlen == -1 and errno == EAGAIN, there is nothing to read but socket is ok
	// - if readlen == 0, the socket is closed
#ifndef	_WIN32	// TODO would be good to enable only the recv... nothing require to use read()
	int	readlen	= read(fdwatch->get_fd(), buf_ptr, buf_len);
#else
	int	readlen	= recv(fdwatch->get_fd(), (char *)buf_ptr, buf_len, 0);
#endif

	// TODO to sort out, this is all the error handling horror
	// - currently if readlen == 0, it close.... this was the behaviour of the 
	//   previous implementation
	// - but does this cause trouble to the rate_sched_t with recv_limit_cb
	//   calling this function EVEN if the fd is not known as readable ?
	// - i dunno
	// - read is returning -1 and EAGAIN is nothing is to be read (from man page)
	// - read return == 0 if an error occurs ? i cant find any good reference for that
	int	errno_copy	= inet_oswarp_t::sock_errno();
	// TODO some issue with how to detect the socket close
	// - see http://tangentsoft.net/wskfaq/articles/bsd-compatibility.html
	// if readlen == 0, treat it as error
#ifndef _WIN32
	if( readlen == 0 || (readlen < 0 && errno_copy != EAGAIN) )
		return handle_error();
#else
	if( readlen == 0 || (readlen < 0 && errno_copy != WSAEWOULDBLOCK) )
		return handle_error();
#endif
	// if some data have been read, notify the caller
	if( readlen > 0 ){
		// notify the m_recv_limit of the readlen
		if( m_recv_limit )	m_recv_limit->data_notify(readlen, rate_tmp);			
		// allocate the pkt_t to notify
		pkt_t	pkt;
		pkt.work_on_data_nocopy(buf_ptr, readlen);
		// notify the caller
		bool	tokeep	= notify_callback( tcp_event_t::build_recved_data(&pkt) );
		if( !tokeep )	return false;
	}

	// DONT watch fdwatch_t::INPUT if rcvdata_maxlen() is 0 or m_xmit_limit is set and blocked
	if( rcvdata_maxlen() == 0 || (m_recv_limit && m_recv_limit->is_used()) ){
		fdwatch->cond( fdwatch->cond() & ~fdwatch_t::INPUT );
	}else{	// DO do watch the fdwatch_t::INPUT in all other cases
		fdwatch->cond( fdwatch->cond() | fdwatch_t::INPUT );
	}

	// return true
	return true;
}

/** \brief Handle write on the socket fd with as much data as possible
 * 
 * @return a tokeep for the tcp_full_t
 */
bool	tcp_full_t::try_to_xmit()						throw()
{
	size_t			old_freelen	= xmitbuf_freelen();
	// log to debug
	KLOG_DBG("enter");
	// sanity check - m_recv_limit is set, it MUST NOT be blocked
	if( m_xmit_limit )	DBG_ASSERT( !xmit_limit().is_used() );

	// if there are some data to write, try to xmit	
	if( xmitbuf.length() ){
		size_t			buf_len;
		rate_limit_tmp_t	rate_tmp;
		// compute the buf_len
		if( m_xmit_limit )	buf_len	= m_xmit_limit->data_request(xmitbuf.length(), rate_tmp);
		else			buf_len	= xmitbuf.length();
	
		// try to write the whole xmitbuf in the socket
#ifndef	_WIN32
		ssize_t	written_len	= write(fdwatch->get_fd(), xmitbuf.void_ptr(), buf_len);
#else
		ssize_t	written_len	= ::send(fdwatch->get_fd(), xmitbuf.char_ptr(), buf_len, 0);
#endif		
		// if the write failed, notify the caller of a CNX_CLOSED
		int	errno_copy	= inet_oswarp_t::sock_errno();
		if( written_len < 0 ){
#ifndef _WIN32
			if( errno_copy != EAGAIN )		return handle_error();
#else
			if( errno_copy != WSAEWOULDBLOCK )	return handle_error();
#endif
			// NOTE: if it is a EAGAIN, act as if written_len == 0
			written_len	= 0;
		}
		// log to debug
		KLOG_DBG("written_len="<< written_len);
		// remove all the written data from the xmitbuf
		xmitbuf.head_free( written_len );
		// notify the m_xmit_limit of the written_len
		if( m_xmit_limit )	m_xmit_limit->data_notify(written_len, rate_tmp);
	}
	// DONT watch fdwatch_t::OUTPUT if nomore data to write or if m_xmit_limit is set and blocked
	if( xmitbuf_usedlen() == 0 || (m_xmit_limit && m_xmit_limit->is_used()) ){
		fdwatch->cond( fdwatch->cond() & ~fdwatch_t::OUTPUT );
	}else{	// DO watch the fdwatch_t::OUTPUT in all other cases
		fdwatch->cond( fdwatch->cond() | fdwatch_t::OUTPUT );
	}
	
	// if maysend is set and the xmitbuf used length just became
	// larger than or equal to maysend_threshold, notify a MAYSEND_ON
	if( maysend_tshold() != UNLIMITED && old_freelen < maysend_tshold()
					&& xmitbuf_freelen() >= maysend_tshold() ){
		bool	tokeep	= notify_callback( tcp_event_t::build_maysend_on() );
		if( !tokeep )	return false;
	}
	
	// return tokeep
	return true;
}

/** \brief Handle the error
 * 
 * @return a tokeep for the tcp_full_t
 */
bool	tcp_full_t::handle_error()						throw()
{
	int		sockopt_val	= 0;
	socklen_t	sockopt_len 	= sizeof(sockopt_val);
	std::string	reason	 	= "undetermined";
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");

	// get the error code from errno - if available
	inet_err	= inet_oswarp_t::getsockopt(fdwatch->get_fd(), SOL_SOCKET, SO_ERROR, &sockopt_val, &sockopt_len);
	if( inet_err.failed() )	KLOG_INFO("can't getsockopt SO_ERROR due to " + inet_err.to_string());

	// read the error if any
	if( sockopt_val )	reason = inet_oswarp_t::sock_strerror(sockopt_val);
	// notify the caller
	return notify_callback( tcp_event_t::build_cnx_closed(reason) );	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rate_limit_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rate_limit_t becomes unblocked
 */
bool	tcp_full_t::neoip_rate_limit_cb(void *cb_userptr, rate_limit_t &cb_rate_limit)	throw()
{
	// forward the callback to the proper rate_limit_t
	if( m_recv_limit == &cb_rate_limit )	return recv_limit_cb(cb_userptr, cb_rate_limit);
	if( m_xmit_limit == &cb_rate_limit )	return xmit_limit_cb(cb_userptr, cb_rate_limit);
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return false;	// only to avoid compiler warning
}

/** \brief callback notified by \ref rate_limit_t becomes unblocked
 */
bool	tcp_full_t::recv_limit_cb(void *userptr, rate_limit_t &cb_rate_limit)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the fdwatch_t::INPUT condition MUST NOT be watched
	DBG_ASSERT( !fdwatch->cond().is_input() );
	// sanity check - m_recv_limit MUST be set
	DBG_ASSERT( m_recv_limit );
	
	// if rcvdata_maxlen() is 0, return now
	// - NOTE: this case may happen, if the rcvdata_maxlen() is zeroed during a blocked period
	if( rcvdata_maxlen() == 0 )	return true;
	
	// handle the input
	bool	tokeep	= try_to_recv();
	if( !tokeep )	return false;
	// return tokeep
	return true;
}

/** \brief callback notified by \ref rate_limit_t becomes unblocked
 */
bool	tcp_full_t::xmit_limit_cb(void *userptr, rate_limit_t &cb_rate_limit)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the fdwatch_t::OUTPUT condition MUST NOT be watched
	DBG_ASSERT( !fdwatch->cond().is_output() );
	// sanity check - m_xmit_limit MUST be set
	DBG_ASSERT( m_xmit_limit );
	// handle the output
	bool	tokeep	= try_to_xmit();
	if( !tokeep )	return false;
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			SEND function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief write data on the tcp_full_t
 * 
 * @return the amount of byte written (NOTE it is between >= 0 and <= to data_len)
 *         or 0 if an error occurs
 */
size_t tcp_full_t::send(const void *data_ptr, size_t data_len) 	throw()
{
	size_t	sendlen	= data_len;
	// log to debug
	KLOG_DBG("enter datalen=" << data_len);
	KLOG_DBG("enter data=" << datum_t(data_ptr, data_len, datum_t::NOCOPY));

	// if xmitbuf_maxlen is set, clamp sendlen with it
	if( xmitbuf_maxlen() != UNLIMITED )
		sendlen = std::min(sendlen, xmitbuf_maxlen() - xmitbuf_usedlen());
	
	// if sendlen == 0, return now
	if( sendlen == 0 )	return sendlen;
	
	// add the data to the queue
	// - NOTE: this does a memory copy
	// - TODO i think i can avoid this one
	//   - some issue with the error notification which have to be done asynchronously
	//   - how to do that ? the fdwatch may report a lot of error before the 
	//     error is notified...
	//   - nevertheless avoiding this memcpy could be a huge performance gain
	//   - so it is to be done
	//   - in case of error, input and output are notified ? i think so, find some
	//     authoritative answer
	//   - maybe the notification may be omited in case of a direct write
	//     and handled only in case of input/output async
	// - it can use sync write IIF xmitbuf.empty()
	// - just write a different function which is called 
	//   IIF xmitbuf.empty && !(m_xmit_limit && m_xmit_limit->is_used())
	//   - and dont do any user notification simply return 0
	xmitbuf.append(data_ptr, sendlen);
	
	// if m_xmit_limit is set and blocked, return now
	if( m_xmit_limit && m_xmit_limit->is_used() )	return sendlen;
	// start watched the output - if this point is reached
	fdwatch->cond( fdwatch->cond() | fdwatch_t::OUTPUT );
	// return the sendlen
	return sendlen;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	tcp_full_t::to_string()	const throw()
{
	// handle the null case
	if( is_null() )		return "null";
	// build the std::string to return
	return local_addr().to_string() + " to " + remote_addr().to_string();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool	tcp_full_t::notify_callback(const tcp_event_t &tcp_event)		throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_tcp_full_event_cb(userptr, *this, tcp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



