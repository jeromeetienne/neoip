/*! \file
    \brief Declaration of the nonce for the scnx layer

*/


#ifndef __NEOIP_SCNX_NONCEID_HPP__ 
#define __NEOIP_SCNX_NONCEID_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_GEN_ID_DECLARATION_START	(scnx_nonceid_t	, skey_auth_algo_t::SHA1, 8);
NEOIP_GEN_ID_DECLARATION_END	(scnx_nonceid_t	, skey_auth_algo_t::SHA1, 8);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_NONCEID_HPP__  */



