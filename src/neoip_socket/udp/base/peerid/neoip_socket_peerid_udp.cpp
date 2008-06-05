/*! \file
    \brief definition of the \ref socket_peerid_udp_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_peerid_udp.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_peerid_vapi_t, socket_peerid_udp_t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set from a string
 */
socket_err_t	socket_peerid_udp_t::ctor_from_str(const std::string &str)	throw()
{
	// parse the input
	ipaddr	= ip_addr_t(str.c_str());
	// if an error occur while parsing this, report it now
	if( ipaddr.is_null() )
		return socket_err_t(socket_err_t::BAD_PARAM, "bad socket_peerid_udp_t format in "+ str);
	// return no error
	return socket_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the domain of this address
 */
socket_domain_t socket_peerid_udp_t::get_domain()	const throw()
{
	return socket_domain_t::UDP;
}

/** \brief return true if this peerid is of type 'localhost', false otherwise
 */
bool	socket_peerid_udp_t::is_localhost()			const throw()
{
	return ipaddr.is_localhost();
}

/** \brief return true if this peerid is of type 'linklocal', false otherwise
 */
bool	socket_peerid_udp_t::is_linklocal()			const throw()
{
	return ipaddr.is_linklocal();
}

/** \brief return true if this peerid is of type 'private', false otherwise
 */
bool	socket_peerid_udp_t::is_private()			const throw()
{
	return ipaddr.is_private();
}

/** \brief return true if this peerid is of type 'public', false otherwise
 */
bool	socket_peerid_udp_t::is_public()			const throw()
{
	return ipaddr.is_public();
}

/** \brief return true if this peerid is of type 'any', false otherwise
 */
bool	socket_peerid_udp_t::is_any()			const throw()
{
	return ipaddr.is_any();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 \ref socket_peerid_udp_t
 */
int	socket_peerid_udp_t::compare(const socket_peerid_vapi_t &other_api)	const throw()
{
	// sanity check - both MUST have the same type
	DBG_ASSERT( typeid(*this) == typeid(other_api) );
	// cast the base class into the derived one
	const socket_peerid_udp_t &other = dynamic_cast <const socket_peerid_udp_t&>(other_api);
	// compare the ip addr
	return ipaddr.compare(other.ipaddr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert a string into an object
 */
std::string	socket_peerid_udp_t::to_string()				const throw()
{
	return ipaddr.to_string();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  serialization functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize the object (without the domain)
 */
void	socket_peerid_udp_t::serialize(serial_t &serial)			const throw()
{
	serial << ipaddr;
}

/** \brief unserialize the object (without the domain)
 */
void	socket_peerid_udp_t::unserialize(serial_t &serial)			throw(serial_except_t)
{
	serial >> ipaddr;
}
	
NEOIP_NAMESPACE_END




