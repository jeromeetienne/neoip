/*! \file
    \brief Header of the \ref slay_domain_t

*/

/* system include */
/* local include */
#include "neoip_slay_domain.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(slay_domain_t,uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(slay_domain_t, TLS	, "TLS"		)
NEOIP_STRTYPE_DEFINITION_ITEM(slay_domain_t, BTJAMSTD	, "BTJAMSTD"	)
NEOIP_STRTYPE_DEFINITION_ITEM(slay_domain_t, BTJAMRC4	, "BTJAMRC4"	)
NEOIP_STRTYPE_DEFINITION_END(slay_domain_t)


NEOIP_NAMESPACE_END

