/*! \file
    \brief Implementation of the \ref ntudp_tunl_pkttype_t

\par Brief Description
\ref ntudp_tunl_pkttype_t are the packet type sent over the tunnel between
\ref ntudp_pserver_t and \ref ntudp_relpeer_t.

*/

/* system include */
/* local include */
#include "neoip_ntudp_tunl_pkttype.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_datum.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(ntudp_tunl_pkttype_t	, ntudp_pkttype_t)
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_tunl_pkttype_t	, REGISTER_REQUEST	, "REGISTER_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_tunl_pkttype_t	, REGISTER_REPLY	, "REGISTER_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_tunl_pkttype_t	, PKT_SRV2CLI		, "PKT_SRV2CLI")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_tunl_pkttype_t	, PKT_CLI2SRV		, "PKT_CLI2SRV")
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_tunl_pkttype_t	, PKT_EXT2SRV		, "PKT_EXT2SRV")
NEOIP_STRTYPE_DEFINITION_END(ntudp_tunl_pkttype_t)

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the ntudp_tunl_pkttype_t::PKT_EXT2SRV packet
 */
pkt_t	ntudp_tunl_pkttype_build_pkt_ext2srv(const ntudp_nonce_t &nonce, const ntudp_peerid_t &dest_peerid
							, const datum_t &datum)	throw()
{
	pkt_t		pkt;
	// put the packet type
	pkt << ntudp_tunl_pkttype_t(ntudp_tunl_pkttype_t::PKT_EXT2SRV);
	// put the nonce
	pkt << nonce;
	// put the destination peerid
	pkt << dest_peerid;
	// put the datum to forward thru the server
	pkt << datum;
	// return the packet
	return pkt;
}
NEOIP_NAMESPACE_END






