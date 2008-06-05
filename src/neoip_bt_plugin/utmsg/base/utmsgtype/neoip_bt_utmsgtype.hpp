/*! \file
    \brief Header of the \ref bt_utmsgtype.hpp
*/


#ifndef __NEOIP_BT_UTMSGTYPE_HPP__ 
#define __NEOIP_BT_UTMSGTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_utmsgtype_t		, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_utmsgtype_t	, HANDSHAKE	, 0x0)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_utmsgtype_t	, UTPEX		, 0x1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_utmsgtype_t	, BYTEACCT	, 0x2)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_utmsgtype_t	, PUNAVAIL	, 0x3)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_utmsgtype_t	, PIECEWISH	, 0x4)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_utmsgtype_t	, BCAST		, 0x5)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_utmsgtype_t	, FSTART	, 0x6)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_utmsgtype_t)
NEOIP_STRTYPE_DECLARATION_END(bt_utmsgtype_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSGTYPE_HPP__  */



