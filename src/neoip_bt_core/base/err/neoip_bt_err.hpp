/*! \file
    \brief Header of the \ref bt_err_t

*/


#ifndef __NEOIP_BT_ERR_HPP__ 
#define __NEOIP_BT_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	http_err_t;
class	kad_err_t;
class	inet_err_t;
class	socket_err_t;
class	file_err_t;
class	rate_err_t;
class	xmlrpc_err_t;
class	ndiag_err_t;
class	flv_err_t;

NEOIP_ERRTYPE_DECLARATION_START(bt_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_HTTP)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_KAD)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_INET)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_SOCKET)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_FILE)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_RATE)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_XMLRPC)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_NDIAG)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_err_t	, FROM_FLV)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(bt_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(bt_err_t)

// convert a various type of error into a bt_err_t
bt_err_t bt_err_from_http(const http_err_t &http_err, const std::string &prefix_str="")	throw();
bt_err_t bt_err_from_kad(const kad_err_t &kad_err, const std::string &prefix_str="")		throw();
bt_err_t bt_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str="")		throw();
bt_err_t bt_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str="")	throw();
bt_err_t bt_err_from_file(const file_err_t &file_err, const std::string &prefix_str="")		throw();
bt_err_t bt_err_from_rate(const rate_err_t &rate_err, const std::string &prefix_str="")		throw();
bt_err_t bt_err_from_xmlrpc(const xmlrpc_err_t &xmlrpc_err, const std::string &prefix_str="")	throw();
bt_err_t bt_err_from_ndiag(const ndiag_err_t &ndiag_err, const std::string &prefix_str="")	throw();
bt_err_t bt_err_from_flv(const flv_err_t &flv_err, const std::string &prefix_str="")		throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_BT_ERR_HPP__  */
