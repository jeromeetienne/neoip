/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_BT_CMDTYPE_HPP__ 
#define __NEOIP_BT_CMDTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_cmdtype_t		, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, UNAUTH_REQ	, 0x0)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, DOAUTH_REQ	, 0x1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, DOWANT_REQ	, 0x2)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, UNWANT_REQ	, 0x3)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, PIECE_ISAVAIL	, 0x4)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, PIECE_BFIELD	, 0x5)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, BLOCK_REQ	, 0x6)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, BLOCK_REP	, 0x7)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, BLOCK_DEL	, 0x8)

// opcode for the mainline 'dht' - bt_protoflag_t::ML_DHT
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, ML_DHT_PORT	, 0x09)

// opcode for the mainline 'fast extension' - bt_protoflag_t::ML_FASTEXT
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, PIECE_SUGGEST	, 0x0d)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, HAVE_ALL	, 0x0e)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, HAVE_NONE	, 0x0f)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, REQ_REJECT	, 0x10)

// opcode for the libtorrent 'extension protocol' - bt_protoflag_t::UTMSG_PAYL
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, UTMSG_PAYL	, 0x14)


NEOIP_STRTYPE_DECLARATION_ITEM_IDX(bt_cmdtype_t		, KEEPALIVE	, 254)	// internal representation
										// it never goes on wire
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_cmdtype_t)
NEOIP_STRTYPE_DECLARATION_END(bt_cmdtype_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CMDTYPE_HPP__  */



