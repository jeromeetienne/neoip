/*! \file
    \brief Implementation of the \ref upnp_sockfam_t
*/

/* system include */
/* local include */
#include "neoip_upnp_sockfam.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(upnp_sockfam_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(upnp_sockfam_t	, TCP	, "TCP")
NEOIP_STRTYPE_DEFINITION_ITEM(upnp_sockfam_t	, UDP	, "UDP")
NEOIP_STRTYPE_DEFINITION_END(upnp_sockfam_t)

NEOIP_NAMESPACE_END




