/*! \file
    \brief Header of the socket_itor_arg_t

*/


#ifndef __NEOIP_SOCKET_ITOR_ARG_HPP__ 
#define __NEOIP_SOCKET_ITOR_ARG_HPP__ 
/* system include */

/* local include */
#include "neoip_argpack.hpp"
#include "neoip_namespace.hpp"

// include for the defined type
#include "neoip_socket_domain.hpp"
#include "neoip_socket_type.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_profile.hpp"


NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DECL_CLASS_BEGIN(socket_itor_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_FCT(socket_itor_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(socket_itor_arg_t)
NEOIP_ARGPACK_DECL_ITEM(socket_itor_arg_t	, socket_profile_t	, profile)
NEOIP_ARGPACK_DECL_ITEM(socket_itor_arg_t	, socket_domain_t	, domain)
NEOIP_ARGPACK_DECL_ITEM(socket_itor_arg_t	, socket_type_t		, type)
NEOIP_ARGPACK_DECL_ITEM(socket_itor_arg_t	, socket_addr_t		, local_addr)
NEOIP_ARGPACK_DECL_ITEM(socket_itor_arg_t	, socket_addr_t		, remote_addr)
NEOIP_ARGPACK_DECL_CLASS_END(socket_itor_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ITOR_ARG_HPP__  */






