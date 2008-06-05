/*! \file
    \brief Header of the \ref skey_ciph_algo_t

*/


#ifndef __NEOIP_SKEY_CIPH_ALGO_HPP__ 
#define __NEOIP_SKEY_CIPH_ALGO_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// TODO remove pkttype and only use strtype
NEOIP_STRTYPE_DECLARATION_START(skey_ciph_algo_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, DES)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, EDE_3DES)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, CAST5)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, BLOWFISH)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, TWOFISH128)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, TWOFISH256)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, AES)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, AES192)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, AES256)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_algo_t	, ARCFOUR)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(skey_ciph_algo_t)
NEOIP_STRTYPE_DECLARATION_END(skey_ciph_algo_t	, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_CIPH_ALGO_HPP__  */





