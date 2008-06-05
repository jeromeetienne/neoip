/*! \file
    \brief Header of the rate_limit_arg_t

*/


#ifndef __NEOIP_RATE_LIMIT_ARG_HPP__ 
#define __NEOIP_RATE_LIMIT_ARG_HPP__ 
/* system include */

/* local include */
#include "neoip_argpack.hpp"
#include "neoip_namespace.hpp"

// include for the defined type
#include "neoip_rate_prec.hpp"
#include "neoip_rate_limit_profile.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rate_sched_t;

NEOIP_ARGPACK_DECL_CLASS_BEGIN(rate_limit_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_FCT(rate_limit_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(rate_limit_arg_t)
NEOIP_ARGPACK_DECL_ITEM_PTR(rate_limit_arg_t	, rate_sched_t *	, rate_sched)
NEOIP_ARGPACK_DECL_ITEM(rate_limit_arg_t	, rate_prec_t		, rate_prec)
NEOIP_ARGPACK_DECL_ITEM(rate_limit_arg_t	, size_t		, absrate_max)
NEOIP_ARGPACK_DECL_ITEM(rate_limit_arg_t	, rate_limit_profile_t	, profile)
NEOIP_ARGPACK_DECL_CLASS_END(rate_limit_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_LIMIT_ARG_HPP__  */






