/*! \file
    \brief Header of the \ref rtmp_type.hpp
*/


#ifndef __NEOIP_RTMP_TYPE_HPP__
#define __NEOIP_RTMP_TYPE_HPP__
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(rtmp_type_t	, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_type_t	, PING		, 0x04)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_type_t	, CLIENTBW	, 0x05)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_type_t	, SERVERBW	, 0x06)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_type_t	, AUDIO		, 0x08)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_type_t	, VIDEO		, 0x09)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_type_t	, META		, 0x12)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(rtmp_type_t	, INVOKE	, 0x14)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(rtmp_type_t)
NEOIP_STRTYPE_DECLARATION_END(rtmp_type_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_TYPE_HPP__  */


