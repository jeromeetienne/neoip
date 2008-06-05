/*! \file
    \brief Definition of the \ref pktfrag_err_t

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_pktfrag_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(pktfrag_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(pktfrag_err_t, OK			, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(pktfrag_err_t, BOGUS_PKT		, "BOGUS PKT" )
NEOIP_ERRTYPE_DEFINITION_ITEM(pktfrag_err_t, FRAGMENT_TOO_LARGE	, "FRAGMENT TOO LARGE" )
NEOIP_ERRTYPE_DEFINITION_END(pktfrag_err_t)

NEOIP_NAMESPACE_END





