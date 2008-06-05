/*! \file
    \brief Definition of the \ref kad_rpcpeer_id_t

\par Brief Description
\ref kad_rpcpeer_id_t handle a object allowing to uniquely idendified
kad_rpcpeer_t.
it uses the kad_rpcpeer_t::kad_peer_t::local_peerid which is statically unique and
a kad_rpcpeer_id_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_kad_rpcpeer_id.hpp"
#include "neoip_kad_rpcpeer.hpp"
#include "neoip_kad_rpclistener.hpp"
#include "neoip_kad_rpclistener_id.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_xmlrpc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a kad_rpcpeer_id_t from a bt_ezswarm_t
 */
kad_rpcpeer_id_t	kad_rpcpeer_id_t::from_rpcpeer(const kad_rpcpeer_t *kad_rpcpeer)	throw()
{
	kad_rpcpeer_id_t	rpcpeer_id;
	// get the udp_listen_lview from the bt_ezswarm_t
	rpcpeer_id.rpclistener_id	( kad_rpcpeer->rpclistener()->rpclistener_id() );
	rpcpeer_id.kad_peerid		( kad_rpcpeer->kad_peer()->local_peerid() );
	// return the just built kad_rpcpeer_id_t
	return rpcpeer_id;
}


/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	kad_rpcpeer_id_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( rpclistener_id().is_null() )	return true;
	if( kad_peerid().is_null() )		return true;
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
int kad_rpcpeer_id_t::compare(const kad_rpcpeer_id_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// NOTE: here both are NOT null
	
	// compare the rpclistener_id
	if( rpclistener_id() < other.rpclistener_id() )	return -1;	
	if( rpclistener_id() > other.rpclistener_id() )	return +1;	
	// NOTE: here both have the same rpclistener_id

	// compare the kad_peerid
	if( kad_peerid() < other.kad_peerid() )		return -1;	
	if( kad_peerid() > other.kad_peerid() )		return +1;	
	// NOTE: here both have the same kad_peerid

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
std::string	kad_rpcpeer_id_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "rpclistener_id="	<< rpclistener_id();	
	oss << " " << "kad_peerid="	<< kad_peerid();	
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc kad_rpcpeer_id_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for kad_rpcpeer_id_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_rpcpeer_id_t &kad_rpcpeer_id)	throw()
{
	// serialize the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build 	<< xmlrpc_build_t::MEMBER_BEG("rpclistener_id");
	xmlrpc_build		<< kad_rpcpeer_id.rpclistener_id();
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build 	<< xmlrpc_build_t::MEMBER_BEG("kad_peerid");
	xmlrpc_build		<< kad_rpcpeer_id.kad_peerid();
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief unserialize xmlrpc for kad_rpcpeer_id_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_rpcpeer_id_t &kad_rpcpeer_id)	throw(xml_except_t)
{
	kad_rpclistener_id_t	rpclistener_id;
	kad_peerid_t		kad_peerid;
	// unserialize the struct
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG("rpclistener_id");
	xmlrpc_parse		>> rpclistener_id;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG("kad_peerid");
	xmlrpc_parse		>> kad_peerid;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;
	// set the unserialized value in the kad_rpcpeer_id_t
	kad_rpcpeer_id.rpclistener_id	(rpclistener_id);
	kad_rpcpeer_id.kad_peerid	(kad_peerid);
	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END

