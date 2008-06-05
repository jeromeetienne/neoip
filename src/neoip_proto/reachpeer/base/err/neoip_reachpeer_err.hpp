/*! \file
    \brief Header of the \ref neoip_crypto_skey_auth_algo.cpp

*/


#ifndef __NEOIP_REACHPEER_ERR_HPP__ 
#define __NEOIP_REACHPEER_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DECLARATION_START(reachpeer_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(reachpeer_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(reachpeer_err_t	, BOGUS_PKT)
NEOIP_ERRTYPE_DECLARATION_ITEM(reachpeer_err_t	, BAD_PARAM)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(reachpeer_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}	
NEOIP_ERRTYPE_DECLARATION_END(reachpeer_err_t)


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_REACHPEER_ERR_HPP__  */


