/*! \file
    \brief definition of the \ref socket_peerid_ntlay_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_peerid_ntlay.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_peerid_vapi_t, socket_peerid_ntlay_t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set from a string
 */
socket_err_t	socket_peerid_ntlay_t::ctor_from_str(const std::string &str)	throw()
{
	// parse the input
	ntudp_peerid	= str.c_str();
	// if an error occur while parsing this, report it now
	if( ntudp_peerid.is_null() )
		return socket_err_t(socket_err_t::BAD_PARAM, "bad socket_peerid_ntlay_t format in "+ str);
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
socket_domain_t socket_peerid_ntlay_t::get_domain()	const throw()
{
	return socket_domain_t::NTLAY;
}

/** \brief return true if this peerid is of type 'localhost', false otherwise
 */
bool	socket_peerid_ntlay_t::is_localhost()			const throw()
{
	DBG_ASSERT( 0 );
	return false;
}

/** \brief return true if this peerid is of type 'linklocal', false otherwise
 */
bool	socket_peerid_ntlay_t::is_linklocal()			const throw()
{
	DBG_ASSERT( 0 );
	return false;
}

/** \brief return true if this peerid is of type 'private', false otherwise
 */
bool	socket_peerid_ntlay_t::is_private()			const throw()
{
	DBG_ASSERT( 0 );
	return false;
}

/** \brief return true if this peerid is of type 'public', false otherwise
 */
bool	socket_peerid_ntlay_t::is_public()			const throw()
{
	DBG_ASSERT( 0 );
	return false;
}

/** \brief return true if this peerid is of type 'any', false otherwise
 */
bool	socket_peerid_ntlay_t::is_any()			const throw()
{
	DBG_ASSERT( 0 );
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 \ref socket_peerid_ntlay_t
 */
int	socket_peerid_ntlay_t::compare(const socket_peerid_vapi_t &other_api)	const throw()
{
	// sanity check - both MUST have the same type
	DBG_ASSERT( typeid(*this) == typeid(other_api) );
	// cast the base class into the derived one
	const socket_peerid_ntlay_t &other = dynamic_cast <const socket_peerid_ntlay_t&>(other_api);
	// compare the ip addr
	return ntudp_peerid.compare(other.ntudp_peerid);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert a string into an object
 */
std::string	socket_peerid_ntlay_t::to_string()				const throw()
{
	return ntudp_peerid.to_string();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  serialization functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize the object (without the domain)
 */
void	socket_peerid_ntlay_t::serialize(serial_t &serial)			const throw()
{
	serial << ntudp_peerid;
}

/** \brief unserialize the object (without the domain)
 */
void	socket_peerid_ntlay_t::unserialize(serial_t &serial)			throw(serial_except_t)
{
	serial >> ntudp_peerid;
}
	
NEOIP_NAMESPACE_END




