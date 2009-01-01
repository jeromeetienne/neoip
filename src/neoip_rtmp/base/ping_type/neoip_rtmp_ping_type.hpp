/*! \file
    \brief Header of the \ref rtmp_ping_type.hpp
*/


#ifndef __NEOIP_RTMP_PING_TYPE_HPP__
#define __NEOIP_RTMP_PING_TYPE_HPP__
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(rtmp_ping_type_t	, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, CLEAR		, 0x00)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, PLAYBUFFERCLR	, 0x01)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, UNKNOWN2	, 0x02)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, CLIENTBUFFER	, 0x03)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, STREAMRESET	, 0x04)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, UNKNOWN5	, 0x05)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, PINGCLIENT	, 0x06)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, PONGSERVER	, 0x07)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_ping_type_t	, UNKNOWN8	, 0x08)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(rtmp_ping_type_t)
NEOIP_STRTYPE_DECLARATION_END(rtmp_ping_type_t		, uint16_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_PING_TYPE_HPP__  */



