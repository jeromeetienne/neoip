/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_CLINEOPT_MODE_HPP__ 
#define __NEOIP_CLINEOPT_MODE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(clineopt_mode_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(clineopt_mode_t	, NOTHING)
NEOIP_STRTYPE_DECLARATION_ITEM(clineopt_mode_t	, OPTIONAL)
NEOIP_STRTYPE_DECLARATION_ITEM(clineopt_mode_t	, REQUIRED)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(clineopt_mode_t)
NEOIP_STRTYPE_DECLARATION_END(clineopt_mode_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CLINEOPT_MODE_HPP__  */



