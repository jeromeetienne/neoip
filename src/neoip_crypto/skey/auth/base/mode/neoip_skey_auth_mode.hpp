/*! \file
    \brief Header of the \ref skey_auth_mode_t

*/


#ifndef __NEOIP_SKEY_AUTH_MODE_HPP__ 
#define __NEOIP_SKEY_AUTH_MODE_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(skey_auth_mode_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_mode_t	, HMAC)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_mode_t	, NOKEY)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(skey_auth_mode_t)
NEOIP_STRTYPE_DECLARATION_END(skey_auth_mode_t	, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_AUTH_MODE_HPP__  */





