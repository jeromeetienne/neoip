/*! \rtmp
    \brief Header of the \ref neoip_crypto_skey_auth_algo.cpp

*/


#ifndef __NEOIP_RTMP_ERR_HPP__
#define __NEOIP_RTMP_ERR_HPP__

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	socket_err_t;

NEOIP_ERRTYPE_DECLARATION_START(rtmp_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(rtmp_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(rtmp_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(rtmp_err_t	, FROM_SOCKET)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(rtmp_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(rtmp_err_t)

// convert a various type of error into a rtmp_err_t
rtmp_err_t rtmp_err_from_socket(const socket_err_t &socket_err, const std::string &prefix_str="")throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_RTMP_ERR_HPP__  */


