/*! \file
    \brief Header of the \ref oload_mod_type_t
*/


#ifndef __NEOIP_OLOAD_MOD_TYPE_HPP__ 
#define __NEOIP_OLOAD_MOD_TYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(oload_mod_type_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(oload_mod_type_t		, RAW)
NEOIP_STRTYPE_DECLARATION_ITEM(oload_mod_type_t		, FLV)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(oload_mod_type_t)
NEOIP_STRTYPE_DECLARATION_END(oload_mod_type_t	, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_MOD_TYPE_HPP__  */



