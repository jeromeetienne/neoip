/*! \file
    \brief Implementation of the ntudp_pserver_rec_t
    
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_pserver_rec.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the ntudp_pserver_rec_t class
 */
ntudp_pserver_rec_t::ntudp_pserver_rec_t(const ipport_addr_t &m_listen_addr_pview
						, const ntudp_peerid_t &m_peerid) throw()
{
	// copy the parameter
	this->m_listen_addr_pview	= m_listen_addr_pview;
	this->m_peerid			= m_peerid;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			comparison operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ntudp_pserver_rec_t and return value ala strcmp/memcmp
 */
int	ntudp_pserver_rec_t::compare( const ntudp_pserver_rec_t &other )	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )			return -1;
	if( !is_null() &&  other.is_null() )			return +1;
	if(  is_null() &&  other.is_null() )			return  0;

	// handle the peerid
	if( peerid() < other.peerid() )				return -1;
	if( peerid() > other.peerid() )				return +1;

	// handle the listen_addr_pview
	if( listen_addr_pview() < other.listen_addr_pview() )	return -1;
	if( listen_addr_pview() > other.listen_addr_pview() )	return +1;
	// here both are considered equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the address to a string
 */
std::string	ntudp_pserver_rec_t::to_string()		const throw()
{
	std::ostringstream oss;
	// handle the null case
	if( is_null() )			return "null";
	// build the string
	oss << "listen_addr_pview="	<< listen_addr_pview();
	oss << " ";
	oss << "peerid="		<< peerid();
	// return a string	
	return oss.str();	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ntudp_pserver_rec_t
 * 
 * - support null ntudp_pserver_rec_t
 */
serial_t& operator << (serial_t& serial, const ntudp_pserver_rec_t &ntudp_pserver_rec)		throw()
{
	// serialize the object
	serial << ntudp_pserver_rec.listen_addr_pview();
	serial << ntudp_pserver_rec.peerid();
	// return serial
	return serial;
}

/** \brief unserialze a ntudp_pserver_rec_t
 * 
 * - support null ntudp_pserver_rec_t
 */
serial_t& operator >> (serial_t & serial, ntudp_pserver_rec_t &ntudp_pserver_rec)		throw(serial_except_t)
{
	ipport_addr_t	m_listen_addr_pview;
	ntudp_peerid_t	m_peerid;
	// unserialize the fields of the object
	serial >> m_listen_addr_pview;
	serial >> m_peerid;
	// set the returned variable
	ntudp_pserver_rec	= ntudp_pserver_rec_t(m_listen_addr_pview, m_peerid);
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END





