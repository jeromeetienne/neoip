/*! \file
    \brief Header of the \ref flv_amf_type.hpp
*/


#ifndef __NEOIP_FLV_AMF_TYPE_HPP__ 
#define __NEOIP_FLV_AMF_TYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_amf_type_t		, -1)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, NUMBER	, 0x00)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, BOOLEAN	, 0x01)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, STRING	, 0x02)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, OBJECT	, 0x03)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, UNDEFINED	, 0x06)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, MIXEDARRAY	, 0x08)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, ENDOFOBJECT	, 0x09)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, ARRAY		, 0x0a)
NEOIP_STRTYPE_DECLARATION_ITEM_IDX(flv_amf_type_t	, DATE		, 0x0b)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_amf_type_t)
NEOIP_STRTYPE_DECLARATION_END(flv_amf_type_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_AMF_TYPE_HPP__  */



