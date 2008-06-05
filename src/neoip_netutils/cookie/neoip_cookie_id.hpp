/*! \file
    \brief Declaration of the id which contains the cookie

*/


#ifndef __NEOIP_COOKIE_ID_HPP__ 
#define __NEOIP_COOKIE_ID_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_GEN_ID_DECLARATION_START	(cookie_id_t	, skey_auth_algo_t::SHA1, 20);
NEOIP_GEN_ID_DECLARATION_END	(cookie_id_t	, skey_auth_algo_t::SHA1, 20);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COOKIE_ID_HPP__  */



