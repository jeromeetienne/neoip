/*! \file
    \brief Header of the \ref libsess_err_t

*/


#ifndef __NEOIP_LIBSESS_ERR_HPP__ 
#define __NEOIP_LIBSESS_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	file_err_t;
class	clineopt_err_t;
class	xmlrpc_err_t;
class	http_err_t;

NEOIP_ERRTYPE_DECLARATION_START(libsess_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(libsess_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(libsess_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(libsess_err_t	, FROM_INET)
NEOIP_ERRTYPE_DECLARATION_ITEM(libsess_err_t	, FROM_CLINEOPT)
NEOIP_ERRTYPE_DECLARATION_ITEM(libsess_err_t	, FROM_XMLRPC)
NEOIP_ERRTYPE_DECLARATION_ITEM(libsess_err_t	, FROM_HTTP)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(libsess_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(libsess_err_t)

// convert a various type of error into a libsess_err_t
libsess_err_t libsess_err_from_file(const file_err_t &file_err, const std::string &prefix_str="")	throw();
libsess_err_t libsess_err_from_clineopt(const clineopt_err_t &clineopt_err, const std::string &prefix_str="")	throw();
libsess_err_t libsess_err_from_xmlrpc(const xmlrpc_err_t &xmlrpc_err, const std::string &prefix_str="")throw();
libsess_err_t libsess_err_from_http(const http_err_t &http_err, const std::string &prefix_str="")throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_LIBSESS_ERR_HPP__  */
