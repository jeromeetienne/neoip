/*! \file
    \brief Header of the \ref neoip_bt_pselect_policy.hpp
*/


#ifndef __NEOIP_BT_PSELECT_POLICY_HPP__ 
#define __NEOIP_BT_PSELECT_POLICY_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_pselect_policy_t		, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_pselect_policy_t		, FIXED)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_pselect_policy_t		, SLIDE)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_pselect_policy_t)
NEOIP_STRTYPE_DECLARATION_END(bt_pselect_policy_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PSELECT_POLICY_HPP__  */



