/*! \file
    \brief Header of the \ref flv_frametype.hpp
*/


#ifndef __NEOIP_FLV_FRAMETYPE_HPP__ 
#define __NEOIP_FLV_FRAMETYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_frametype_t		, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_frametype_t	, KEY			, 0x01)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_frametype_t	, INTER			, 0x02)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_frametype_t	, INTER_DISPOSABLE	, 0x03)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_frametype_t)
NEOIP_STRTYPE_DECLARATION_END(flv_frametype_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_FRAMETYPE_HPP__  */



