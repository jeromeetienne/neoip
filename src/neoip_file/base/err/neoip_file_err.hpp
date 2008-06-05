/*! \file
    \brief Header of the \ref neoip_crypto_skey_auth_algo.cpp

*/


#ifndef __NEOIP_FILE_ERR_HPP__ 
#define __NEOIP_FILE_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DECLARATION_START(file_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(file_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(file_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(file_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}	
NEOIP_ERRTYPE_DECLARATION_END(file_err_t)

// convert a various type of error into a socket_err_t
file_err_t file_err_from_errno(const std::string &prefix_str="")	throw();


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_FILE_ERR_HPP__  */


