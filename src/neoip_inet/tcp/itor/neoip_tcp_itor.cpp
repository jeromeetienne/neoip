/*! \file
    \brief Definition of the tcp_itor_t
    
*/

/* system include */
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_tcp_itor.hpp"
#include "neoip_tcp_itor_arg.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
tcp_itor_t::tcp_itor_t()				throw()
{
	// zero some field
	fdwatch		= NULL;		
	callback	= NULL;
}

/** \brief destructor
 */
tcp_itor_t::~tcp_itor_t()				throw()
{
	// close the fdwatch if needed
	nipmem_zdelete	fdwatch;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       start
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
tcp_itor_t &	tcp_itor_t::profile(const tcp_profile_t &p_profile)		throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == inet_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}
/** \brief Set the local address
 */
tcp_itor_t &	tcp_itor_t::local_addr(const ipport_addr_t &p_local_addr)	throw()
{
	// copy the parameter
	this->m_local_addr	= p_local_addr;
	// return the object itself
	return *this;
}

/** \brief start the action
 */
inet_err_t	tcp_itor_t::start(const ipport_addr_t &p_remote_addr
				, tcp_itor_cb_t *callback, void *userptr)	throw()
{
	struct 	sockaddr_in	addr_in;
	std::string		errstr;	
	inet_err_t		inet_err;
	
	// copy the parameter
	this->m_remote_addr	= p_remote_addr;
	this->callback		= callback;
	this->userptr		= userptr;	
	// check the parameter
	DBG_ASSERT( callback );
	DBG_ASSERT( remote_addr().is_fully_qualified() );

	// start the expire_timeout
	expire_timeout.start(profile().itor().timeout_delay(), this, NULL);
	
	// create the socket
	int	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if( sock_fd < 0 ){
		errstr = "Cant create socket. due to " + inet_oswarp_t::sock_strerror();
		goto error;
	}
	// set REUSEADDR
	inet_err	= inet_oswarp_t::set_reuseaddr(sock_fd);
	if( inet_err.failed() ){
		errstr = "setsockopt SO_REUSEADDR failed due to " + inet_err.to_string();
		goto close_socket;
	}
	
	// bind the local_addr if provided
	if( !local_addr().is_null() ){
		addr_in = local_addr().to_sockaddr_in();
		if( bind(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) ){
			errstr = "cant bind socket to " + local_addr().to_string() 
					+ " due to " + inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
	}
		
	// set this socket in non blocking for the connection
	inet_err	= inet_oswarp_t::set_nonblock(sock_fd);
	if( inet_err.failed() ){
		errstr = inet_err.to_string();
		goto close_socket;
	}

	// do the connect() itself (in non-blocking)
	addr_in = remote_addr().to_sockaddr_in();
#if 0	// TODO what is the difference between those two ?!?!?
	// - the first seems to be a debug thing which has been left over
	if(connect( sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) != -1 || errno != EINPROGRESS ){
		DBG_ASSERT( 0 );
		errstr = "cant connect() socket to " + remote_addr().to_string()
						+ " due to " + inet_oswarp_t::sock_strerror();
		goto close_socket;
	}
#else
	if( connect(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) < 0 ){
		if( inet_oswarp_t::sock_errno() != inet_oswarp_t::CONNECT_INPROGRESS ){
			errstr = "cant connect() socket to " + remote_addr().to_string()
					+ " due to " + inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
	}
#endif	

	// if the local_addr is dynamic, get the value now
	if( !local_addr().is_fully_qualified() ){
		socklen_t	namelen	= sizeof(addr_in);
		if( getsockname(sock_fd, (struct sockaddr *)&addr_in, &namelen) ){
			errstr = "cant getsockname() on socket due to due to "
						+ inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
		// convert the struct sockaddr_in into a ipport_addr_t
		m_local_addr = ipport_addr_t(addr_in);
	}
	// log to debug
	KLOG_DBG("m_local_addr=" << local_addr());
#ifndef	_WIN32	// NOTE: _WIN32 do not provide a fully qualified just after the connect()
		// - likely due to the non blocking connect
		// - but it does provide once it is connected
		// - so the local_addr is reread once connected

	// sanity check - the local_addr MUST be fully_qualified
	DBG_ASSERT( local_addr().is_fully_qualified() );
#endif
	// start the fdwatch
	fdwatch = nipmem_new fdwatch_t();
	fdwatch->start(sock_fd, fdwatch_t::OUTPUT, this, NULL);

	// log to debug
	KLOG_DBG("tcp_itor_t created " << *this );		
	// return no error
	return inet_err_t::OK;
	// handle the error case
close_socket:;	inet_oswarp_t::close_fd( sock_fd );
error:;		return inet_err_t(inet_err_t::SYSTEM_ERR, errstr);
}

/** \brief start the action (based on tcp_itor_arg_t)
 */
inet_err_t tcp_itor_t::start(const tcp_itor_arg_t &itor_arg, tcp_itor_cb_t *callback, void *userptr)	throw()
{
	// sanity check - the tcp_itor_arg_t MUST be valid
	DBG_ASSERT( itor_arg.is_valid() );
	// handle local_addr if present
	if( itor_arg.local_addr_present() )	local_addr(itor_arg.local_addr());
	// copy tcp_profile_t if present
	if( itor_arg.profile_present() )	profile(itor_arg.profile());
	// use the main start() function now
	return start(itor_arg.remote_addr(), callback, userptr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       fdwatch callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback to notify when the fdwatch_t has events to report
 */
bool	tcp_itor_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond )	throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter fdwatch_cond=" << cb_fdwatch_cond);

	// read the SO_ERROR to determine if the connect() succeed or failed
	// - in short, if socket_val == 0, the connect() is completed, else it failed
	// - from connect(2): "The socket is non-blocking and the connection cannot be completed 
	//   immediately. It is possible to select(2) or poll(2) for completion by selecting the
	//   socket for writing. After select indicates writability, use getsockopt(2) to read 
	//   the SO_ERROR option at level SOL_SOCKET to determine whether connect completed 
	//   successfully (SO_ERROR is zero) or unsuccessfully (SO_ERROR is one of the usual
	//   error codes listed here, explaining the reason for the failure)."
	int		sockopt_val	= 0;
	socklen_t	sockopt_len 	= sizeof(sockopt_val);
	inet_err	= inet_oswarp_t::getsockopt(fdwatch->get_fd(), SOL_SOCKET, SO_ERROR, &sockopt_val, &sockopt_len);
	if( inet_err.failed() || sockopt_val ){
		std::string	reason	 = inet_oswarp_t::sock_strerror(sockopt_val);
		// notify the caller
		return notify_callback( tcp_event_t::build_cnx_refused(reason) );
	}

#ifdef	_WIN32	// NOTE: _WIN32 do not provide a fully qualified just after nonblock connect()
	// if local_addr is not yet fully qualified, reread it now
	if( !local_addr().is_fully_qualified() ){
		struct 	sockaddr_in	addr_in;
		socklen_t	namelen	= sizeof(addr_in);
		if( getsockname(fdwatch->get_fd(), (struct sockaddr *)&addr_in, &namelen) ){
			KLOG_ERR( "cant getsockname() on socket due to due to "
						+ inet_oswarp_t::sock_strerror());
		}
		// convert the struct sockaddr_in into a ipport_addr_t
		m_local_addr = ipport_addr_t(addr_in);
	}
	// sanity check - the local_addr MUST be fully_qualified
	DBG_ASSERT( local_addr().is_fully_qualified() );
#endif


	// create the tcp_full_t
	tcp_full_t *	tcp_full;
	tcp_full	= nipmem_new tcp_full_t(local_addr(), remote_addr(), fdwatch);
	tcp_full->profile(profile().full());
	// mark the fdwatch as unused - DO NOT close it as it now belongs to neoip_tcp_full_t
	fdwatch		= NULL;
	// backup the object_slotid of the tcp_full_t - to be able to return its tokeep value
	// - in fact this should be a fdwatch_t object_slotid_t but as fdwatch_t ownership
	//   is transfered to tcp_full_t and as tcp_full_t dont delete fdwatch_t except if it is
	//   deleted, it works and avoid to have a object_slotid_t for every fdwatch_t
	slot_id_t	tcp_full_slotid= tcp_full->get_object_slotid();
	// notify the caller
	notify_callback( tcp_event_t::build_cnx_established(tcp_full) );
	// if the fdwatch_t has not been deleted, so 'tokeep' else return 'dontkeep'
	return object_slotid_tokeep(tcp_full_slotid);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool 	tcp_itor_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// notify the event
	std::string	reason	= "timedout after " + expire_timeout.get_period().to_string();
	return notify_callback( tcp_event_t::build_itor_timedout(reason) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	tcp_itor_t::to_string()	const throw()
{
	// handle the null case
	if( is_null() )	return "null";
	// build and return the std::string
	return local_addr().to_string() + " to " + remote_addr().to_string();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			callback function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool tcp_itor_t::notify_callback(const tcp_event_t &tcp_event)			throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_tcp_itor_event_cb( userptr, *this, tcp_event );
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



