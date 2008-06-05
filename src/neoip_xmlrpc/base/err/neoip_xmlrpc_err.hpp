/*! \file
    \brief Header of the \ref xmlrpc_err_t

*/


#ifndef __NEOIP_XMLRPC_ERR_HPP__ 
#define __NEOIP_XMLRPC_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_err_t;

NEOIP_ERRTYPE_DECLARATION_START(xmlrpc_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(xmlrpc_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(xmlrpc_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(xmlrpc_err_t	, FROM_HTTP)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(xmlrpc_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(xmlrpc_err_t)

// convert a various type of error into a xmlrpc_err_t
xmlrpc_err_t xmlrpc_err_from_http(const http_err_t &http_err, const std::string &prefix_str="")	throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_XMLRPC_ERR_HPP__  */



