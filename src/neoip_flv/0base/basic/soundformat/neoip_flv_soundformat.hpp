/*! \file
    \brief Header of the \ref flv_soundformat.hpp
*/


#ifndef __NEOIP_FLV_SOUNDFORMAT_HPP__ 
#define __NEOIP_FLV_SOUNDFORMAT_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_soundformat_t	, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundformat_t	, UNCOMPRESSED		, 0x00)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundformat_t	, ADPCM			, 0x01)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundformat_t	, MP3			, 0x02)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundformat_t	, NELLYMOSER_8KHZ_MONO	, 0x05)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundformat_t	, NELLYMOSER		, 0x06)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_soundformat_t)
NEOIP_STRTYPE_DECLARATION_END(flv_soundformat_t		, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_SOUNDFORMAT_HPP__  */



