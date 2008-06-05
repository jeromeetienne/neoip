/*! \file
    \brief Header of the \ref socket_type_t

*/


#ifndef __NEOIP_SOCKET_TYPE_HPP__ 
#define __NEOIP_SOCKET_TYPE_HPP__ 

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_nlay_type.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(socket_type_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_type_t	, DGRAM)	//! datagram (unreliable message of fixed maximum length)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_type_t	, RDM)		//! realiable datagram (unordered reliable message of fixed maximum length)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_type_t	, SEQPACKET)	//! datagram (ordered reliable message of fixed maximum length)
NEOIP_STRTYPE_DECLARATION_ITEM(socket_type_t	, STREAM)	//! sequenced,  reliable,  two-way,  connection-based byte stream
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(socket_type_t)
public:
	bool	is_dgram()	const throw()	{ return get_value() == DGRAM; 		}
	bool	is_rdm()	const throw()	{ return get_value() == RDM;		}
	bool	is_seqpacket()	const throw()	{ return get_value() == SEQPACKET;	}
	bool	is_stream()	const throw()	{ return get_value() == STREAM;		}

	/** \brief return true is the socket type is reliable
	 */
	bool	is_reliable()	const throw() {
		return is_rdm() || is_seqpacket() || is_stream();
	}
	/** \brief return true is the socket type is ordered
	 */
	bool	is_ordered()	const throw() {
		return is_seqpacket() || is_stream();
	}
	/** \brief return true is the socket type is ordered
	 */
	bool	is_datagram()	const throw() {
		return is_rdm() || is_seqpacket() || is_dgram();
	}

	/** \brief convert a socket_type_t into a nlay_type_t
	 */
	nlay_type_t to_nlay()			const throw(){
		switch( get_value() ){
		case NONE:	return nlay_type_t::NONE;
		case DGRAM:	return nlay_type_t::DGRAM;
		case RDM:	return nlay_type_t::RDM;
		case SEQPACKET:	return nlay_type_t::SEQPACKET;
		case STREAM:	return nlay_type_t::STREAM;
		default:	DBG_ASSERT(0);
		}
		// NOTE: this point MUST never be reached
		DBG_ASSERT( 0 );
		return nlay_type_t::NONE;
	}
NEOIP_STRTYPE_DECLARATION_END(socket_type_t, int)

// convert a nlay_type_t into a socket_type_t
socket_type_t socket_type_from_nlay(const nlay_type_t &nlay_type)	throw();


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_TYPE_HPP__  */

