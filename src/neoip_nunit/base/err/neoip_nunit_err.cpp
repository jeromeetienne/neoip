/*! \file
    \brief Definition of the \ref nunit_err_t

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_nunit_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(nunit_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(nunit_err_t	, OK			, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nunit_err_t	, ERROR			, "ERROR" )
NEOIP_ERRTYPE_DEFINITION_ITEM(nunit_err_t	, DELAYED		, "DELAYED" )
NEOIP_ERRTYPE_DEFINITION_END(nunit_err_t)

NEOIP_NAMESPACE_END





