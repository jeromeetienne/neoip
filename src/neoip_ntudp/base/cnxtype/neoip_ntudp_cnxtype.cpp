/*! \file
    \brief Definition of the \ref ntudp_cnxtype_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_cnxtype.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_STRTYPE_DEFINITION_START(ntudp_cnxtype_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_cnxtype_t	, DIRECT	, "DIRECT" )
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_cnxtype_t	, ESTARELAY	, "ESTARELAY" )
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_cnxtype_t	, REVERSE	, "REVERSE" )
NEOIP_STRTYPE_DEFINITION_ITEM(ntudp_cnxtype_t	, IMPOSSIBLE	, "IMPOSSIBLE" )
NEOIP_STRTYPE_DEFINITION_END(ntudp_cnxtype_t)

NEOIP_NAMESPACE_END





