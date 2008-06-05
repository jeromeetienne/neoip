/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_BT_CHECK_POLICY_HPP__ 
#define __NEOIP_BT_CHECK_POLICY_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_check_policy_t		, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_check_policy_t		, EVERY_PIECE)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_check_policy_t		, PARTIAL_PIECE)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_check_policy_t		, SUBFILE_EXIST)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_check_policy_t)
NEOIP_STRTYPE_DECLARATION_END(bt_check_policy_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CHECK_POLICY_HPP__  */



