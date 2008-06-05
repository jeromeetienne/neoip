/*! \file
    \brief Implementation of the \ref scnx_pkttype_t
*/

/* system include */
/* local include */
#include "neoip_scnx_pkttype.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(scnx_pkttype_t	, nlay_pkttype_t)
NEOIP_STRTYPE_DEFINITION_ITEM(scnx_pkttype_t	, CNX_REQUEST	, "CNX_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(scnx_pkttype_t	, CNX_REPLY	, "CNX_REPLY")
NEOIP_STRTYPE_DEFINITION_ITEM(scnx_pkttype_t	, ITOR_AUTH	, "ITOR_AUTH")
NEOIP_STRTYPE_DEFINITION_ITEM(scnx_pkttype_t	, RESP_AUTH	, "RESP_AUTH")
NEOIP_STRTYPE_DEFINITION_ITEM(scnx_pkttype_t	, SEC_DGRAM	, "SEC_DGRAM" )
NEOIP_STRTYPE_DEFINITION_ITEM(scnx_pkttype_t	, REKEY_REQUEST	, "REKEY_REQUEST")
NEOIP_STRTYPE_DEFINITION_ITEM(scnx_pkttype_t	, REKEY_REPLY	, "REKEY_REPLY")
NEOIP_STRTYPE_DEFINITION_END(scnx_pkttype_t)

NEOIP_NAMESPACE_END




