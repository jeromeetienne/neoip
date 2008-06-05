/*! \file
    \brief Header of the dnsgrab_arg_t

*/


#ifndef __NEOIP_DNSGRAB_ARG_HPP__ 
#define __NEOIP_DNSGRAB_ARG_HPP__ 
/* system include */
/* local include */
#include "neoip_argpack.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_delay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DECL_CLASS_BEGIN(dnsgrab_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_STUB(dnsgrab_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(dnsgrab_arg_t)
NEOIP_ARGPACK_DECL_ITEM(dnsgrab_arg_t		, ipport_addr_t	, listen_addr)
NEOIP_ARGPACK_DECL_ITEM(dnsgrab_arg_t		, std::string	, reg_location)
NEOIP_ARGPACK_DECL_ITEM(dnsgrab_arg_t		, size_t	, reg_priority)
NEOIP_ARGPACK_DECL_ITEM(dnsgrab_arg_t		, delay_t	, reg_request_ttl)
NEOIP_ARGPACK_DECL_ITEM(dnsgrab_arg_t		, std::string	, reg_tag)
NEOIP_ARGPACK_DECL_ITEM(dnsgrab_arg_t		, std::string	, reg_nonce)
NEOIP_ARGPACK_DECL_CLASS_END(dnsgrab_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DNSGRAB_ARG_HPP__  */






