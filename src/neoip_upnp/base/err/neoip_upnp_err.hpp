/*! \file
    \brief Header of the \ref upnp_err_t

*/


#ifndef __NEOIP_UPNP_ERR_HPP__ 
#define __NEOIP_UPNP_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	http_err_t;
class	inet_err_t;

NEOIP_ERRTYPE_DECLARATION_START(upnp_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(upnp_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(upnp_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(upnp_err_t	, SOAP_FAULT)
NEOIP_ERRTYPE_DECLARATION_ITEM(upnp_err_t	, FROM_HTTP)
NEOIP_ERRTYPE_DECLARATION_ITEM(upnp_err_t	, FROM_INET)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(upnp_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(upnp_err_t)

// convert a various type of error into a upnp_err_t
upnp_err_t upnp_err_from_http(const http_err_t &http_err, const std::string &prefix_str="")	throw();
upnp_err_t upnp_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str="")	throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_UPNP_ERR_HPP__  */
