/*! \file
    \brief definition of the \ref socket_portid_tcp_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_portid_tcp.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_portid_vapi_t, socket_portid_tcp_t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set from a string
 */
socket_err_t	socket_portid_tcp_t::ctor_from_str(const std::string &str)	throw()
{
	// parse the input
	port	= atoi( str.c_str() );
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
socket_domain_t socket_portid_tcp_t::get_domain()	const throw()
{
	return socket_domain_t::TCP;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 \ref socket_portid_tcp_t
 */
int	socket_portid_tcp_t::compare(const socket_portid_vapi_t &other_api)	const throw()
{
	// sanity check - both MUST have the same type
	DBG_ASSERT( typeid(*this) == typeid(other_api) );
	// cast the base class into the derived one
	const socket_portid_tcp_t &other = dynamic_cast <const socket_portid_tcp_t&>(other_api);

	// compare the port
	if( port < other.port )		return -1;
	if( port > other.port )		return +1;
	// NOTE: here both are considered equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert a string into an object
 */
std::string	socket_portid_tcp_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// build the string 
	oss << port;
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  serialization functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize the object (without the domain)
 */
void	socket_portid_tcp_t::serialize(serial_t &serial)			const throw()
{
	serial << port;
}

/** \brief unserialize the object (without the domain)
 */
void	socket_portid_tcp_t::unserialize(serial_t &serial)			throw(serial_except_t)
{
	serial >> port;
}
	
NEOIP_NAMESPACE_END




