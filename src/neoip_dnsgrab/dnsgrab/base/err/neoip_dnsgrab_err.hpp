/*! \file
    \brief Header of the \ref neoip_crypto_skey_auth_algo.cpp

*/


#ifndef __NEOIP_DNSGRAB_ERR_HPP__ 
#define __NEOIP_DNSGRAB_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class inet_err_t;
class file_err_t;

NEOIP_ERRTYPE_DECLARATION_START(dnsgrab_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(dnsgrab_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(dnsgrab_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(dnsgrab_err_t	, FROM_INET)
NEOIP_ERRTYPE_DECLARATION_ITEM(dnsgrab_err_t	, FROM_FILE)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(dnsgrab_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}	
NEOIP_ERRTYPE_DECLARATION_END(dnsgrab_err_t)


// convert a various type of error into a dnsgrab_err_t
dnsgrab_err_t dnsgrab_err_from_inet(const inet_err_t &inet_err, const std::string &prefix_str="")throw();
dnsgrab_err_t dnsgrab_err_from_file(const file_err_t &file_err, const std::string &prefix_str="")throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_DNSGRAB_ERR_HPP__  */


