/*! \file
    \brief Header of the \ref router_err_t

*/


#ifndef __NEOIP_ROUTER_ERR_HPP__ 
#define __NEOIP_ROUTER_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_err_t;
class	netif_err_t;
class	socket_err_t;
class	dnsgrab_err_t;
class	ntudp_err_t;
class	file_err_t;
class	crypto_err_t;

NEOIP_ERRTYPE_DECLARATION_START(router_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, FROM_KAD)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, FROM_NETIF)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, FROM_SOCKET)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, FROM_DNSGRAB)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, FROM_NTUDP)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, FROM_FILE)
NEOIP_ERRTYPE_DECLARATION_ITEM(router_err_t	, FROM_CRYPTO)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(router_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(router_err_t)

// convert a various type of error into a router_err_t
router_err_t router_err_from_kad(const kad_err_t &kad_err, const std::string &prefix_str="")		throw();
router_err_t router_err_from_netif(const netif_err_t &netif_err, const std::string &prefix_str="")	throw();
router_err_t router_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str="")	throw();
router_err_t router_err_from_dnsgrab(const dnsgrab_err_t &dnsgrab_err, const std::string &prefix_str="")throw();
router_err_t router_err_from_ntudp(const ntudp_err_t &ntudp_err, const std::string &prefix_str="")	throw();
router_err_t router_err_from_file(const file_err_t &file_err, const std::string &prefix_str="")		throw();
router_err_t router_err_from_crypto(const crypto_err_t &crypto_err, const std::string &prefix_str="")	throw();


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_ROUTER_ERR_HPP__  */



