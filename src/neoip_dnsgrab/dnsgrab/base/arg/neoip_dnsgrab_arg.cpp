/*! \file
    \brief Definition of the dnsgrab_arg_t

*/

/* system include */
/* local include */
#include "neoip_dnsgrab_arg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(dnsgrab_arg_t)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(dnsgrab_arg_t	, ipport_addr_t	, listen_addr	, "127.0.0.1")
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(dnsgrab_arg_t	, std::string	, reg_location	, "user")
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(dnsgrab_arg_t	, size_t	, reg_priority	, 500)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(dnsgrab_arg_t	, delay_t	, reg_request_ttl, delay_t::from_sec(60))
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(dnsgrab_arg_t	, std::string	, reg_tag	, "")
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(dnsgrab_arg_t	, std::string	, reg_nonce	, "")
NEOIP_ARGPACK_DEF_CLASS_END(dnsgrab_arg_t)

NEOIP_NAMESPACE_END

