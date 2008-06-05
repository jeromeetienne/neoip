/*! \file
    \brief Header of the \ref ntudp_err_t

*/


#ifndef __NEOIP_NTUDP_ERR_HPP__ 
#define __NEOIP_NTUDP_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_err_t;
class	inet_err_t;
class	netif_err_t;
class	ndiag_err_t;

NEOIP_ERRTYPE_DECLARATION_START(ntudp_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(ntudp_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(ntudp_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(ntudp_err_t	, ALREADY_BOUND)
NEOIP_ERRTYPE_DECLARATION_ITEM(ntudp_err_t	, FROM_KAD)
NEOIP_ERRTYPE_DECLARATION_ITEM(ntudp_err_t	, FROM_INET)
NEOIP_ERRTYPE_DECLARATION_ITEM(ntudp_err_t	, FROM_NETIF)
NEOIP_ERRTYPE_DECLARATION_ITEM(ntudp_err_t	, FROM_NDIAG)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(ntudp_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(ntudp_err_t)

// convert a various type of error into a ntudp_err_t
ntudp_err_t ntudp_err_from_kad(const kad_err_t &kad_err, const std::string &prefix_str="")	throw();
ntudp_err_t ntudp_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str="")	throw();
ntudp_err_t ntudp_err_from_netif(const netif_err_t &netif_err, const std::string &prefix_str="")throw();
ntudp_err_t ntudp_err_from_ndiag(const ndiag_err_t &ndiag_err, const std::string &prefix_str="")	throw();


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_NTUDP_ERR_HPP__  */



