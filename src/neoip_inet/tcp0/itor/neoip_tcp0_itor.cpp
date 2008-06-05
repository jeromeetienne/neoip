/*! \file
    \brief Definition of the tcp_itor_t
    
*/

/* system include */
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

/* local include */
#include "neoip_tcp_itor.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_tcp_full.hpp"
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

/** \brief Set the local address
 */
tcp_itor_t &	tcp_itor_t::set_local_addr(const ipport_addr_t &local_addr)		throw()
{
	// copy the parameter
	this->local_addr	= local_addr;
	// return the object itself
	return *this;
}

/** \brief start the action
 */
inet_err_t	tcp_itor_t::start(const ipport_addr_t &remote_addr
				, tcp_itor_cb_t *callback, void *userptr)	throw()
{
	int			opt_on	= 1;
	struct 	sockaddr_in	addr_in;
	std::string		errstr;	
	
	// copy the parameter
	this->remote_addr	= remote_addr;
	this->callback		= callback;
	this->userptr		= userptr;	
	// check the parameter
	DBG_ASSERT( callback );
	DBG_ASSERT( !remote_addr.is_null() );

		
	// create the socket
	int	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if( sock_fd < 0 ){
		errstr = "Cant create socket. errno=" + inet_oswarp_t::sock_strerror();
		goto error;
	}
	// set REUSEADDR
	if( setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt_on, sizeof(opt_on)) < 0 ){
		errstr = "Cant setsockopt REUSEADDR. errno=" + inet_oswarp_t::sock_strerror();
		goto close_socket;
	}

	// bind the local_addr if provided
	if( !local_addr.is_null() ){
		addr_in = local_addr.to_sockaddr_in();
		if( bind(sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) ){
			errstr = "cant bind socket to " + local_addr.to_string() 
							+ " errno=" + inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
	}
		
	// set this socket in non blocking for the connection
	if( fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0 ){
		errstr = "cant put socket in O_NONBLOCK mode";
		goto close_socket;
	}
	
	// do the connect() itself (in non-blocking)
	addr_in = remote_addr.to_sockaddr_in();
	if( connect( sock_fd, (struct sockaddr *)&addr_in, sizeof(addr_in) ) < 0 ){
		if( errno != EINPROGRESS ){
			errstr = "cant connect() socket to " + local_addr.to_string()
								+ " errno=" + inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
	}
	
	// if the local_addr is dynamic, get the value now
	if( local_addr.is_null() ){
		socklen_t	namelen	= sizeof(addr_in);
		if( getsockname( sock_fd, (struct sockaddr *)&addr_in, &namelen ) ){
			errstr = "cant getsockname() on socket due to errno=" + inet_oswarp_t::sock_strerror();
			goto close_socket;
		}
		local_addr = ipport_addr_t(addr_in);
	}

	// start the fdwatch
	fdwatch = nipmem_new fdwatch_t();
	fdwatch->start(sock_fd, fdwatch_t::OUTPUT | fdwatch_t::ERROR, this, NULL);

	// log to debug
	KLOG_DBG("tcp_itor_t created " << *this );		

	return inet_err_t::OK;
	
close_socket:;	inet_oswarp_t::close_fd( sock_fd );
error:;		return inet_err_t(inet_err_t::SYSTEM_ERR, errstr);
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
	// log to debug
	KLOG_DBG("enter fdwatch_cond=" << cb_fdwatch_cond);
	// if fdwatch.is_output() then the connection has been established
	if( cb_fdwatch_cond.is_output() ){
		// create the tcp_full_t
		tcp_full_t *	tcp_full	= nipmem_new tcp_full_t(local_addr, remote_addr, fdwatch);
		// mark the fdwatch as unused - DO NOT close it as it now belongs to neoip_tcp_full_t
		fdwatch		= NULL;
		// backup the object_slotid of the tcp_full_t - to be able to return its tokeep value
		// - in fact this should be a fdwatch_t object_slotid_t but as fdwatch_t ownership
		//   is transfered to tcp_full_t and as tcp_full_t dont delete fdwatch_t except if it is
		//   deleted, it works and avoid to have a object_slotid_t for every fdwatch_t
		slot_id_t	tcp_full_slotid	= tcp_full->get_object_slotid();
		// notify the caller
		notify_callback( tcp_event_t::build_cnx_established(tcp_full) );
		// if the fdwatch_t has not been deleted, so 'tokeep' else return 'dontkeep'
		return object_slotid_tokeep(tcp_full_slotid);
	}
	
	// if fdwatch.is_output() then the connection has been established
	if( cb_fdwatch_cond.is_error() ){
		std::string	reason	 = "undetermined";
		int		sockopt_val;
		socklen_t	sockopt_len = sizeof(sockopt_val);
		// get the error code
		if(getsockopt(cb_fdwatch.get_fd(), SOL_SOCKET, SO_ERROR, &sockopt_val, &sockopt_len)==0)
			reason	= neoip_strerror(sockopt_val);
		// notify the caller
		tcp_event_t tcp_event = tcp_event_t::build_cnx_refused(reason);
		return notify_callback(tcp_event);
	}

	// keep the fdwatch running
	return true;
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
	if( is_null() )	return "null";
	return local_addr.to_string() + " to " + remote_addr.to_string();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
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



