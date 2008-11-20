/*! \file
    \brief Header of the \ref rtmp_resp_cnx_state_t
*/


#ifndef __NEOIP_RTMP_RESP_CNX_STATE_HPP__
#define __NEOIP_RTMP_RESP_CNX_STATE_HPP__
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(rtmp_resp_cnx_state_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(rtmp_resp_cnx_state_t	, WAITING_ITORSYN)
NEOIP_STRTYPE_DECLARATION_ITEM(rtmp_resp_cnx_state_t	, WAITING_ITORACK)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(rtmp_resp_cnx_state_t)
NEOIP_STRTYPE_DECLARATION_END(rtmp_resp_cnx_state_t	, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_RESP_CNX_STATE_HPP__  */



