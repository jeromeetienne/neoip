/*! \file
    \brief Definition of the \ref inet_oswarp_t class
    

- NOTE: good info on the difference between win32 and bsd socket
  - http://tangentsoft.net/wskfaq/articles/bsd-compatibility.html
  - http://tangentsoft.net/wskfaq/
*/

/* system include */
#include <fcntl.h>
#ifndef _WIN32
#	include <netinet/in.h>
#	include <arpa/inet.h>
#endif
/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

#ifndef _WIN32
	const int	inet_oswarp_t::CONNECT_INPROGRESS	= EINPROGRESS;
#else
	const int	inet_oswarp_t::CONNECT_INPROGRESS	= WSAEWOULDBLOCK;
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Do a getsockopt
 * 
 * - NOTE: required because winsock getsockopt accept only a (char *) and not a 
 *   (void *)
 */
inet_err_t	inet_oswarp_t::getsockopt(int sock_fd, int level, int optname
				, void *optval, socklen_t *optlen)		throw()
{
#ifndef _WIN32
	if( ::getsockopt(sock_fd, level, optname, optval, optlen) < 0 ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, neoip_strerror(errno));
	}
#else
	if( ::getsockopt(sock_fd, level, optname, (char *)optval, optlen) < 0 ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "getsockopt failed");
	}
#endif
	// return noerror
	return inet_err_t::OK;
}

/** \brief Do a setsockopt
 * 
 * - NOTE: required because winsock setsockopt accept only a (char *) and not a 
 *   (void *)
 */
inet_err_t	inet_oswarp_t::setsockopt(int sock_fd, int level, int optname
				, const void *optval, socklen_t optlen)		throw()
{
#ifndef _WIN32
	if( ::setsockopt(sock_fd, level, optname, optval, optlen) < 0 ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "setsocktopt failed due to " + neoip_strerror(errno));
	}
#else
	if( ::setsockopt(sock_fd, level, optname, (char *)optval, optlen) < 0 ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "setsocktopt failed");
	}
#endif
	// return noerror
	return inet_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set the sock_fd into SO_REUSEADDR
 */
inet_err_t	inet_oswarp_t::set_reuseaddr(int sock_fd, bool on)		throw()
{
	inet_err_t	inet_err;
	int		opt_on	= on ? 1 : 0;
#ifndef __APPLE__
	// set REUSEADDR - to allow the socket to bind an address already bound
	inet_err= inet_oswarp_t::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt_on, sizeof(opt_on));
	if( inet_err.failed() )	return inet_err;
#else
	// set REUSEPORT - to allow the socket to bind an address already bound
	// - this is the same as SO_REUSEPORT but for macos (and bsd ?)
	inet_err= inet_oswarp_t::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt_on, sizeof(opt_on));
	if( inet_err.failed() )	return inet_err;
#endif
	// return noerror
	return inet_err_t::OK;
}

/** \brief set the sock_fd into nonblock mode
 */
inet_err_t	inet_oswarp_t::set_nonblock(int sock_fd)			throw()
{
#ifndef _WIN32
	if( fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0 ){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "Cant set socket in nonblock due to " + neoip_strerror(errno));
	}
#else
	unsigned long flags = 1;
	if( ioctlsocket(sock_fd, FIONBIO, &flags) == SOCKET_ERROR){
		return inet_err_t(inet_err_t::SYSTEM_ERR, "Cant set socket in nonblock due to " + neoip_strerror(WSAGetLastError()));
	}
#endif
	// return noerror
	return inet_err_t::OK;
}

/** \brief inet_aton converts the Internet host address cp from the standard numbers-and-dots 
 *         notation into binary  data.
 * 
 * - NOTE: this is recoded here because the mingw32 doesnt provide it
 *   - so it is emulated with a inet_addr and a special case to fix the inet_addr 
 *     bug on "255.255.255.255"
 */
