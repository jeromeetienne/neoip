/*! \file
    \brief Header of the \ref nlay_nego_pkttype_t

*/


#ifndef __NEOIP_NLAY_NEGO_PKTTYPE_HPP__ 
#define __NEOIP_NLAY_NEGO_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// declaration of the pkttype for the parameters negociations
NEOIP_STRTYPE_DECLARATION_START(nlay_nego_pkttype_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(nlay_nego_pkttype_t	, PKTCOMP_PAYL)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(nlay_nego_pkttype_t)
NEOIP_STRTYPE_DECLARATION_END(nlay_nego_pkttype_t, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_NEGO_PKTTYPE_HPP__  */



