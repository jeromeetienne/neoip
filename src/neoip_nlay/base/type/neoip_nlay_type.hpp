/*! \file
    \brief Header of the \ref nlay_type_t

*/


#ifndef __NEOIP_NLAY_TYPE_HPP__ 
#define __NEOIP_NLAY_TYPE_HPP__ 

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(nlay_type_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_type_t	, DGRAM)	//! datagram (unreliable message of fixed maximum length)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_type_t	, RDM)		//! realiable datagram (unordered reliable message of fixed maximum length)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_type_t	, SEQPACKET)	//! datagram (ordered reliable message of fixed maximum length)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_type_t	, STREAM)	//! sequenced,  reliable,  two-way,  connection-based byte stream
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(nlay_type_t)
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
NEOIP_STRTYPE_DECLARATION_END(nlay_type_t, int)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_TYPE_HPP__  */

