/*! \file
    \brief Header of the \ref neoip_crypto_skey_auth_algo.cpp

*/


#ifndef __NEOIP_PKTFRAG_ERR_HPP__ 
#define __NEOIP_PKTFRAG_ERR_HPP__ 

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DECLARATION_START(pktfrag_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(pktfrag_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(pktfrag_err_t	, BOGUS_PKT)
NEOIP_ERRTYPE_DECLARATION_ITEM(pktfrag_err_t	, FRAGMENT_TOO_LARGE)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(pktfrag_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(pktfrag_err_t)



NEOIP_NAMESPACE_END


#endif	/* __NEOIP_PKTFRAG_ERR_HPP__  */



