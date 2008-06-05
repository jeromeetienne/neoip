/*! \file
    \brief Header of the \ref skey_auth_algo_t

*/


#ifndef __NEOIP_SKEY_AUTH_ALGO_HPP__ 
#define __NEOIP_SKEY_AUTH_ALGO_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(skey_auth_algo_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, SHA1)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, RMD160)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, MD5)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, MD4)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, MD2)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, TIGER)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, HAVAL)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, SHA256)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, SHA384)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, SHA512)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_auth_algo_t	, CRC32)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(skey_auth_algo_t)
NEOIP_STRTYPE_DECLARATION_END(skey_auth_algo_t	, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_AUTH_ALGO_HPP__  */





