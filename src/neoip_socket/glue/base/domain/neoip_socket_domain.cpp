/*! \file
    \brief Header of the \ref socket_domain_t

*/

/* system include */
/* local include */
#include "neoip_socket_domain.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(socket_domain_t,uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(socket_domain_t, UDP	, "udp" )
NEOIP_STRTYPE_DEFINITION_ITEM(socket_domain_t, NTLAY	, "ntlay" )
NEOIP_STRTYPE_DEFINITION_ITEM(socket_domain_t, NTUDP	, "ntudp" )
NEOIP_STRTYPE_DEFINITION_ITEM(socket_domain_t, TCP	, "tcp" )
NEOIP_STRTYPE_DEFINITION_ITEM(socket_domain_t, STCP	, "stcp" )
NEOIP_STRTYPE_DEFINITION_END(socket_domain_t)


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//               socket_domain_support_type
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if this socket_domain_t support the socket_type, false otherwise
 */
bool	socket_domain_support_type(const socket_domain_t &socket_domain
					, const socket_type_t &socket_type)	throw()
{
	switch( socket_domain.get_value() ){
	case socket_domain_t::UDP:
			if( socket_type == socket_type_t::DGRAM )	return true;
			if( socket_type == socket_type_t::RDM )		return true;
			if( socket_type == socket_type_t::SEQPACKET )	return true;
// TODO nlay doesnt fully support stream 
//			if( socket_type == socket_type_t::STREAM )	return true;
			return false;
	case socket_domain_t::NTLAY:
			if( socket_type == socket_type_t::DGRAM )	return true;
			if( socket_type == socket_type_t::RDM )		return true;
			if( socket_type == socket_type_t::SEQPACKET )	return true;
// TODO nlay doesnt fully support stream 
//			if( socket_type == socket_type_t::STREAM )	return true;
			return false;
	case socket_domain_t::NTUDP:
			if( socket_type == socket_type_t::DGRAM )	return true;
			return false;
	case socket_domain_t::TCP:
			if( socket_type == socket_type_t::STREAM )	return true;
			return false;
	case socket_domain_t::STCP:
			if( socket_type == socket_type_t::STREAM )	return true;
			return false;
	default:	DBG_ASSERT( 0 );
	}
	// NOTE: this point MUST not be reached
	DBG_ASSERT( 0 );
	return socket_domain_t::NONE;
}

NEOIP_NAMESPACE_END

