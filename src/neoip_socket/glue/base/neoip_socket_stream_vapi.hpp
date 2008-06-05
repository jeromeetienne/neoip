/*! \file
    \brief Declaration of the socket_stream_vapi_t

\brief Brief Description
\ref socket_stream_vapi_t describes the generic API to access stream
within the socket. socket_stream_vapi_t is used as a central point for every
socket type being able to provide streaming, aka it is not limited to 
the socket_full_t API, it is used in tcp_full_t and any other socket type
implementing the streaming.

- TODO to port elsewhere
  - in the neoip_socket/glue
      
*/


#ifndef __NEOIP_SOCKET_STREAM_VAPI_HPP__ 
#define __NEOIP_SOCKET_STREAM_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref socket_stream_vapi_t declares all the functions specific to reliable stream
 */
class socket_stream_vapi_t {
public:
	virtual void	maysend_tshold(size_t new_tshold)			throw()		= 0;
	virtual size_t	maysend_tshold()					const throw()	= 0;
	virtual void	xmitbuf_maxlen(size_t new_xmitbuf_maxlen)		throw()		= 0;
	virtual size_t	xmitbuf_maxlen()					const throw()	= 0;
	virtual size_t	xmitbuf_usedlen()					const throw()	= 0;
	virtual size_t	xmitbuf_freelen()					const throw()	= 0;
	// virtual destructor
	virtual ~socket_stream_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_STREAM_VAPI_HPP__  */



