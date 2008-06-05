/*! \file
    \brief Header of the \ref socket_domain_t

*/


#ifndef __NEOIP_SOCKET_DOMAIN_HPP__ 
#define __NEOIP_SOCKET_DOMAIN_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_socket_type.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;



NEOIP_STRTYPE_DECLARATION_START(socket_domain_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_domain_t	, UDP)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_domain_t	, NTUDP)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_domain_t	, NTLAY)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_domain_t	, TCP)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_domain_t	, STCP)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(socket_domain_t)

	bool is_udp()	const throw() { return get_value() == UDP;	}
	bool is_ntudp()	const throw() { return get_value() == NTUDP;	}
	bool is_ntlay()	const throw() { return get_value() == NTLAY;	}
	bool is_tcp()	const throw() { return get_value() == TCP;	}
	bool is_stcp()	const throw() { return get_value() == STCP;	}

	// TODO to change for the external non inlined function
	bool	support(const socket_type_t &socket_type)	const throw()
	{
	switch( get_value() ){
	case UDP:
			if( socket_type == socket_type_t::DGRAM )	return true;
			if( socket_type == socket_type_t::RDM )		return true;
			if( socket_type == socket_type_t::SEQPACKET )	return true;
// TODO nlay doesnt fully support stream 
//			if( socket_type == socket_type_t::STREAM )	return true;
			return false;
	case NTLAY:
			if( socket_type == socket_type_t::DGRAM )	return true;
			if( socket_type == socket_type_t::RDM )		return true;
			if( socket_type == socket_type_t::SEQPACKET )	return true;
// TODO nlay doesnt fully support stream 
//			if( socket_type == socket_type_t::STREAM )	return true;
			return false;
	case NTUDP:
			if( socket_type == socket_type_t::DGRAM )	return true;
			return false;
	case TCP:
			if( socket_type == socket_type_t::STREAM )	return true;
			return false;
	case STCP:
			if( socket_type == socket_type_t::STREAM )	return true;
			return false;
	default:	DBG_ASSERT( 0 );
	}
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );
	return false;
	}

NEOIP_STRTYPE_DECLARATION_END(socket_domain_t	, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_DOMAIN_HPP__  */





