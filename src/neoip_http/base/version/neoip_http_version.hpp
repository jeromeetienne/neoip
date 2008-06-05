/*! \file
    \brief Header of the \ref http_version_t

*/


#ifndef __NEOIP_HTTP_VERSION_HPP__ 
#define __NEOIP_HTTP_VERSION_HPP__ 

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(http_version_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(http_version_t	, V1_0)
NEOIP_STRTYPE_DECLARATION_ITEM(http_version_t	, V1_1)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(http_version_t)
NEOIP_STRTYPE_DECLARATION_END(http_version_t, int)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_VERSION_HPP__  */

