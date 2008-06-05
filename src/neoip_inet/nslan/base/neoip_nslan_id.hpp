/*! \file
    \brief definition for nslan_keyid_t and nslan_realmid_t
*/


#ifndef __NEOIP_NSLAN_ID_HPP__ 
#define __NEOIP_NSLAN_ID_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_GEN_ID_DECLARATION_START	(nslan_keyid_t	, skey_auth_algo_t::SHA1, 20);
NEOIP_GEN_ID_DECLARATION_END	(nslan_keyid_t	, skey_auth_algo_t::SHA1, 20);

NEOIP_GEN_ID_DECLARATION_START	(nslan_realmid_t, skey_auth_algo_t::SHA1, 20);
NEOIP_GEN_ID_DECLARATION_END	(nslan_realmid_t, skey_auth_algo_t::SHA1, 20);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NSLAN_ID_HPP__  */



