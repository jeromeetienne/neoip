/*! \file
    \brief Header of the \ref clineopt_err_t

*/


#ifndef __NEOIP_CLINEOPT_ERR_HPP__ 
#define __NEOIP_CLINEOPT_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DECLARATION_START(clineopt_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(clineopt_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(clineopt_err_t	, UNKNOWN_OPTION)
NEOIP_ERRTYPE_DECLARATION_ITEM(clineopt_err_t	, MISSING_OPTION)
NEOIP_ERRTYPE_DECLARATION_ITEM(clineopt_err_t	, INVALID_PARAM)
NEOIP_ERRTYPE_DECLARATION_ITEM(clineopt_err_t	, NO_PARAM)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(clineopt_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(clineopt_err_t)

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_CLINEOPT_ERR_HPP__  */



