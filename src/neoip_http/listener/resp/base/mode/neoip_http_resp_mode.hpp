/*! \file
    \brief Header of the \ref http_resp_mode_t

*/


#ifndef __NEOIP_HTTP_RESP_MODE_HPP__ 
#define __NEOIP_HTTP_RESP_MODE_HPP__ 

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(http_resp_mode_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(http_resp_mode_t	, ACCEPT_SUBPATH)
NEOIP_STRTYPE_DECLARATION_ITEM(http_resp_mode_t	, REJECT_SUBPATH)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(http_resp_mode_t)
NEOIP_STRTYPE_DECLARATION_END(http_resp_mode_t, int)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_RESP_MODE_HPP__  */

