/*! \file
    \brief Header of the \ref http_scheme_t

*/


#ifndef __NEOIP_HTTP_SCHEME_HPP__
#define __NEOIP_HTTP_SCHEME_HPP__

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(http_scheme_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(http_scheme_t	, HTTP)
NEOIP_STRTYPE_DECLARATION_ITEM(http_scheme_t	, HTTPS)
NEOIP_STRTYPE_DECLARATION_ITEM(http_scheme_t	, RTMP)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(http_scheme_t)
	bool is_http()	const throw() { return get_value() == HTTP;	}
	bool is_https()	const throw() { return get_value() == HTTPS;	}
	bool is_rtmp()	const throw() { return get_value() == RTMP;	}
	uint16_t default_port()	const throw()
	{
		if( is_http() )		return 80;
		else if( is_https())	return 443;
		else if( is_rtmp())	return 1935;
		else 			DBG_ASSERT(0);
		return 0x4242;
	}
NEOIP_STRTYPE_DECLARATION_END(http_scheme_t, int)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SCHEME_HPP__  */

