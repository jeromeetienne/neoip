/*! \file
    \brief Implementation of the ntudp_addr_t
    
*/

/* system include */
/* local include */
#include "neoip_ntudp_addr.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the ntudp_addr_t class
 */
ntudp_addr_t::ntudp_addr_t(const ntudp_peerid_t &m_peerid, const ntudp_portid_t &m_portid) throw()
{
	this->m_peerid	= m_peerid;
	this->m_portid	= m_portid;
}

/** \brief constructor of the ntudp_addr_t class from a string
 */
ntudp_addr_t::ntudp_addr_t(const char *addr_str)			throw()
{
	if( from_string(addr_str) ){
		KLOG_ERR("ntudp_addr_t constructor cant parse " << addr_str << " address. nullifying!");
		DBG_ASSERT( 0 );	// just to detect the error immediatly
		nullify();
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  	from/to_string
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the ntudp_addr_t from a string
 * 
 * - TODO likely some issue with the ANY cases. in peerid and in portid
 *   - i need a ANY i dont think this is handled in gen_id<> 
 *   - hnmmm to handel the .is_null() == is_any() would be dirty
 *   - for gen_id<> == "0..0" = is null
 *   - so put a special value for ntudp_peerid_t::ANY and ntudp_portid_t::ANY and there! :)
 * 
 * @return false if no error occured, true otherwise
 */
bool ntudp_addr_t::from_string(const std::string &ntudp_addr_str) throw()
{
	// if the string is empty, it is an error
	if( ntudp_addr_str.empty() )	return true;
	// nullify the ntudp_addr
	nullify();
	// split by the ':' which split the address from the port number
	std::vector<std::string>	tmp = string_t::split(ntudp_addr_str, ":", 2);
	// check there is 2 parts
	if( tmp.size() == 0 )		return true;
	// handle the peerid
	if( tmp[0].size() )		m_peerid = tmp[0].c_str();
	
	// handle the default port case
	if( tmp.size() == 1 )		return false;
	// set the portid
	m_portid	= tmp[1].c_str();
	// return no errro
	return false;
}
/** \brief convert the address to a string
 */
std::string	ntudp_addr_t::to_string()				const throw()
{
	// handle the null case
	if( is_null() )			return "null";
	// build the string
	std::ostringstream oss;
	oss << peerid();
	oss << ":";
	oss << portid();
	// return a string	
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//		 COMPARISON OPERTATOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 ntudp_addr_t and return value ala strcmp/memcmp
 */
int	ntudp_addr_t::compare( const ntudp_addr_t &other )	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// handle the peerid
	if( peerid() < other.peerid() )			return -1;
	if( peerid() > other.peerid() )			return +1;
	// handle the port
	if( portid() < other.portid() )			return -1;
	if( portid() > other.portid() )			return +1;
	// here both are considered equal
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ntudp_addr_t
 * 
 * - support null ntudp_addr_t
 */
serial_t& operator << (serial_t& serial, const ntudp_addr_t &ntudp_addr)		throw()
{
	// serialize the peerid/portid
	serial << ntudp_addr.peerid();
	serial << ntudp_addr.portid();
	// return serial
	return serial;
}

/** \brief unserialze a ntudp_addr_t
 * 
 * - support null ntudp_addr_t
 */
serial_t& operator >> (serial_t & serial, ntudp_addr_t &ntudp_addr)		throw(serial_except_t)
{
	ntudp_peerid_t	ntudp_peerid;
	ntudp_portid_t	ntudp_portid;
	// unserialize the peerid
	serial >> ntudp_peerid;
	// unserialize the portid
	serial >> ntudp_portid;
	// set the returned variable
	ntudp_addr	= ntudp_addr_t(ntudp_peerid, ntudp_portid);
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END





