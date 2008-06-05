/*! \file
    \brief Header of the ns_packet
*/


#ifndef __NEOIP_SCNX_PKTTYPE_HPP__ 
#define __NEOIP_SCNX_PKTTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_nlay_pkttype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief determine if the scnx_full_t MUST add a packet type for data packet.
 * 
 * - if set to 1, the scnx's caller doesnt need to add a pkttype in data packet
 *   as scnx_full_t distingish them by itself by adding a SEC_DGRAM
 * - if set to 0, it is up to the scnx's caller to add a pkttype to the datapacket
 *   before sending it to scnx_full_t
 *   - moreover this pkttype MUST be compatible with scnx_pkttype_t in size and value
 *   - using scnx_full_t in the nlay layer, doesnt need a SEC_DGRAM and remove
 *     it to gain a byte of overhead per packet
 * - TODO to keep the INTERNAL_DATATYPE case seems complex for not much...
 *   - should i remove it from the code ?
 */
#define NEOIP_SCNX_FULL_INTERNAL_DATATYPE	0

#if NEOIP_SCNX_FULL_INTERNAL_DATATYPE
	NEOIP_STRTYPE_DECLARATION_START(scnx_pkttype_t	, 0)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, CNX_REQUEST)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, CNX_REPLY)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, ITOR_AUTH)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, RESP_AUTH)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, SEC_DGRAM)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, REKEY_REQUEST)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, REKEY_REPLY)
	NEOIP_STRTYPE_DECLARATION_ITEM_LAST(scnx_pkttype_t)
	NEOIP_STRTYPE_DECLARATION_END(scnx_pkttype_t	, uint8_t)
#else
	NEOIP_STRTYPE_DECLARATION_START(scnx_pkttype_t	, NEOIP_PKTTYPE_OFFSET_SCNX)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, CNX_REQUEST)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, CNX_REPLY)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, ITOR_AUTH)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, RESP_AUTH)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, SEC_DGRAM)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, REKEY_REQUEST)
	NEOIP_STRTYPE_DECLARATION_ITEM(scnx_pkttype_t	, REKEY_REPLY)
	NEOIP_STRTYPE_DECLARATION_ITEM_LAST(scnx_pkttype_t)
	NEOIP_STRTYPE_DECLARATION_END(scnx_pkttype_t	, nlay_pkttype_t)
#endif

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_PKTTYPE_HPP__  */



