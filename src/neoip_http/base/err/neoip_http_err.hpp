/*! \file
    \brief Header of the \ref http_err_t

*/


#ifndef __NEOIP_HTTP_ERR_HPP__ 
#define __NEOIP_HTTP_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	inet_err_t;
class	socket_err_t;

NEOIP_ERRTYPE_DECLARATION_START(http_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(http_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(http_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(http_err_t	, FROM_INET)
NEOIP_ERRTYPE_DECLARATION_ITEM(http_err_t	, FROM_SOCKET)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(http_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(http_err_t)

// convert a various type of error into a http_err_t
http_err_t http_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str="")	throw();
http_err_t http_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str="")throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_HTTP_ERR_HPP__  */
