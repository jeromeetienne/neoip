/*! \file
    \brief Declaration of all the gen_id_t of the router layer

*/


#ifndef __NEOIP_ROUTER_ID_HPP__ 
#define __NEOIP_ROUTER_ID_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_GEN_ID_DECLARATION_START	(router_peerid_t	, skey_auth_algo_t::SHA1, 20);
NEOIP_GEN_ID_DECLARATION_END	(router_peerid_t	, skey_auth_algo_t::SHA1, 20);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ID_HPP__  */



