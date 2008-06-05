/*! \file
    \brief Definition of the \ref inet_err_t

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_inet_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(inet_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(inet_err_t	, OK			, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(inet_err_t	, ERROR			, "ERROR" )
NEOIP_ERRTYPE_DEFINITION_ITEM(inet_err_t	, BAD_PARAMETER		, "BAD_PARAMETER" )
NEOIP_ERRTYPE_DEFINITION_ITEM(inet_err_t	, SYSTEM_ERR		, "SYSTEM_ERR" )
NEOIP_ERRTYPE_DEFINITION_END(inet_err_t)

NEOIP_NAMESPACE_END





