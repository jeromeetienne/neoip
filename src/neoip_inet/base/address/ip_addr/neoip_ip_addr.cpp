/*! \file
    \brief Class to handle the ip address

\par Limitation
- dont handle the hostname
- dont handle IPv6 address

*/

/* system include */
#ifndef _WIN32
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#endif
/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_ip_netaddr.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the ip_addr_t class from a string
 */
ip_addr_t::ip_addr_t(const char *ip_addr_str)		throw()
{
	if( from_string(ip_addr_str) ){
		KLOG_DBG("ip_addr_t constructor cant parse " << ip_addr_str << " address. nullifying!");
		//DBGNET_ASSERT( 0 );
		nullify();
	}
}

/** \brief constructor of the ip_addr_t class from a string
 */
ip_addr_t::ip_addr_t(const std::string &ip_addr_str)		throw()
{
	if( from_string(ip_addr_str.c_str()) ){
		KLOG_DBG("ip_addr_t constructor cant parse " << ip_addr_str << " address. nullifying!");
		//DBGNET_ASSERT( 0 );
		nullify();
	}
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  from/to string
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the ip_addr_t from a string
 * 
 * @return false if no error occured, true otherwise
 */
bool ip_addr_t::from_string(const std::string &ip_addr_str) throw()
{
	struct	in_addr	inaddr;
	inet_err_t	inet_err;
	// do the inet_aton
	inet_err	= inet_oswarp_t::inet_aton(ip_addr_str.c_str(), &inaddr);
	if( inet_err.failed() )	return true;
	// set the object
	*this = ip_addr_t(ntohl(inaddr.s_addr));
	return false;
}

/** \brief convert the address to a string
 */
std::string	ip_addr_t::to_string()				const throw()
{
	if( is_null() )	return "null";
	// sanity check - currently only handle ipv4
	DBG_ASSERT( is_v4() );
	// convert the address into a string		
	struct in_addr	inaddr;
	inaddr.s_addr = htonl(get_v4_addr());
	// put the string into the ostream
	return inet_ntoa( inaddr );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		 COMPARISON OPERTATOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ip_addr_t and return value ala strcmp/memcmp
 */
int	ip_addr_t::compare( const ip_addr_t &other )	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are not null
	
	// handle only v4 address
	DBG_ASSERT( is_v4() && other.is_v4() );
	// handle the address
	if( get_v4_addr() < other.get_v4_addr() )	return -1;
	if( get_v4_addr() > other.get_v4_addr() )	return +1;
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  	sockaddr_in
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert a ip_addr_t to a sockaddr_in
 */
ip_addr_t::ip_addr_t(const struct sockaddr_in &sa_in)		throw()
{
	// sanity check - currently only handle ipv4
	DBG_ASSERT( sa_in.sin_family == AF_INET );		
	// convert the sockaddr_in
	version		= 4;
	address.v4	= ntohl(sa_in.sin_addr.s_addr);
}

/** \brief convert a ip_addr_t to a sockaddr_in
 */
struct 	sockaddr_in ip_addr_t::to_sockaddr_in()				const throw()
{
	struct sockaddr_in sa_in;
	// sanity check - currently only handle ipv4
	DBG_ASSERT( is_v4() );
	// zero the struct
	memset( &sa_in, 0, sizeof(sa_in) );
	// set the struct
	sa_in.sin_family	= AF_INET;
	sa_in.sin_addr.s_addr	= htonl( address.v4 );
	// return the built object
	return sa_in;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                      arithmetic operation
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Overload the addition
 */
ip_addr_t	ip_addr_t::operator+ (const uint32_t val)	const throw()
{
	// sanity check - currently only handle ipv4	
	DBG_ASSERT( is_v4() );
	// return the result
	return ip_addr_t( get_v4_addr() + val );
}

/** \brief Overload the substraction
 */
ip_addr_t	ip_addr_t::operator- (const uint32_t val)	const throw()
{
	// sanity check - currently only handle ipv4	
	DBG_ASSERT( is_v4() );
	// return the result
	return ip_addr_t( get_v4_addr() - val );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       ip address type 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the ip_addr is localhost
 */
bool ip_addr_t::is_localhost() const throw()
{
	// if it is_null(), return false now
	if( is_null() )		return false;
	// sanity check - currently only handle ipv4
	DBG_ASSERT( is_v4() );
	// test against localhost network address
	if( ip_netaddr_t("127.0.0.0", 8).contain(*this) )	return true;
	return false;
}


/** \brief return true is the ip_addr is linklocal - as in rfc3927.2.1
 */
bool ip_addr_t::is_linklocal() const throw()
{
	// if it is_null(), return false now
	if( is_null() )		return false;
	// sanity check - currently only handle ipv4
	DBG_ASSERT( is_v4() );
	// test against linklocal network address
	if( ip_netaddr_t("169.254.0.0", 16).contain(*this) )	return true;
	return false;
}

/** \brief return true is the ip_addr is private - as in rfc1918.3
 */
bool ip_addr_t::is_private() const throw()
{
	// if it is_null(), return false now
	if( is_null() )			return false;
	// sanity check - currently only handle ipv4
	DBG_ASSERT( is_v4() );
	// test against rfc1918 network address
	if( ip_netaddr_t("10.0.0.0", 8).contain(*this) )	return true;
	if( ip_netaddr_t("172.16.0.0", 12).contain(*this) )	return true;
	if( ip_netaddr_t("192.168.0.0", 16).contain(*this) )	return true;
	return false;
}

/** \brief return true is the ip_addr is public (aka not local and not private not linklocal)
 */
bool ip_addr_t::is_public() const throw()
{
	if( is_null() )			return false;
	if( !is_fully_qualified() )	return false;
	if( is_private() )		return false;
	if( is_localhost() )		return false;	
	if( is_linklocal() )		return false;
	return true;
}

/** \brief Return true if the ip_addr_t is said 'fully_qualified'
 */
bool	ip_addr_t::is_fully_qualified()	const throw()
{
	if( is_null() )		return false;
	if( is_any() )		return false;
	if( is_multicast() )	return false;
	if( is_broadcast() )	return false;
	return true;
}

/** \brief return true if the ip_addr_t is ANY
 */
bool ip_addr_t::is_any() const throw()
{
	// if it is_null(), return false now
	if( is_null() )			return false;
	// sanity check - currently only handle ipv4
	DBG_ASSERT( is_v4() );
	// test
	if( *this != "0.0.0.0" )	return false;
	return true;
}

/** \brief return true is the ip_addr is multicast
 */
bool ip_addr_t::is_multicast() const throw()
{
	// if it is_null(), return false now
	if( is_null() )				return false;
	// sanity check - currently only handle ipv4
	DBG_ASSERT( is_v4() );
	// test against multicast address
	if( ip_netaddr_t("224.0.0.0", 4).contain(*this) )	return true;
	return false;
}

/** \brief return true if the ip_addr_t is broadcast
 */
bool ip_addr_t::is_broadcast() const throw()
{
	// if it is_null(), return false now
	if( is_null() )				return false;
	// sanity check - currently only handle ipv4
	DBG_ASSERT( is_v4() );
	// test
	if( *this != "255.255.255.255" )	return false;
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ip_addr_t
 * 
 * - support null ip_addr_t
 */
serial_t& operator << (serial_t& serial, const ip_addr_t &ip_addr)		throw()
{
	// serialize the version
	serial << ip_addr.version;
	// if ip_addr_t is null, dont serialize any data
	if( ip_addr.is_null() )	return	serial;
	// only ipv4 is handled
	serial << ip_addr.get_v4_addr();
	return serial;
}

/** \brief unserialze a ip_addr_t
 * 
 * - support null ip_addr_t
 */
serial_t& operator >> (serial_t & serial, ip_addr_t &ip_addr)			throw(serial_except_t)
{
	uint8_t		version;
	uint32_t	v4_addr;
	// nullify the object
	ip_addr	= ip_addr_t();
	// check the payload length for the version
	if(serial.get_len() < (ssize_t)sizeof(version))
		nthrow_serial_plain("ip_addr_t Payload Too Short");
	// unserialize the version
	serial >> version;
	// if the serialized ip_addr_t is null
	if( version == 0 )		return serial;
	// only ipv4 is handled
	if( version != 4)	
		nthrow_serial_plain("ip_addr_t tried to unserialized a unknown version");
	// check the payload length for the ipv4 data
	if(serial.get_len() < (ssize_t)sizeof(uint32_t))
		nthrow_serial_plain("ip_addr_t Payload Too Short");
	// get the ip v4 data
	serial >> v4_addr;
	ip_addr	= ip_addr_t(v4_addr);
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END





