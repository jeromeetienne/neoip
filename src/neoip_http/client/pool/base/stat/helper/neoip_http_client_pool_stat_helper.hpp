/*! \file
    \brief Header of the http_client_pool_stat_helper_t class
    
*/


#ifndef __NEOIP_HTTP_CLIENT_POOL_STAT_HELPER_HPP__ 
#define __NEOIP_HTTP_CLIENT_POOL_STAT_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_client_pool_stat_t;

/** \brief static helpers to manipulate http_client_pool_stat_t
 */
class http_client_pool_stat_helper_t {
public:
	static	std::string	to_html(const http_client_pool_stat_t &pool_stat)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_CLIENT_POOL_STAT_HELPER_HPP__  */



