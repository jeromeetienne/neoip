/*! \file
    \brief Header of the tcp_itor_arg_t

*/


#ifndef __NEOIP_TCP_ITOR_ARG_HPP__ 
#define __NEOIP_TCP_ITOR_ARG_HPP__ 
/* system include */

/* local include */
#include "neoip_argpack.hpp"
#include "neoip_namespace.hpp"

// include for the defined type
#include "neoip_ipport_addr.hpp"
#include "neoip_tcp_profile.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DECL_CLASS_BEGIN(tcp_itor_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_FCT(tcp_itor_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(tcp_itor_arg_t)
NEOIP_ARGPACK_DECL_ITEM(tcp_itor_arg_t	, ipport_addr_t		, local_addr)
NEOIP_ARGPACK_DECL_ITEM(tcp_itor_arg_t	, ipport_addr_t		, remote_addr)
NEOIP_ARGPACK_DECL_ITEM(tcp_itor_arg_t	, tcp_profile_t		, profile)
NEOIP_ARGPACK_DECL_CLASS_END(tcp_itor_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_ITOR_ARG_HPP__  */






