/*! \flv
    \brief Header of the \ref neoip_crypto_skey_auth_algo.cpp

*/


#ifndef __NEOIP_FLV_ERR_HPP__ 
#define __NEOIP_FLV_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	socket_err_t;

NEOIP_ERRTYPE_DECLARATION_START(flv_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(flv_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(flv_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(flv_err_t	, FROM_SOCKET)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(flv_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}	
NEOIP_ERRTYPE_DECLARATION_END(flv_err_t)

// convert a various type of error into a flv_err_t
flv_err_t flv_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str="")throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_FLV_ERR_HPP__  */


