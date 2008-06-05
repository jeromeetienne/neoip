/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_BT_REQAUTH_TYPE_HPP__ 
#define __NEOIP_BT_REQAUTH_TYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_reqauth_type_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_reqauth_type_t	, DENY)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_reqauth_type_t	, COOP)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_reqauth_type_t	, HOPE)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_reqauth_type_t	, GIVE)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_reqauth_type_t	, FSTART)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_reqauth_type_t	, IDLE)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_reqauth_type_t)
NEOIP_STRTYPE_DECLARATION_END(bt_reqauth_type_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_REQAUTH_TYPE_HPP__  */



