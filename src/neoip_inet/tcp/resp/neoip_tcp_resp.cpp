/*! \file
    \brief Definition of the tcp_resp_t
    
*/

/* system include */
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_tcp_resp.hpp"
#include "neoip_tcp_resp_arg.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_tcp_event.hpp"
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
tcp_resp_t::tcp_resp_t()				throw()
{
	// zero some field
	fdwatch		= NULL;	
	callback	= NULL;
}

/** \brief destructor
 */
tcp_resp_t::~tcp_resp_t()				throw()
{
	// delete the fdwatch if needed
	nipmem_zdelete	fdwatch;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
tcp_resp_t &	tcp_resp_t::profile(const tcp_profile_t &p_profile)		throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == inet_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}
/** \brief start the action
 */
inet_err_t	tcp_resp_t::start(const ipport_addr_t &p_listen_addr
				, tcp_resp_cb_t *callback, void *userptr)	throw()
{
	struct 	sockaddr_in	addr_in;
	std::string		errstr;
	inet_err_t		inet_err;

	// copy some parameter
	this->m_listen_addr	= p_listen_addr;
	this->callback		= callback;
	this->userptr		= userptr;
	// sanity check - the listen_addr MUST be non null
	DBG_ASSERT( !listen_addr().is_null() );
		
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

	// bind the socket   	
	addr_in = listen_addr().to_sockaddr_in();
	if( bind(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) ){
		errstr = "cant bind socket to " + listen_addr().to_string() 
				+ " due to " + inet_oswarp_t::sock_strerror();
		goto close_socket;
	}

	// if the listen port was undefined, read the one assigned by the system
	if( listen_addr().get_port() == 0 ){
		socklen_t	addrlen = sizeof(addr_in);
		if( getsockname(sock_fd, (struct sockaddr *)&addr_in, &addrlen)){
			errstr = "cant getsocketname due to " + inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
		// convert the struct socketaddr_in into a ipport_addr_t
		m_listen_addr = ipport_addr_t(addr_in);
	}

	// set this socket in non blocking for the connection
	inet_err	= inet_oswarp_t::set_nonblock(sock_fd);
	if( inet_err.failed() ){
		errstr = inet_err.to_string();
		goto close_socket;
	}

	// put the socket in listen mode
	if( listen(sock_fd, 64) < 0 ){
		errstr = "cant cant listen on to " + listen_addr().to_string() 
					+ " due to " + inet_oswarp_t::sock_strerror();
		goto close_socket;
	}

	// some logging
	KLOG_DBG("tcp_resp_t created on " << listen_addr);		

	// start the fdwatch
	fdwatch = nipmem_new fdwatch_t();
	fdwatch->start(sock_fd, fdwatch_t::INPUT, this, NULL);
	
	// return no error
	return inet_err_t::OK;
	// handle the error case
close_socket:;	inet_oswarp_t::close_fd( sock_fd );
error:;		KLOG_INFO("errstr=" << errstr);
		return inet_err_t(inet_err_t::SYSTEM_ERR, errstr);
}

/** \brief start the action (based on tcp_resp_arg_t)
 */
inet_err_t tcp_resp_t::start(const tcp_resp_arg_t &resp_arg, tcp_resp_cb_t *callback, void *userptr)	throw()
{
	// sanity check - the tcp_resp_arg_t MUST be valid
	DBG_ASSERT( resp_arg.is_valid() );
	// copy tcp_profile_t if present
	if( resp_arg.profile_present() )	profile(resp_arg.profile());
	// use the main start() function now
	return start(resp_arg.listen_addr(), callback, userptr);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       fdwatch callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool	tcp_resp_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &fdwatch_cond )	throw()
{
	struct 	sockaddr_in	addr_in;
	inet_err_t		inet_err;
	ipport_addr_t		local_addr, remote_addr;
	// log to debug
	KLOG_DBG("enter fdwatch_cond=" << fdwatch_cond);
	// sanity check
	DBG_ASSERT( fdwatch_cond.is_input() );	

	// accept the socket
	socklen_t 	addrlen = sizeof(addr_in);
	int 		full_fd	= accept(fdwatch->get_fd(),(struct sockaddr*)&addr_in, &addrlen);
	DBG_ASSERT( full_fd >= 0 );
	
	// get the remote addr
	remote_addr	= ipport_addr_t(addr_in);

	// get the local address
	// - It is impossible to simply read the listen_addr as it may be ANY addr
	addrlen		= sizeof(addr_in);
	if( getsockname(full_fd, (struct sockaddr *)&addr_in, &addrlen) ){
		KLOG_ERR("cant getsockname() socket");
		return true;
	}
	local_addr	= ipport_addr_t(addr_in);

	// log to debug
	KLOG_DBG("received tcp connection. local_addr=" << local_addr << " remote_addr=" << remote_addr );	

	// create the fdwatch_t for tcp_full_t
	fdwatch_t *	full_fdwatch	= nipmem_new fdwatch_t();
	full_fdwatch->start(full_fd, fdwatch_t::NONE, NULL, NULL);
	// create the tcp_full_t
	tcp_full_t *	tcp_full;
	tcp_full	= nipmem_new tcp_full_t(local_addr, remote_addr, full_fdwatch);
	tcp_full->profile(profile().full());
	// notify the caller
	return notify_callback( tcp_event_t::build_cnx_established(tcp_full) );
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	tcp_resp_t::to_string()	const throw()
{
	// handle the null case
	if( is_null() )	return "null";
	// return the listen_addr_t in a string
	return listen_addr().to_string();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool tcp_resp_t::notify_callback(const tcp_event_t &tcp_event)			throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_tcp_resp_event_cb( userptr, *this, tcp_event );
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



