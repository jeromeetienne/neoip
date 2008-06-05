/*! \file
    \brief Implementation of the \ref ntudp_sock_errcode_t

\par Brief Description
\ref ntudp_sock_errcode_t are the error code used with the ntudp_sock_pkttype_t::ERROR

*/

/* system include */
/* local include */
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(ntudp_sock_errcode_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_errcode_t	, UNREACH_PEERID	 , "UNREACH_PEERID")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_errcode_t	, UNREACH_PORTID	 , "UNREACH_PORTID")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_errcode_t	, ALREADY_BOUND		 , "ALREADY_BOUND")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_errcode_t	, UNKNOWN_CNX		 , "UNKNOWN_CNX")
NEOIP_STRTYPE_DEFINITION_END(ntudp_sock_errcode_t)

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the ntudp_sock_pkttype_t::ERROR packet
 */
pkt_t	ntudp_sock_errcode_build_pkt(const ntudp_sock_errcode_t &sock_errcode
					, const ntudp_nonce_t &client_nonce)	throw()
{
	pkt_t		pkt;
	// put the packet type
	pkt << ntudp_sock_pkttype_t(ntudp_sock_pkttype_t::ERROR_PKT);
	// put the nonce
	pkt << client_nonce;
	// put the error type
	pkt << sock_errcode;
	// return the packet
	return pkt;
}

NEOIP_NAMESPACE_END






