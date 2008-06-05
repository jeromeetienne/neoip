/*! \file
    \brief Header of the \ref neoip_crypto_skey_auth_algo.cpp

*/


#ifndef __NEOIP_SCNX_ERR_HPP__ 
#define __NEOIP_SCNX_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


NEOIP_ERRTYPE_DECLARATION_START(scnx_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BOGUS_PKT)		// when a packet has an invalid format
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BAD_NONCE)		// when a estacnx's pkt contains an invalid nonce.
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BAD_COOKIE)		// when a estacnx's pkt contains an invalid cookie
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, ACL_REFUSED)		// when a estacnx's pkt is refused due to acl
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BAD_SIGNATURE)	// when a estacnx's pkt contains an invalid signature
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BAD_POP)		// when a pop of estacnx is invalid
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BAD_ITOR_STATE)	// when a packet is received by a itor but it isnt in the proper state
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BAD_AUXNEGO)		// when an error occurs during the auxilary negociation
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, CIPH_NEGO_FAILED)	// when it is impossible to find a match for full_ciph in estacnx
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, AUTH_NEGO_FAILED)	// when it is impossible to find a match for full_auth in estacnx
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BAD_FULL_CIPH)	// when a fullcnx's pkt cant be decrypted
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, BAD_FULL_AUTH)	// when a fullcnx's pkt cant be authenticated
NEOIP_ERRTYPE_DECLARATION_ITEM(scnx_err_t	, REPLAYED_PKT)		// when a fullcnx's pkt has been already received
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(scnx_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(scnx_err_t)


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_SCNX_ERR_HPP__  */