inet_err_t inet_oswarp_t::inet_aton(const char *name_str, struct in_addr *inaddr_dest) throw()
{
	// log to debug
	KLOG_DBG("enter name_str=" << name_str);
#if 0	// TODO maouaoua no gethostbyname you morron
	// - this would do a dns request if name_str is a string
	// - this is inefficient
	// - this is actually a BUG because this is used to determine if the a name_str
	//   is a dotted ip, and gethostbyname is blocking!!!
	// - see below for an emulation with inet_addr
	// - this is kept 'just in case' e.g. to test if the alternative is ok 
	//   on win32
	struct hostent *entry;
	// do a gethostbyname on the name_ed
	entry	= gethostbyname(name_str);
	// if entry is null, return an error
	if (!entry) return inet_err_t(inet_err_t::ERROR, "inet_aton failed on (" + std::string(name_str) + ") as it is an invalid name");
	// copy the result
	memcpy((char *) inaddr_dest, entry->h_addr_list[0], sizeof(*inaddr_dest));
#else
	// emulation using inet_addr

	// handle the special case of "255.255.255.255" because this is uint32_t(-1)
	// and this is the inet_addr error code INADDR_NONE too, aka conflict 
	// between the 2 
	if( !strcmp(name_str, "255.255.255.255") ){
		inaddr_dest->s_addr = 0xFFFFFFFFu;
		return inet_err_t::OK;
	}
	// call the inet_addr
	uint32_t	v4_addr	= inet_addr(name_str);
	// if inet_addr failed, return an error
	if(v4_addr == INADDR_NONE)
		return inet_err_t(inet_err_t::ERROR, "inet_aton failed on (" + std::string(name_str) + ") as it is an invalid name");
	// copy the result in the inaddr_dest
	inaddr_dest->s_addr = v4_addr;
#endif
	// return noerror
	return inet_err_t::OK;
}


/** \brief Close a socket file descriptor
 */
