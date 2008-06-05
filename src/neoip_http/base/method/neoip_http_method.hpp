/*! \file
    \brief Header of the \ref http_method_t

*/


#ifndef __NEOIP_HTTP_METHOD_HPP__ 
#define __NEOIP_HTTP_METHOD_HPP__ 

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(http_method_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(http_method_t	, GET)
NEOIP_STRTYPE_DECLARATION_ITEM(http_method_t	, POST)
NEOIP_STRTYPE_DECLARATION_ITEM(http_method_t	, HEAD)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(http_method_t)
	bool is_get()	const throw() { return get_value() == GET;	}
	bool is_post()	const throw() { return get_value() == POST;	}
	bool is_head()	const throw() { return get_value() == HEAD;	}
NEOIP_STRTYPE_DECLARATION_END(http_method_t, int)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_METHOD_HPP__  */

