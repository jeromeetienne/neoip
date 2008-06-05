/*! \file
    \brief Declaration of the ntudp_peerid_t
    
\par Brief Description
\ref ntudp_id_t is the "identificator" of a peer in ntudp.

- This module is based on the gen_id template. so there is no associated .cpp

*/


#ifndef __NEOIP_NTUDP_PEERID_HPP__ 
#define __NEOIP_NTUDP_PEERID_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_GEN_ID_DECLARATION_START	(ntudp_peerid_t, skey_auth_algo_t::MD5, 16);
NEOIP_GEN_ID_DECLARATION_END	(ntudp_peerid_t, skey_auth_algo_t::MD5, 16);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PEERID_HPP__  */



