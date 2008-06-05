/*! \file
    \brief Header of the \ref slay_err_t

*/


#ifndef __NEOIP_SLAY_ERR_HPP__ 
#define __NEOIP_SLAY_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class crypto_err_t;
class bt_err_t;

NEOIP_ERRTYPE_DECLARATION_START(slay_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(slay_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(slay_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(slay_err_t	, FROM_CRYPTO)
NEOIP_ERRTYPE_DECLARATION_ITEM(slay_err_t	, FROM_BT)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(slay_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(slay_err_t)

// convert a various type of error into a slay_err_t
slay_err_t slay_err_from_crypto(const crypto_err_t &crypto_err, const std::string &prefix_str="")	throw();
slay_err_t slay_err_from_bt(const bt_err_t &bt_err, const std::string &prefix_str="")	throw();

NEOIP_NAMESPACE_END



#endif	/* __NEOIP_SLAY_ERR_HPP__  */



