/*! \file
    \brief Definition of the \ref clineopt_err_t

*/

/* system include */
/* local include */
#include "neoip_clineopt_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DEFINITION_START(clineopt_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(clineopt_err_t	, OK		, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(clineopt_err_t	, UNKNOWN_OPTION, "UNKNOWN_OPTION" )
NEOIP_ERRTYPE_DEFINITION_ITEM(clineopt_err_t	, MISSING_OPTION, "MISSING_OPTION" )
NEOIP_ERRTYPE_DEFINITION_ITEM(clineopt_err_t	, INVALID_PARAM	, "INVALID_PARAM" )
NEOIP_ERRTYPE_DEFINITION_ITEM(clineopt_err_t	, NO_PARAM	, "NO_PARAM" )
NEOIP_ERRTYPE_DEFINITION_END(clineopt_err_t)


NEOIP_NAMESPACE_END





