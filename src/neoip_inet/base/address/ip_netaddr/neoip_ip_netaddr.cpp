/*! \file
    \brief Implementation of the ip_netaddr_t

- TODO coded without real comprehension of the netaddr concept
  - especially the size of the prefix - what is the limit ? can it be 32 ?
  - what is the bcast/any addr on 10.0.0.1/32 ?
  - what is the first/last hostaddr on 10.0.0.1/32 ?
 
*/

/* system include */
/* local include */
#include "neoip_ip_netaddr.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the ip_netaddr_t class from a string
 */
ip_netaddr_t::ip_netaddr_t(const char *netaddr_str)	throw()
{
	if( from_string(netaddr_str) ){
		KLOG_ERR("ip_netaddr_t constructor cant parse " << netaddr_str << " address. nullifying!");
		DBGNET_ASSERT( 0 );	// just to detect the error immediatly
		nullify();
	}
}

/** \brief constructor of the ip_netaddr_t class from a string
 */
ip_netaddr_t::ip_netaddr_t(const std::string &netaddr_str)	throw()
{
	if( from_string(netaddr_str.c_str()) ){
		KLOG_ERR("ip_netaddr_t constructor cant parse " << netaddr_str << " address. nullifying!");
		DBGNET_ASSERT( 0 );	// just to detect the error immediatly
		nullify();
	}
}


/** \brief constructor of the ip_netaddr_t class
 */
ip_netaddr_t::ip_netaddr_t(const ip_addr_t &base_addr, int prefix_len) throw()
{
	this->base_addr		= base_addr;
	this->prefix_len	= prefix_len;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 	from/to_string
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the ip_netaddr_t from a string
 * 
 * - this function is used in several place to set ip_netaddr_t from a string
 * - this function should be recoded using C++ style of parsing and not 
 *   glibc special function such as strndupa
 * 
 * @return false if no error occured, true otherwise (format ip_addr/prefix e.g. 10.0.0.1/8)
 */
bool ip_netaddr_t::from_string(const std::string &netaddr_str) throw()
{
	std::vector<std::string>	tmp = string_t::split(netaddr_str, "/", 2);
	// check there is 2 parts
	if( tmp.size() != 2 )	return true;
	// get the base_addr
	base_addr	= tmp[0].c_str();
	// get the prefix
	prefix_len	= atoi( tmp[1].c_str() );
	// return no error
	return false;
}

/** \brief convert the ip_netaddr_t to a string
 */
std::string	ip_netaddr_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return base_addr.to_string() + "/" + OSTREAMSTR((int)prefix_len);	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		 COMPARISON OPERTATOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ip_addr_t and return value ala strcmp/memcmp
 */
int	ip_netaddr_t::compare( const ip_netaddr_t &other )	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the prefix
	if( prefix_len < other.prefix_len )		return -1;
	if( prefix_len > other.prefix_len )		return +1;
	// handle the base address
	if( base_addr < other.base_addr )		return -1;
	if( base_addr > other.base_addr )		return +1;
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       UTILITY functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** return the ANY address for this netmask
 */
ip_addr_t	ip_netaddr_t::get_netmask() const throw()
{
	// sanity check - currently only ipv4 is supported
	DBG_ASSERT( base_addr.is_v4() );
	// set the returned value
	return ip_addr_t(((uint32_t)0xffffffff) << (32-prefix_len));
}

/** return the ANY address for this netmask
 */
ip_addr_t	ip_netaddr_t::get_any_addr() const throw()
{
	// sanity check - currently only ipv4 is supported
	DBG_ASSERT( base_addr.is_v4() );
	// set the returned value
	return ip_addr_t(base_addr.get_v4_addr() & get_netmask().get_v4_addr());
}

/** return the BROADCAST address for this netmask
 */
ip_addr_t	ip_netaddr_t::get_bcast_addr() const throw()
{
	// sanity check - currently only ipv4 is supported
	DBG_ASSERT( base_addr.is_v4() );
	// set the returned value
	return ip_addr_t(base_addr.get_v4_addr() | ~(get_netmask().get_v4_addr()));
}

/** \brief return true if the ip_addr is contained in this ip_netaddr_t, false otherwise
 */
bool	ip_netaddr_t::contain(const ip_addr_t &ip_addr)	const throw()
{
	// if both doesnt have the same version, the ip_addr isnt contained
	if( base_addr.get_version() != ip_addr.get_version() )	return false;
	// perform the test
	return get_any_addr() <= ip_addr && ip_addr <= get_bcast_addr();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    'cursor' handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the first address of this ip_netaddr_t
 * 
 * - it return ip address valid for host aka not any or bcast
 */
ip_addr_t	ip_netaddr_t::get_first_addr()	const throw()
{
	ip_addr_t	first_addr	= base_addr;
	// if the first_addr is any, increase it
	if( first_addr == get_any_addr() )	first_addr = first_addr + 1;
	// if the last_addr is not contained in this ip_netaddr_t, return the base_addr itself
	// - this may happen with prefix_len > 31
	if( !contain(first_addr) )	return base_addr;
	// return the first_addr	
	return first_addr;
}

/** \brief return the last address of this ip_netaddr_t
 * 
 * - it return ip address valid for host aka not any or bcast
 */
ip_addr_t	ip_netaddr_t::get_last_addr()	const throw()
{
	ip_addr_t	last_addr = get_bcast_addr() - 1;
	// if the last_addr is not contained in this ip_netaddr_t, return the bcast_addr itself
	// - this may happen with prefix_len > 31
	if( !contain(last_addr) )	return get_bcast_addr();
	// return the last_addr
	return last_addr;
}

/** \brief return the next ip address after the one in parameter for this ip_netaddr_t
 * 
 * - it return ip address valid for host, not destination address
 *   - aka it doesnt return the any or broadcast address of the network
 */
ip_addr_t	ip_netaddr_t::get_next_addr(const ip_addr_t &cur_ip_addr)	const throw()
{
	// if the cur_ip_addr is null, return the first address
	if( cur_ip_addr.is_null() )		return get_first_addr();
	// if the cur_ip_addr is the last addreess, return the first_addr
	if( cur_ip_addr == get_last_addr() )	return get_first_addr();

	// return the next address
	return	cur_ip_addr + 1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ip_netaddr_t
 * 
 * - support null ip_netaddr_t
 */
serial_t& operator << (serial_t& serial, const ip_netaddr_t &ip_netaddr)		throw()
{
	// serialize the prefix_len
	serial << ip_netaddr.prefix_len;
	// serialize the base_addr
	serial << ip_netaddr.base_addr;
	// return serial
	return serial;
}

/** \brief unserialze a ip_netaddr_t
 * 
 * - support null ip_netaddr_t
 */
serial_t& operator >> (serial_t & serial, ip_netaddr_t &ip_netaddr)			throw(serial_except_t)
{
	uint8_t		prefix_len;
	ip_addr_t	base_addr;
	serial >> prefix_len;
	serial >> base_addr;
	// set the returned variable
	ip_netaddr	= ip_netaddr_t(base_addr, prefix_len);
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

