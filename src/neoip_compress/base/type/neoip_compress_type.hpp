/*! \file
    \brief Header of the \ref compress_type_t

*/


#ifndef __NEOIP_COMPRESS_TYPE_HPP__ 
#define __NEOIP_COMPRESS_TYPE_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(compress_type_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(compress_type_t	, NULL_COMP)
NEOIP_STRTYPE_DECLARATION_ITEM(compress_type_t	, DEFLATE)
NEOIP_STRTYPE_DECLARATION_ITEM(compress_type_t	, GZIP)
NEOIP_STRTYPE_DECLARATION_ITEM(compress_type_t	, ZLIB)
NEOIP_STRTYPE_DECLARATION_ITEM(compress_type_t	, BZIP)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(compress_type_t)
NEOIP_STRTYPE_DECLARATION_END(compress_type_t, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COMPRESS_TYPE_HPP__  */





