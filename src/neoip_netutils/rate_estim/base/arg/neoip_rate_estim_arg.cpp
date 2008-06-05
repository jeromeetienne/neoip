/*! \file
    \brief Definition of the rate_estim_arg_t

*/

/* system include */
/* local include */
#include "neoip_rate_estim_arg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(rate_estim_arg_t)
NEOIP_ARGPACK_DEF_ITEM(rate_estim_arg_t	, delay_t	, total_delay)
NEOIP_ARGPACK_DEF_ITEM(rate_estim_arg_t	, delay_t	, slice_delay)
NEOIP_ARGPACK_DEF_CLASS_END(rate_estim_arg_t)

NEOIP_NAMESPACE_END

