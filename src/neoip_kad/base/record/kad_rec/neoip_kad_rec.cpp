/*! \file
    \brief Definition of the \ref kad_rec_t
    
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_rec.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor with payload
 */
kad_rec_t::kad_rec_t(const kad_recid_t &recid, const kad_keyid_t &keyid, const delay_t &ttl
							, const datum_t &payload)	throw()
{
	this->recid	= recid;
	this->keyid	= keyid;
	this->payload	= payload;
	this->ttl	= ttl;
}

////////////////////////////////////////////////////////////////////////////////	
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the local kad_rec_t is the winner of a tie breaker between
 *         several 2 kad_rec_t with the same recid
 * 
 * - It applies the rules the "highest ttl wins the tie"
 * - if the ttl are equal, the remote node wins to favour the new records
 */
bool	kad_rec_t::is_tie_winner_against(const kad_rec_t &other)	const throw()
{
	// sanity check - both kad_rec_t MUST have the same recid
	DBG_ASSERT( get_recid() == other.get_recid() );
	// if the local ttl is less than other one, the local node loose
	if( get_ttl() < other.get_ttl() )	return false;
	// here the local record wins the tie.
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::string kad_rec_t::to_string()			const throw()
{
	std::ostringstream	oss;
	oss << "recid=" << recid;
	oss << " ";
	oss << "keyid=" << keyid;
	oss << " ";
	oss << "payload=" << payload;
	oss << " ";
	oss << "ttl=" << ttl;
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial kad_rec_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a kad_rec_t
 */
serial_t& operator << (serial_t& serial, const kad_rec_t &kad_rec)	throw()
{
	// serialize the data
	serial << kad_rec.get_recid();
	serial << kad_rec.get_keyid();
	serial << kad_rec.get_payload();
	serial << kad_rec.get_ttl();
	return serial;
}

/** \brief unserialze a kad_rec_t
 */
serial_t& operator >> (serial_t& serial, kad_rec_t &kad_rec)  		throw(serial_except_t)
{
	kad_recid_t	recid;
	kad_keyid_t	keyid;
	datum_t		payload;
	delay_t		ttl;
	// unserial the data
	serial >> recid;
	serial >> keyid;
	serial >> payload;
	serial >> ttl;
	// copy the unserialed data
	kad_rec = kad_rec_t(recid, keyid, ttl, payload);
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc kad_rec_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a kad_rec_t into a xmlrpc
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_rec_t &kad_rec)	throw()
{
	// serialize the data
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("recid")	<< kad_rec.get_recid()		<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("keyid")	<< kad_rec.get_keyid()		<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("payload")<< kad_rec.get_payload()	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("ttl")	<< kad_rec.get_ttl()		<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a kad_rec_t into a xmlrpc
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_rec_t &kad_rec)	throw(xml_except_t)
{
	kad_recid_t	recid;
	kad_keyid_t	keyid;
	datum_t		payload;
	delay_t		ttl;
	// get value from the xmlrpc
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("recid")	>> recid	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("keyid")	>> keyid	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("payload")>> payload	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_BEG("ttl")	>> ttl		>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;
	// copy the unserialed data
	kad_rec = kad_rec_t(recid, keyid, ttl, payload);
	// return the object itself
	return xmlrpc_parse;
}


NEOIP_NAMESPACE_END;






