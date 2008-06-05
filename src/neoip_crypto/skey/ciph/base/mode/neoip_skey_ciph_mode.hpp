/*! \file
    \brief Header of the \ref skey_ciph_mode_t

*/


#ifndef __NEOIP_SKEY_CIPH_MODE_HPP__ 
#define __NEOIP_SKEY_CIPH_MODE_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// TODO remove pkttype and only use strtype
NEOIP_STRTYPE_DECLARATION_START(skey_ciph_mode_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_mode_t	, ECB)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_mode_t	, CTR)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_mode_t	, CBC)
NEOIP_STRTYPE_DECLARATION_ITEM(skey_ciph_mode_t	, STREAM)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(skey_ciph_mode_t)
NEOIP_STRTYPE_DECLARATION_END(skey_ciph_mode_t	, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_CIPH_MODE_HPP__  */





