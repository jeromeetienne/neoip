/*! \file
    \brief Header of the \ref flv_parse_state_t
*/


#ifndef __NEOIP_FLV_PARSE_STATE_HPP__ 
#define __NEOIP_FLV_PARSE_STATE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(flv_parse_state_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(flv_parse_state_t	, TOPHD)
NEOIP_STRTYPE_DECLARATION_ITEM(flv_parse_state_t	, TAG)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(flv_parse_state_t)
NEOIP_STRTYPE_DECLARATION_END(flv_parse_state_t	, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_PARSE_STATE_HPP__  */



