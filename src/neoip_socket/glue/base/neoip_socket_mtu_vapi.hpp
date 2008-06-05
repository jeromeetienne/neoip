/*! \file
    \brief Declaration of the socket_mtu_vapi_t

\brief Brief Description
\ref socket_mtu_vapi_t describes the generic API to access mtu matter
within the socket. socket_mtu_vapi_t is used as a central point for every
socket type using mtu, aka it is not limited to the socket_full_t API, it is 
used in udp_full_t and any other socket type implementing the mtu.
- NOTE: this is limited to datagram socket

*/


#ifndef __NEOIP_SOCKET_MTU_VAPI_HPP__ 
#define __NEOIP_SOCKET_MTU_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief the callback class for socket_mtu_vapi_t - limited for socket_type_t::is_datagram() 
 */
class socket_mtu_vapi_t {
public:
	/*************** core function	***************************************/
	virtual void	mtu_pathdisc(bool onoff)	throw()		= 0;
	virtual bool	mtu_pathdisc()			const throw()	= 0;
	virtual size_t	mtu_overhead()			const throw()	= 0;
	virtual size_t	mtu_outter()			const throw()	= 0;
	virtual size_t	mtu_inner()			const throw()	= 0;

	// virtual destructor
	virtual ~socket_mtu_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_MTU_VAPI_HPP__  */



