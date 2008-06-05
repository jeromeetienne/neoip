/*! \file
    \brief Header of the nlay_dataize_t

*/


#ifndef __NEOIP_NLAY_DATAIZE_HPP__
#define __NEOIP_NLAY_DATAIZE_HPP__

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_nlay_pkttype.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// packet type for this nlay_full_t
NEOIP_STRTYPE_DECLARATION_START(dataize_pkttype_t	, NEOIP_PKTTYPE_OFFSET_DATAIZE)
NEOIP_STRTYPE_DECLARATION_ITEM(dataize_pkttype_t	, DATAIZE)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(dataize_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(dataize_pkttype_t, nlay_pkttype_t)

#if NEOIP_PKTTYPE_DATAIZE_MAX_NB_PACKET != 16
#	error "invalid pkttype value!!! fix neoip_strtype.hpp constants."
#endif

/** \brief just a nlay_full_api_t to add a DATAIZE pkttype in packet from above the socket
 */
class nlay_dataize_t : public nlay_full_api_t {
private:
public:
	// function inherited from nlay_full_api_t
	void		register_handler(nlay_regpkt_t *regpkt_lower)	throw();
	nlay_err_t	pkt_from_upper(pkt_t &pkt)				throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();
	size_t		get_mtu_overhead()					const throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_dataize_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_DATAIZE_HPP__ 



