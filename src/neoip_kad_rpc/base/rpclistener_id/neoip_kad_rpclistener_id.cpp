/*! \file
    \brief Definition of the \ref kad_rpclistener_id_t

\par Brief Description
\ref kad_rpclistener_id_t handle a object allowing to uniquely idendified
kad_rpclistener_t.
it uses the kad_rpclistener_t::udp_vresp::listen_addr which is unique due
to the way it is handled by the OS tcpip stack as it is a unicast listen addr.

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_kad_rpclistener_id.hpp"
#include "neoip_kad_rpclistener.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_xmlrpc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a kad_rpclistener_id_t from a bt_ezswarm_t
 */
kad_rpclistener_id_t	kad_rpclistener_id_t::from_rpclistener(const kad_rpclistener_t *rpclistener)	throw()
{
	kad_rpclistener_id_t	rpclistener_id;
	// get the udp_listen_lview from the bt_ezswarm_t
	rpclistener_id.udp_listen_lview	( rpclistener->udp_vresp()->get_listen_addr() );
	// return the just built kad_rpclistener_id_t
	return rpclistener_id;
}


/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	kad_rpclistener_id_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( udp_listen_lview().is_null() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int kad_rpclistener_id_t::compare(const kad_rpclistener_id_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// NOTE: here both are NOT null
	
	// compare the udp_listen_lview
	if( udp_listen_lview() < other.udp_listen_lview() )	return -1;	
	if( udp_listen_lview() > other.udp_listen_lview() )	return +1;	
	// NOTE: here both have the same udp_listen_lview

	// here both are equal
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	kad_rpclistener_id_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "udp_listen_lview="	<< udp_listen_lview();	
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          xmlrpc kad_rpclistener_id_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for kad_rpclistener_id_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_rpclistener_id_t &kad_rpclistener_id)	throw()
{
	// serialize the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build 	<< xmlrpc_build_t::MEMBER_BEG("udp_listen_lview");
	xmlrpc_build		<< kad_rpclistener_id.udp_listen_lview();
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief unserialize xmlrpc for kad_rpclistener_id_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_rpclistener_id_t &rpclistener_id)	throw(xml_except_t)
{
	ipport_addr_t	udp_listen_lview;
	// unserialize the struct
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG("udp_listen_lview");
	xmlrpc_parse		>> udp_listen_lview;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;
	// set the unserialized value in the kad_rpclistener_id_t
	rpclistener_id.udp_listen_lview	(udp_listen_lview);
	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END

