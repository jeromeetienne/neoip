/*! \file
    \brief Header of the \ref flv_tagtype.hpp
*/


#ifndef __NEOIP_FLV_TAGTYPE_HPP__ 
#define __NEOIP_FLV_TAGTYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_tagtype_t		, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_tagtype_t	, AUDIO		, 0x08)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_tagtype_t	, VIDEO		, 0x09)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_tagtype_t	, META		, 0x12)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_tagtype_t)
NEOIP_STRTYPE_DECLARATION_END(flv_tagtype_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_TAGTYPE_HPP__  */



