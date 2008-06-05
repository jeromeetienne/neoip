/*! \file
    \brief Header of the xmlrpc_client_helper_t class
    
*/


#ifndef __NEOIP_XMLRPC_CLIENT_HELPER_HPP__ 
#define __NEOIP_XMLRPC_CLIENT_HELPER_HPP__ 
/* system include */
#include <stdio.h>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	xmlrpc_err_t;
class	datum_t;

/** \brief static helpers for libneoip_cast
 */
class xmlrpc_client_helper_t {
public:
	static xmlrpc_err_t	has_succeed(const xmlrpc_err_t &xmlrpc_err
						, const datum_t &xmlrpc_resp)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_CLIENT_HELPER_HPP__  */



