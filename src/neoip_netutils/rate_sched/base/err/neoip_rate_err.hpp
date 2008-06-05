/*! \file
    \brief Header of the \ref rate_err_t

*/


#ifndef __NEOIP_RATE_ERR_HPP__ 
#define __NEOIP_RATE_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DECLARATION_START(rate_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(rate_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(rate_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(rate_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(rate_err_t)

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_RATE_ERR_HPP__  */
