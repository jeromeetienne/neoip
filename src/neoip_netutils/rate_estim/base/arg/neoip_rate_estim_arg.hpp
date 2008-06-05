/*! \file
    \brief Header of the rate_estim_arg_t

*/


#ifndef __NEOIP_RATE_ESTIM_ARG_HPP__ 
#define __NEOIP_RATE_ESTIM_ARG_HPP__ 
/* system include */
/* local include */
#include "neoip_argpack.hpp"
#include "neoip_namespace.hpp"

// include for the defined type
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rate_estim_cb_t;

NEOIP_ARGPACK_DECL_CLASS_BEGIN(rate_estim_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_STUB(rate_estim_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(rate_estim_arg_t)
NEOIP_ARGPACK_DECL_ITEM(rate_estim_arg_t	, delay_t	, total_delay)
NEOIP_ARGPACK_DECL_ITEM(rate_estim_arg_t	, delay_t	, slice_delay)
NEOIP_ARGPACK_DECL_CLASS_END(rate_estim_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_ESTIM_ARG_HPP__  */






