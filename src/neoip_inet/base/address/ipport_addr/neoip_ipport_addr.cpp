/*! \file
    \brief Implementation of the ipport_addr_t
    
*/

/* system include */
/* local include */
#include "neoip_inet_oswarp.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ipport_addr_t constant
const ipport_addr_t	ipport_addr_t::ANY_IP4	= "0.0.0.0:0";
// end of constants definition


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the ipport_addr_t class
 */
ipport_addr_t::ipport_addr_t(const ip_addr_t &new_ipaddr, uint16_t new_port) throw()
{
	this->m_ipaddr	= new_ipaddr;
	this->m_port	= new_port;
}

/** \brief constructor of the ipport_addr_t class from a string
 */
ipport_addr_t::ipport_addr_t(const char *ipport_addr_str)			throw()
{
	if( from_string(ipport_addr_str) ){
		KLOG_ERR("ipport_addr_t constructor cant parse " << ipport_addr_str << " address. nullifying!");
		nullify();
	}
}

/** \brief constructor of the ipport_addr_t class from a string
 */
ipport_addr_t::ipport_addr_t(const std::string &ipport_addr_str)			throw()
{
	if( from_string(ipport_addr_str.c_str()) ){
		KLOG_ERR("ipport_addr_t constructor cant parse " << ipport_addr_str << " address. nullifying!");
		nullify();
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  	from/to_string
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the ip_addr_t from a string
 * 
 * - this function is used in several place to set ip_addr_t from a string
 * - this function should be recoded using C++ style of parsing and not 
 *   glibc special function such as strndupa
 * 
 * @return false if no error occured, true otherwise
 */
bool ipport_addr_t::from_string(const std::string &ipport_addr_str) throw()
{
	// if the string is empty, it is an error
	if( ipport_addr_str.empty() )	return true;
	// split by the ':' which split the address from the port number
	std::vector<std::string>	tmp = string_t::split(ipport_addr_str, ":", 2);
	// check there is 2 parts
	if( tmp.size() == 0 )		return true;
	// handle the ip address
	if( tmp[0].size() )		m_ipaddr	= tmp[0].c_str();
	else				m_ipaddr	= "0.0.0.0";	// TODO this doesnt support ipv6
	
	// set the port to the default value
	m_port = 0;
	// handle the default port case
	if( tmp.size() == 1 )		return false;
	// set the port
	m_port	= atoi( tmp[1].c_str() );
	// return false as no error occurs
	return false;
}


/** \brief convert the address to a string
 */
std::string	ipport_addr_t::to_string()				const throw()
{
	std::string	str;
	if( is_null() )			return "null";
	str += ipaddr().to_string();
	str += ":";
	if( port() == 0 )		str += "default";
	else				str += OSTREAMSTR(port());
	// return the just built string
	return str;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ipport_addr_t and return value ala strcmp/memcmp
 */
int	ipport_addr_t::compare( const ipport_addr_t &other )	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// handle the ip_addr_t
	if( ipaddr() < other.ipaddr() )		return -1;
	if( ipaddr() > other.ipaddr() )		return +1;
	// handle the port
	if( port() < other.port() )		return -1;
	if( port() > other.port() )		return +1;
	// if this point is reached, both are considered equal, so return 0
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  	sockaddr_in
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert a ipport_addr_t to a sockaddr_in
 */
ipport_addr_t::ipport_addr_t(const struct sockaddr_in &sa_in)		throw()
{
	// sanity check - currently only handle ipv4
	DBG_ASSERT( sa_in.sin_family == AF_INET );	
	// convert the object
	m_ipaddr	= ip_addr_t( ntohl(sa_in.sin_addr.s_addr) );
	m_port		= ntohs( sa_in.sin_port );
}

/** \brief convert a ipport_addr_t to a sockaddr_in
 */
struct 	sockaddr_in ipport_addr_t::to_sockaddr_in()				const throw()
{
	struct sockaddr_in sa_in;
	// sanity check - currently only handle ipv4
	DBG_ASSERT( ipaddr().is_v4() );
	// zero the struct
	memset( &sa_in, 0, sizeof(sa_in) );
	// set the struct
	sa_in.sin_family	= AF_INET;
	sa_in.sin_addr.s_addr	= htonl( ipaddr().get_v4_addr() );
	sa_in.sin_port		= htons( port() );
	// return the built object
	return sa_in;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ipport_addr_t
 * 
 * - support null ipport_addr_t
 */
serial_t& operator << (serial_t& serial, const ipport_addr_t &ipport_addr)		throw()
{
	// serialize the ip_addr
	serial << ipport_addr.ipaddr();
	serial << ipport_addr.port();
	// return serial
	return serial;
}

/** \brief unserialze a ipport_addr_t
 * 
 * - support null ipport_addr_t
 */
serial_t& operator >> (serial_t & serial, ipport_addr_t &ipport_addr)			throw(serial_except_t)
{
	ip_addr_t	m_ipaddr;
	uint16_t	m_port;
	// unserialize the values
	serial >> m_ipaddr;
	serial >> m_port;
	// set the returned variable
	ipport_addr	= ipport_addr_t(m_ipaddr, m_port);
	// return serial
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc ipport_addr_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a ipport_addr_t into a xmlrpc
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const ipport_addr_t &ipport_addr)	throw()
{
	// put a string representing the ipport_addr_t
	xmlrpc_build << ipport_addr.to_string();
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a ipport_addr_t into a xmlrpc
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, ipport_addr_t &ipport_addr)	throw(xml_except_t)
{
	std::string	value;
	// get value from the xmlrpc
	xmlrpc_parse >> value;
	// convert the value
	ipport_addr	= value;
	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END





