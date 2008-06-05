/*! \file
    \brief Implementation of the ntudp_rdvpt_t
    
*/

/* system include */
/* local include */
#include "neoip_ntudp_rdvpt.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the ntudp_rdvpt_t class
 */
ntudp_rdvpt_t::ntudp_rdvpt_t(const ipport_addr_t &ipport_addr, const ntudp_rdvpt_t::type_t &rdvpt_type)
										throw()
{
	// sanity check - the ipport_addr MUST be fully qualified
	DBG_ASSERT( ipport_addr.is_fully_qualified() );
	// copy the parameter
	this->ipport_addr	= ipport_addr;
	this->rdvpt_type	= rdvpt_type;
}


/** \brief convert the rdvptess to a string
 */
std::string	ntudp_rdvpt_t::to_string()				const throw()
{
	// handle the null case
	if( is_null() )			return "null";
	// build the string
	std::ostringstream oss;
	oss << "ipport_addr="	<< ipport_addr;
	oss << "(";
	if( rdvpt_type == RELAY )	oss << "RELAY";
	else if( rdvpt_type == DIRECT )	oss << "DIRECT";
	else DBG_ASSERT( 0 );
	oss << ")";
	// return a string	
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		 COMPARISON OPERTATOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ntudp_rdvpt_t and return value ala strcmp/memcmp
 */
int	ntudp_rdvpt_t::compare( const ntudp_rdvpt_t &other )	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the ipport_addr
	if( get_addr() < other.get_addr() )		return -1;
	if( get_addr() > other.get_addr() )		return +1;
	// handle the rdvpt_type
	if( get_type() < other.get_type() )		return -1;
	if( get_type() > other.get_type() )		return +1;
	// here both are considered equal
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ntudp_rdvpt_t
 * 
 * - support null ntudp_rdvpt_t
 */
serial_t& operator << (serial_t& serial, const ntudp_rdvpt_t &ntudp_rdvpt)		throw()
{
	// serialize the object
	serial << ntudp_rdvpt.ipport_addr;
	serial << uint8_t(ntudp_rdvpt.rdvpt_type);
	// return serial
	return serial;
}

/** \brief unserialze a ntudp_rdvpt_t
 * 
 * - support null ntudp_rdvpt_t
 */
serial_t& operator >> (serial_t & serial, ntudp_rdvpt_t &ntudp_rdvpt)		throw(serial_except_t)
{
	ipport_addr_t	ipport_addr;
	uint8_t		rdvpt_type;
	// unserialize the object
	serial >> ipport_addr;
	serial >> rdvpt_type;
	// set the returned variable
	ntudp_rdvpt	= ntudp_rdvpt_t(ipport_addr, (ntudp_rdvpt_t::type_t)rdvpt_type);
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END





