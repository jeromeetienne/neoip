/*! \file
    \brief Definition of the socket_type_t
*/


/* system include */
/* local include */
#include "neoip_socket_type.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(socket_type_t,int)
NEOIP_STRTYPE_DEFINITION_ITEM(socket_type_t, DGRAM	, "DGRAM" )
NEOIP_STRTYPE_DEFINITION_ITEM(socket_type_t, RDM	, "RDM" )
NEOIP_STRTYPE_DEFINITION_ITEM(socket_type_t, SEQPACKET	, "SEQPACKET" )
NEOIP_STRTYPE_DEFINITION_ITEM(socket_type_t, STREAM	, "STREAM" )
NEOIP_STRTYPE_DEFINITION_END(socket_type_t)


/** \brief convert a nlay_type_t into a socket_type_t
 * 
 * - this function isnt done in socket_type_t itself as it would require 2
 *   implicit convertion and c++ does only one.
 */
socket_type_t socket_type_from_nlay(const nlay_type_t &nlay_type)	throw()
{
	switch( nlay_type.get_value() ){
	case nlay_type_t::NONE:		return socket_type_t::NONE;
	case nlay_type_t::DGRAM:	return socket_type_t::DGRAM;
	case nlay_type_t::RDM:		return socket_type_t::RDM;
	case nlay_type_t::SEQPACKET:	return socket_type_t::SEQPACKET;
	case nlay_type_t::STREAM:	return socket_type_t::STREAM;
	default:	DBG_ASSERT(0);
	}
	// NOTE: this point MUST not be reached
	DBG_ASSERT( 0 );
	return socket_type_t::NONE;
}

NEOIP_NAMESPACE_END

