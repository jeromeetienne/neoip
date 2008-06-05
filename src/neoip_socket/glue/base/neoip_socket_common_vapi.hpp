/*! \file
    \brief Declaration of the socket_common_vapi_t

\brief Brief Description
\ref socket_common_vapi_t describes the generic API to access common
within the socket. socket_common_vapi_t is used as a central point for every
socket type being able to provide commoning, aka it is not limited to 
the socket_full_t API, it is used in tcp_full_t and any other socket type
implementing the commoning.

- TODO this is halfbacked
  - no handle of the local_addr()/remote_addr()
  - this vapi is not used

*/


#ifndef __NEOIP_SOCKET_COMMON_VAPI_HPP__ 
#define __NEOIP_SOCKET_COMMON_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref socket_common_vapi_t declares all the functions which are implemented by any socket
 */
class socket_common_vapi_t {
public:
	// TODO here what about the remote_addr/local_addr
	// - someissue as it return a socket_addr_t in neoip_socket
	//   and a ipport_addr_t in neoip_tcp
	//   - well put it anyway and only emulate on tcp_full_t ?
	// - to put the send shortcut for string/datum_t/bytearray_t ?
	virtual void	rcvdata_maxlen(size_t new_rcvdata_maxlen)		throw()		= 0;
	virtual size_t	rcvdata_maxlen()					const throw()	= 0;
	virutal size_t	send(const void *data_ptr, size_t data_len) 		throw()		= 0;
	// virtual destructor
	virtual ~socket_common_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_COMMON_VAPI_HPP__  */



