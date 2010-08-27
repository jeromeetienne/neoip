/*! \file
    \brief Declaration of the inet_oswarp_T

- WARNING: this .hpp MUST NOT be included in another .hpp
  - it is because it include windows include which got the bad habit of doing
    #define on many things, and so change the code after the #include
  - see the issue with the #define on ERROR, INFINITE, OPTIONAL
    
*/


#ifndef __NEOIP_INET_OSWARP_HPP__ 
#define __NEOIP_INET_OSWARP_HPP__ 
/* system include */
#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	undef ERROR		// remove dirty define from mingw
#	undef INFINITE
#	undef OPTIONAL
#else
#	include <netdb.h>
#	include <sys/types.h>
#	include <sys/socket.h>
#endif
/* local include */
#include "neoip_errno.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


// list of forward declaration
class	inet_err_t;

/** \brief helper to do inet operation which depends on the operating system
 */
class	inet_oswarp_t {
public:
	static const	int	CONNECT_INPROGRESS;
	static inet_err_t	setsockopt(int sock_fd, int level, int optname
					, const void *optval, socklen_t optlen)	throw();
	static inet_err_t	getsockopt(int sock_fd, int level, int optname
					, void *optval, socklen_t *optlen)	throw();
	static inet_err_t	set_reuseaddr(int sock_fd, bool on = true)	throw();
	static inet_err_t	set_reuseport(int sock_fd, bool on = true)	throw();
	static inet_err_t	set_nonblock(int sock_fd)			throw();
	static inet_err_t	inet_aton(const char *name_str
						, struct in_addr *inaddr_dest)	throw();

	static void		close_fd(int fd)				throw();

	/*************** errno stuff	***************************************/						
	static int		sock_errno()					throw();
	static std::string	sock_strerror(int my_errno)			throw();
	static std::string	sock_strerror()	throw()	{ return sock_strerror(sock_errno());	}
};
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_INET_OSWARP_HPP__  */



