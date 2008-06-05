/*! \file
    \brief Implementation of the \ref ntudp_sock_pkttype_t

\par Brief Description
\ref ntudp_sock_pkttype_t are the packet type used to establish the ntudp socket.
They are exchanged between ntudp_resp_t/ntudp_itor_t/ntudp_full_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(ntudp_sock_pkttype_t	, ntudp_pkttype_t)
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, DIRECT_CNX_REQUEST	, "DIRECT_CNX_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, DIRECT_CNX_REPLY	, "DIRECT_CNX_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, ESTARELAY_CNX_REQUEST	, "ESTARELAY_CNX_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, ESTARELAY_CNX_REPLY	, "ESTARELAY_CNX_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, ESTARELAY_CNX_I2R_ACK	, "ESTARELAY_CNX_I2R_ACK")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, ESTARELAY_CNX_R2I_ACK	, "ESTARELAY_CNX_R2I_ACK")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, REVERSE_CNX_REQUEST	, "REVERSE_CNX_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, REVERSE_CNX_REPLY	, "REVERSE_CNX_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, REVERSE_CNX_I2R_ACK	, "REVERSE_CNX_I2R_ACK")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, REVERSE_CNX_R2I_ACK	, "REVERSE_CNX_R2I_ACK")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_sock_pkttype_t	, ERROR_PKT		, "ERROR_PKT")
NEOIP_STRTYPE_DEFINITION_END(ntudp_sock_pkttype_t)


NEOIP_NAMESPACE_END






