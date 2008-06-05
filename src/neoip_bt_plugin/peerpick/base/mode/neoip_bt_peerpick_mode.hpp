/*! \file
    \brief Header of the \ref bt_peerpick_mode_t
*/


#ifndef __NEOIP_BT_PEERPICK_MODE_HPP__ 
#define __NEOIP_BT_PEERPICK_MODE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_peerpick_mode_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_peerpick_mode_t	, PLAIN)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_peerpick_mode_t	, RELAY)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_peerpick_mode_t	, CASTI)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_peerpick_mode_t	, CASTO)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_peerpick_mode_t)
NEOIP_STRTYPE_DECLARATION_END(bt_peerpick_mode_t	, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERPICK_MODE_HPP__  */



