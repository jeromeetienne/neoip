/*! \file
    \brief Declaration of the ntudp_nonce_t
    
\par Brief Description
\ref ntudp_nonce_t is used by the requested to 'authenticate' the replied
packets. This is used at various place in the ntudp layer

\par Principle description:
- all request contains a nonce chosen by the initiator
- this nonce MUST be copied in the reply by the responder
- the initiator MUST NOT accept a reply which contains an invalid nonce
=> this prevent an attacker from being off-path.
   - on-path attackers can still provides forged replies but in ntudp, it
     produces 'only' a DoS (i.e. denial of Service) so it is ok as it is 
     impossible to prevent on-path attackers from doing DoS on the current internet.

*/


#ifndef __NEOIP_NONCE_HPP__ 
#define __NEOIP_NONCE_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_GEN_ID_DECLARATION_START	(ntudp_nonce_t, skey_auth_algo_t::MD5, 16);
NEOIP_GEN_ID_DECLARATION_END	(ntudp_nonce_t, skey_auth_algo_t::MD5, 16);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NONCE_HPP__  */



