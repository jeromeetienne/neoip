/*! \file
    \brief Header of the \ref flv_codecid.hpp
*/


#ifndef __NEOIP_FLV_CODECID_HPP__ 
#define __NEOIP_FLV_CODECID_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_codecid_t		, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_codecid_t	, SORENSEN_H263	, 0x02)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_codecid_t	, SCREEN_VIDEO	, 0x03)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_codecid_t	, ON2_VP6	, 0x04)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_codecid_t)
NEOIP_STRTYPE_DECLARATION_END(flv_codecid_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_CODECID_HPP__  */