void	inet_oswarp_t::close_fd(int fd)				throw()
{
#ifndef _WIN32
	int	err	= close(fd);
	if( err )	KLOG_ERR("Cant close fd " << fd << " due to " << sock_strerror());
#else
	int	err	= closesocket(fd);
	if( err == SOCKET_ERROR )
		KLOG_ERR("Cant close fd " << fd << " due to " << sock_strerror());
#endif
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			errno stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef	_WIN32

/** \brief Implement the error message for winsock
 * 
 * - adapted from http://tangentsoft.net/wskfaq/examples/basics/ws-util.cpp
 *   which is public domain
 */
struct winsock_errmsg_t {
private:
	int		m_errno;
	std::string	m_message;
public:
	/*************** ctor/dtor	***************************************/
	winsock_errmsg_t(int p_errno, const std::string &p_message) 	throw()
			: m_errno(p_errno), m_message(p_message)	{}
	/*************** query function	***************************************/
	int			get_errno()	const throw()	{ return m_errno;	}
	const std::string &	message()	const throw()	{ return m_message;	}
};
//! define the list of all winsock error message
static winsock_errmsg_t winsock_errmsg_list[] = {
	winsock_errmsg_t(0,                  "No error"),
	winsock_errmsg_t(WSAEINTR,           "Interrupted system call"),
	winsock_errmsg_t(WSAEBADF,           "Bad file number"),
	winsock_errmsg_t(WSAEACCES,          "Permission denied"),
	winsock_errmsg_t(WSAEFAULT,          "Bad address"),
	winsock_errmsg_t(WSAEINVAL,          "Invalid argument"),
	winsock_errmsg_t(WSAEMFILE,          "Too many open sockets"),
	winsock_errmsg_t(WSAEWOULDBLOCK,     "Operation would block"),
	winsock_errmsg_t(WSAEINPROGRESS,     "Operation now in progress"),
	winsock_errmsg_t(WSAEALREADY,        "Operation already in progress"),
	winsock_errmsg_t(WSAENOTSOCK,        "Socket operation on non-socket"),
	winsock_errmsg_t(WSAEDESTADDRREQ,    "Destination address required"),
	winsock_errmsg_t(WSAEMSGSIZE,        "Message too long"),
	winsock_errmsg_t(WSAEPROTOTYPE,      "Protocol wrong type for socket"),
	winsock_errmsg_t(WSAENOPROTOOPT,     "Bad protocol option"),
	winsock_errmsg_t(WSAEPROTONOSUPPORT, "Protocol not supported"),
	winsock_errmsg_t(WSAESOCKTNOSUPPORT, "Socket type not supported"),
	winsock_errmsg_t(WSAEOPNOTSUPP,      "Operation not supported on socket"),
	winsock_errmsg_t(WSAEPFNOSUPPORT,    "Protocol family not supported"),
	winsock_errmsg_t(WSAEAFNOSUPPORT,    "Address family not supported"),
	winsock_errmsg_t(WSAEADDRINUSE,      "Address already in use"),
	winsock_errmsg_t(WSAEADDRNOTAVAIL,   "Can't assign requested address"),
	winsock_errmsg_t(WSAENETDOWN,        "Network is down"),
	winsock_errmsg_t(WSAENETUNREACH,     "Network is unreachable"),
	winsock_errmsg_t(WSAENETRESET,       "Net connection reset"),
	winsock_errmsg_t(WSAECONNABORTED,    "Software caused connection abort"),
	winsock_errmsg_t(WSAECONNRESET,      "Connection reset by peer"),
	winsock_errmsg_t(WSAENOBUFS,         "No buffer space available"),
	winsock_errmsg_t(WSAEISCONN,         "Socket is already connected"),
	winsock_errmsg_t(WSAENOTCONN,        "Socket is not connected"),
	winsock_errmsg_t(WSAESHUTDOWN,       "Can't send after socket shutdown"),
	winsock_errmsg_t(WSAETOOMANYREFS,    "Too many references, can't splice"),
	winsock_errmsg_t(WSAETIMEDOUT,       "Connection timed out"),
	winsock_errmsg_t(WSAECONNREFUSED,    "Connection refused"),
	winsock_errmsg_t(WSAELOOP,           "Too many levels of symbolic links"),
	winsock_errmsg_t(WSAENAMETOOLONG,    "File name too long"),
	winsock_errmsg_t(WSAEHOSTDOWN,       "Host is down"),
	winsock_errmsg_t(WSAEHOSTUNREACH,    "No route to host"),
	winsock_errmsg_t(WSAENOTEMPTY,       "Directory not empty"),
	winsock_errmsg_t(WSAEPROCLIM,        "Too many processes"),
	winsock_errmsg_t(WSAEUSERS,          "Too many users"),
	winsock_errmsg_t(WSAEDQUOT,          "Disc quota exceeded"),
	winsock_errmsg_t(WSAESTALE,          "Stale NFS file handle"),
	winsock_errmsg_t(WSAEREMOTE,         "Too many levels of remote in path"),
	winsock_errmsg_t(WSASYSNOTREADY,     "Network system is unavailable"),
	winsock_errmsg_t(WSAVERNOTSUPPORTED, "Winsock version out of range"),
	winsock_errmsg_t(WSANOTINITIALISED,  "WSAStartup not yet called"),
	winsock_errmsg_t(WSAEDISCON,         "Graceful shutdown in progress"),
	winsock_errmsg_t(WSAHOST_NOT_FOUND,  "Host not found"),
	winsock_errmsg_t(WSANO_DATA,         "No host data of that type was found")
};

/** \brief Implement a custom strerror() for winsick
 */
static std::string winsock_strerror(int my_errno)	throw()
{
	size_t	nb_item	= sizeof(winsock_errmsg_list) / sizeof(winsock_errmsg_t);
	// go thru the whole winsock_errmsg_list
	for(size_t i = 0; i < nb_item; i++ ){
		const winsock_errmsg_t &	errmsg	= winsock_errmsg_list[i];
		// if this winsock_errmsg_t matches, return its messages
		if( errmsg.get_errno() == my_errno )	return errmsg.message();
	}
	// if no matching errno has been found, return a string with the errno value
	std::ostringstream	oss;
	oss << "Unknown winsock errno(" << my_errno << ")";
	return oss.str();
}
#endif

/** \brief Return the errno for the socket
 */
int	inet_oswarp_t::sock_errno()					throw()
{
#ifndef _WIN32
	return errno;
#else
	return WSAGetLastError();
#endif
}

/** \brief Return a string describing the error of the errno
 */
std::string	inet_oswarp_t::sock_strerror(int my_errno)		throw()
{
#ifndef	_WIN32
	return neoip_strerror(my_errno);
#else
	return winsock_strerror(my_errno);
#endif
}

NEOIP_NAMESPACE_END


