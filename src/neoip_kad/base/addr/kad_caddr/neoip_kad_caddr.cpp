/*! \file
    \brief Definition of the \ref kad_caddr_t
    
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_caddr.hpp"
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
kad_caddr_t::kad_caddr_t(const ipport_addr_t &addr, const kad_peerid_t &peerid
						, const cookie_id_t &cookie_id)		throw()
{
	this->addr	= addr;
	this->peerid	= peerid;
	this->cookie_id	= cookie_id;
}

/** \brief Constructor with value
 */
kad_caddr_t::kad_caddr_t(const kad_addr_t &kad_addr, const cookie_id_t &cookie_id)	throw()
{
	this->addr	= kad_addr.get_oaddr();
	this->peerid	= kad_addr.get_peerid();
	this->cookie_id	= cookie_id;
}
	
/** \brief return true if the object is null, false otherwise
 */
bool	kad_caddr_t::is_null()	const throw()
{
	if( addr.is_null() )	return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int kad_caddr_t::compare(const kad_caddr_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the peerid
	if( peerid < other.peerid )		return -1;	
	if( peerid > other.peerid )		return +1;	
	// NOTE: here both have the same peerid

	// compare the address
	if( addr < other.addr )			return -1;	
	if( addr > other.addr )			return +1;	
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
std::string kad_caddr_t::to_string()			const throw()
{
	std::ostringstream	oss;
	oss << "addr=" << addr;
	oss << " ";
	oss << "peerid=" << peerid;
	oss << " ";
	oss << "cookie_id=" << cookie_id;
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc kad_caddr_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a kad_caddr_t into a xmlrpc
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_caddr_t &kad_caddr)	throw()
{
	// serialize the data
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("oaddr")	<< kad_caddr.get_oaddr()	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("peerid")	<< kad_caddr.get_peerid()	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build	<<xmlrpc_build_t::MEMBER_BEG("cookie_id")<<kad_caddr.get_cookie_id()	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a kad_caddr_t into a xmlrpc
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_caddr_t &kad_caddr)		throw(xml_except_t)
{
	ipport_addr_t	addr;
	kad_peerid_t	peerid;
	cookie_id_t	cookie_id;
	// get value from the xmlrpc
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("oaddr")		>> addr		>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("peerid")		>> peerid	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("cookie_id")	>> cookie_id	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;
	// copy the unserialed data
	kad_caddr = kad_caddr_t(addr, peerid, cookie_id);
	// return the object itself
	return xmlrpc_parse;
}


NEOIP_NAMESPACE_END;






