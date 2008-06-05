/*! \file
    \brief Header of the \ref tls_authtype_t

*/


#ifndef __NEOIP_TLS_AUTHTYPE_HPP__ 
#define __NEOIP_TLS_AUTHTYPE_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(tls_authtype_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(tls_authtype_t	, ANON)
NEOIP_STRTYPE_DECLARATION_ITEM(tls_authtype_t	, CERT)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(tls_authtype_t)
NEOIP_STRTYPE_DECLARATION_END(tls_authtype_t, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TLS_AUTHTYPE_HPP__  */





