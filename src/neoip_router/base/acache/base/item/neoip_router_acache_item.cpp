/*! \file
    \brief Definition of the \ref router_acache_item_t

*/


/* system include */
/* local include */
#include "neoip_router_acache_item.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

router_acache_item_t::router_acache_item_t(const router_name_t &m_remote_dnsname
			, const ip_addr_t &m_local_iaddr, const ip_addr_t &m_remote_iaddr)	throw()
{
	// sanity check - remote_dnsname MUST be is_fully_qualified()
	DBG_ASSERT( m_remote_dnsname.is_fully_qualified() );
	// sanity check - local_iaddr MUST be is_fully_qualified()
	DBG_ASSERT( m_local_iaddr.is_fully_qualified() );
	// sanity check - remote_iaddr MUST be is_fully_qualified()
	DBG_ASSERT( m_remote_iaddr.is_fully_qualified() );
	// copy the paramters
	remote_dnsname	(m_remote_dnsname);
	local_iaddr	(m_local_iaddr);
	remote_iaddr	(m_remote_iaddr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 router_acache_item_t and return value ala strcmp/memcmp
 */
int	router_acache_item_t::compare(const router_acache_item_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the remote_dnsname
	if( remote_dnsname() < other.remote_dnsname() )	return -1;
	if( remote_dnsname() > other.remote_dnsname() )	return +1;
	// handle the local_iaddr
	if( local_iaddr() < other.local_iaddr() )	return -1;
	if( local_iaddr() > other.local_iaddr() )	return +1;
	// handle the remote_iaddr
	if( remote_iaddr() < other.remote_iaddr() )	return -1;
	if( remote_iaddr() > other.remote_iaddr() )	return +1;
	// here both are considered equal, so return 0
	return 0;
}
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	router_acache_item_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[remote_dnsname="	<< remote_dnsname();
	oss << " local_iaddr="		<< local_iaddr();
	oss << " remote_iaddr="		<< remote_iaddr();
	oss << "]";
	// return the just built string
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a router_acache_item_t
 */
serial_t& operator << (serial_t& serial, const router_acache_item_t &acache_item)		throw()
{
	// serialize the router_acache_item_t
	serial << acache_item.remote_dnsname().to_string();
	serial << acache_item.local_iaddr();
	serial << acache_item.remote_iaddr();
	// return serial
	return serial;
}

/** \brief unserialze a router_acache_item_t
 * 
 * - supremote_iaddr null router_acache_item_t
 */
serial_t& operator >> (serial_t & serial, router_acache_item_t &acache_item)	throw(serial_except_t)
{
	std::string	remote_dnsname;
	ip_addr_t	local_iaddr;
	ip_addr_t	remote_iaddr;
	// unserialize the router_acache_item_t fields
	serial >> remote_dnsname;
	serial >> local_iaddr;
	serial >> remote_iaddr;
	// set the destination value
	acache_item	= router_acache_item_t(router_name_t(remote_dnsname), local_iaddr, remote_iaddr);
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END

