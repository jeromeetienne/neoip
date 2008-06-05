/*! \file
    \brief Definition of the \ref kad_addr_t
    
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_addr.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor with value
 */
kad_addr_t::kad_addr_t(const ipport_addr_t &m_oaddr, const kad_peerid_t &m_peerid)	throw()
{
	this->m_oaddr	= m_oaddr;
	this->m_peerid	= m_peerid;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          filter function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief filter out the kad_addr_t when the outter address is out of scope of a public address
 * 
 * @return true if the kad_addr_t MUST NOT be used, false otherwise
 */
bool	kad_addr_t::filter_fct_dstpublic(const kad_addr_t &kad_addr)	throw()
{
	// if the ip address IS NOT public, return true to filter it out
	if( !kad_addr.oaddr().get_ipaddr().is_public() )	return true;
	// else return false to keep it
	return false;
}

/** \brief filter out the kad_addr_t when the outter address is out of scope of a private address
 * 
 * @return true if the kad_addr_t MUST NOT be used, false otherwise
 */
bool	kad_addr_t::filter_fct_dstprivate(const kad_addr_t &kad_addr)	throw()
{
	// if the ip address IS private, return true to filter it out
	if( kad_addr.oaddr().get_ipaddr().is_private() )	return true;
	// else return false to keep it
	return false;
}

/** \brief filter out the kad_addr_t when the outter address is out of scope of a linklocal address
 * 
 * @return true if the kad_addr_t MUST NOT be used, false otherwise
 */
bool	kad_addr_t::filter_fct_dstlinklocal(const kad_addr_t &kad_addr)	throw()
{
	// if the ip address IS localhost, return true to filter it out
	if( kad_addr.oaddr().get_ipaddr().is_localhost() )	return true;
	// else return false to keep it
	return false;
}

/** \brief filter out the kad_addr_t when the outter address is out of scope of a localhost address
 * 
 * @return true if the kad_addr_t MUST NOT be used, false otherwise
 */
bool	kad_addr_t::filter_fct_dstlocalhost(const kad_addr_t &kad_addr)	throw()
{
	// ALWAYS return false to keep it
	return false;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int kad_addr_t::compare(const kad_addr_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the peerid
	if( peerid() < other.peerid() )		return -1;	
	if( peerid() > other.peerid() )		return +1;	
	// NOTE: here both have the same peerid

	// compare the address
	if( oaddr() < other.oaddr() )		return -1;	
	if( oaddr() > other.oaddr() )		return +1;	
	// NOTE: here both have the same address

	// here both are equal
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string kad_addr_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "[";
	oss << "oaddr=" << oaddr();
	oss << " ";
	oss << "peerid=" << peerid();
	oss << "]";
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial kad_addr_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a kad_addr_t
 */
serial_t& operator << (serial_t& serial, const kad_addr_t &kad_addr)		throw()
{
	// serialize the data
	serial << kad_addr.oaddr();
	serial << kad_addr.peerid();
	// return the serial object
	return serial;
}

/** \brief unserialze a kad_addr_t
 */
serial_t& operator >> (serial_t& serial, kad_addr_t &kad_addr)  		throw(serial_except_t)
{
	ipport_addr_t	oaddr;
	kad_peerid_t	peerid;
	// unserial the data
	serial >> oaddr;
	serial >> peerid;
	// copy the unserialed data
	kad_addr = kad_addr_t(oaddr, peerid);
	// return the serial object
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc kad_addr_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a kad_addr_t into a xmlrpc
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_addr_t &kad_addr)	throw()
{
	// serialize the data
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("oaddr")	<< kad_addr.oaddr()	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("peerid")	<< kad_addr.peerid()	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a kad_addr_t into a xmlrpc
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_addr_t &kad_addr)		throw(xml_except_t)
{
	ipport_addr_t	oaddr;
	kad_peerid_t	peerid;
	// get value from the xmlrpc
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("oaddr")	>> oaddr	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("peerid")	>> peerid	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;
	// copy the unserialed data
	kad_addr = kad_addr_t(oaddr, peerid);
	// return the object itself
	return xmlrpc_parse;
}
NEOIP_NAMESPACE_END;






