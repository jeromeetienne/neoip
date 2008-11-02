/*! \file
    \brief Header of the \ref flv_soundtype.hpp
*/


#ifndef __NEOIP_FLV_SOUNDTYPE_HPP__ 
#define __NEOIP_FLV_SOUNDTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_soundtype_t	, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundtype_t	, MONO		, 0x00)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundtype_t	, STEREO	, 0x01)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_soundtype_t)
NEOIP_STRTYPE_DECLARATION_END(flv_soundtype_t		, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_SOUNDTYPE_HPP__  */



