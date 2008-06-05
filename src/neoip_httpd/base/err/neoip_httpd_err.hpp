/*! \file
    \brief Header of the \ref httpd_err_t

*/


#ifndef __NEOIP_HTTPD_ERR_HPP__ 
#define __NEOIP_HTTPD_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DECLARATION_START(httpd_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(httpd_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(httpd_err_t	, NOT_FOUND)
NEOIP_ERRTYPE_DECLARATION_ITEM(httpd_err_t	, BOGUS_REQUEST)
NEOIP_ERRTYPE_DECLARATION_ITEM(httpd_err_t	, MOVED_TEMP)
NEOIP_ERRTYPE_DECLARATION_ITEM(httpd_err_t	, INTERNAL_ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(httpd_err_t	, DELAYED_REPLY)
NEOIP_ERRTYPE_DECLARATION_ITEM(httpd_err_t	, SEND_FILE)
NEOIP_ERRTYPE_DECLARATION_ITEM(httpd_err_t	, UNAUTHORIZED)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(httpd_err_t)
	bool	is_error()	const throw()
		{ return get_value() == NOT_FOUND
			|| get_value() == BOGUS_REQUEST
			|| get_value() == INTERNAL_ERROR;	}
NEOIP_ERRTYPE_DECLARATION_END(httpd_err_t)


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_HTTPD_ERR_HPP__  */
