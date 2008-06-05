/*! \file
    \brief Definition of the nlay_type_t
*/


/* system include */
/* local include */
#include "neoip_nlay_type.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(nlay_type_t,int)
NEOIP_STRTYPE_DEFINITION_ITEM(nlay_type_t, DGRAM	, "DGRAM" )
NEOIP_STRTYPE_DEFINITION_ITEM(nlay_type_t, RDM		, "RDM" )
NEOIP_STRTYPE_DEFINITION_ITEM(nlay_type_t, SEQPACKET	, "SEQPACKET" )
NEOIP_STRTYPE_DEFINITION_ITEM(nlay_type_t, STREAM	, "STREAM" )
NEOIP_STRTYPE_DEFINITION_END(nlay_type_t)


NEOIP_NAMESPACE_END

