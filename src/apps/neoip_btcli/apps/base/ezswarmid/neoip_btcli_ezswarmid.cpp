/*! \file
    \brief Definition of the \ref btcli_ezswarmid_t

*/


/* system include */
#include <iostream>\
/* local include */
#include "neoip_btcli_ezswarmid.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_xmlrpc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the bt_ezswarm_t identified by this btcli_ezswarmid_t
 * 
 * - or NULL if none is found
 */
bt_ezswarm_t *	btcli_ezswarmid_t::to_bt_ezswarm(const bt_ezsession_t *bt_ezsession)	const throw()
{
	bt_ezswarm_t *	bt_ezswarm;
	// get this bt_ezswarm_t from the bt_ezsession
	bt_ezswarm	= bt_ezsession->bt_ezswarm_by_infohash(infohash());
	// return the found value (may be NULL)
	return bt_ezswarm;
}

/** \brief Build a btcli_ezswarmid_t from a bt_ezswarm_t
 */
btcli_ezswarmid_t	btcli_ezswarmid_t::from_bt_ezswarm(const bt_ezswarm_t *bt_ezswarm)	throw()
{
	btcli_ezswarmid_t	ezswarmid;
	// get the infohash from the bt_ezswarm_t
	ezswarmid.infohash	( bt_ezswarm->mfile().infohash() );
	// return the just built btcli_ezswarmid_t
	return ezswarmid;
}


/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	btcli_ezswarmid_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( infohash().is_null() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}
	

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	btcli_ezswarmid_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "infohash="	<< infohash();	
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc btcli_ezswarmid_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for btcli_ezswarmid_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const btcli_ezswarmid_t &btcli_ezswarmid)	throw()
{
	// serialize the struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build 	<< xmlrpc_build_t::MEMBER_BEG("infohash");
	xmlrpc_build		<< btcli_ezswarmid.infohash();
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief unserialize xmlrpc for btcli_ezswarmid_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, btcli_ezswarmid_t &btcli_ezswarmid)	throw(xml_except_t)
{
	bt_id_t	infohash;
	// unserialize the struct
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG("infohash");
	xmlrpc_parse		>> infohash;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;
	// set the unserialized value in the btcli_ezswarmid_t
	btcli_ezswarmid.infohash	(infohash);
	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END

