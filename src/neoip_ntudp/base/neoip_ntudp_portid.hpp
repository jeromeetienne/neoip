/*! \file
    \brief Declaration of the ntudp_portid_t
    
\par Brief Description
\ref reludp_id_t is the "identificator" of a port in ntudp. 
It is a unique port for a given ntudp_peerid_t

- This module is based on the gen_id template. so there is no associated .cpp

*/


#ifndef __NEOIP_NTUDP_PORTID_HPP__ 
#define __NEOIP_NTUDP_PORTID_HPP__ 
/* system include */
/* local include */
#include "neoip_gen_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_GEN_ID_DECLARATION_START	(ntudp_portid_t, skey_auth_algo_t::MD5, 16);
NEOIP_GEN_ID_DECLARATION_END	(ntudp_portid_t, skey_auth_algo_t::MD5, 16);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PORTID_HPP__  */



