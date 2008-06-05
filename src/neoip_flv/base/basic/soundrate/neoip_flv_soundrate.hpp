/*! \file
    \brief Header of the \ref flv_soundrate.hpp
*/


#ifndef __NEOIP_FLV_SOUNDRATE_HPP__ 
#define __NEOIP_FLV_SOUNDRATE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_soundrate_t	, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundrate_t	, RATE_5_5KHZ	, 0x00)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundrate_t	, RATE_11_KHZ	, 0x01)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundrate_t	, RATE_22_KHZ	, 0x02)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundrate_t	, RATE_44_KHZ	, 0x03)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_soundrate_t)
NEOIP_STRTYPE_DECLARATION_END(flv_soundrate_t		, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_SOUNDRATE_HPP__  */



