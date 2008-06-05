/*! \file
    \brief Header of the \ref flv_soundsize.hpp
*/


#ifndef __NEOIP_FLV_SOUNDSIZE_HPP__ 
#define __NEOIP_FLV_SOUNDSIZE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_soundsize_t	, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundsize_t	, SIZE_8_BIT	, 0x00)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_soundsize_t	, SIZE_16_BIT	, 0x01)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_soundsize_t)
NEOIP_STRTYPE_DECLARATION_END(flv_soundsize_t		, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_SOUNDSIZE_HPP__  */



