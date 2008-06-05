/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_BT_JAMSTD_RESP_STATE_HPP__ 
#define __NEOIP_BT_JAMSTD_RESP_STATE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_jamstd_resp_state_t		, 0		)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_jamstd_resp_state_t		, CNXESTA_WAIT	)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_jamstd_resp_state_t		, CNXAUTH_WAIT	)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_jamstd_resp_state_t		, PADAUTH_WAIT	)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_jamstd_resp_state_t		, INITPKT_WAIT	)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_jamstd_resp_state_t)
NEOIP_STRTYPE_DECLARATION_END(bt_jamstd_resp_state_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMSTD_RESP_STATE_HPP__  */



