/*! \file
    \brief Header of the \ref neoip_crypto_skey_auth_algo.cpp

*/


#ifndef __NEOIP_SOCKET_ERR_HPP__ 
#define __NEOIP_SOCKET_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class nlay_err_t;
class inet_err_t;
class ntudp_err_t;
class crypto_err_t;
class slay_err_t;

NEOIP_ERRTYPE_DECLARATION_START(socket_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, BAD_PARAM)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, BAD_ADDR)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, FROM_NLAY)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, FROM_INET)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, FROM_NTUDP)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, FROM_CRYPTO)
NEOIP_ERRTYPE_DECLARATION_ITEM(socket_err_t	, FROM_SLAY)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(socket_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(socket_err_t)

// convert a various type of error into a socket_err_t
socket_err_t socket_err_from_nlay(const nlay_err_t &nlay_err, const std::string &prefix_str="")		throw();
socket_err_t socket_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str="")		throw();
socket_err_t socket_err_from_ntudp(const ntudp_err_t &ntudp_err, const std::string &prefix_str="")	throw();
socket_err_t socket_err_from_crypto(const crypto_err_t &crypto_err, const std::string &prefix_str="")	throw();
socket_err_t socket_err_from_slay(const slay_err_t &slay_err, const std::string &prefix_str="")	throw();

NEOIP_NAMESPACE_END



#endif	/* __NEOIP_SOCKET_ERR_HPP__  */



