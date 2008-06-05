/*! \file
    \brief Definition of the http_status_t
    
- for the list of all status codes
  - http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html

*/


/* system include */
/* local include */
#include "neoip_http_status.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(http_status_t,int)
NEOIP_STRTYPE_DEFINITION_ITEM(http_status_t, OK			, "OK" 			)
NEOIP_STRTYPE_DEFINITION_ITEM(http_status_t, PARTIAL_CONTENT	, "PARTIAL_CONTENT"	)
NEOIP_STRTYPE_DEFINITION_ITEM(http_status_t, MOVED_PERMANENTLY	, "MOVED_PERMANENTLY"	)
NEOIP_STRTYPE_DEFINITION_ITEM(http_status_t, BAD_REQUEST	, "BAD_REQUEST"		)
NEOIP_STRTYPE_DEFINITION_ITEM(http_status_t, UNAUTHORIZED	, "UNAUTHORIZED"	)
NEOIP_STRTYPE_DEFINITION_ITEM(http_status_t, NOT_FOUND		, "NOT_FOUND"		)
NEOIP_STRTYPE_DEFINITION_ITEM(http_status_t, SERVICE_UNAVAILABLE, "SERVICE_UNAVAILABLE" )
NEOIP_STRTYPE_DEFINITION_END(http_status_t)

NEOIP_NAMESPACE_END

