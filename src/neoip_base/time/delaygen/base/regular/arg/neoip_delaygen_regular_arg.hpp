/*! \file
    \brief Header of the delaygen_regular_arg_t

*/


#ifndef __NEOIP_DELAYGEN_REGULAR_ARG_HPP__ 
#define __NEOIP_DELAYGEN_REGULAR_ARG_HPP__ 
/* system include */

/* local include */
#include "neoip_argpack.hpp"
#include "neoip_delay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DECL_CLASS_BEGIN(delaygen_regular_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_FCT(delaygen_regular_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(delaygen_regular_arg_t)
NEOIP_ARGPACK_DECL_ITEM(delaygen_regular_arg_t		, delay_t	, first_delay)
NEOIP_ARGPACK_DECL_ITEM(delaygen_regular_arg_t		, delay_t	, period)
NEOIP_ARGPACK_DECL_ITEM(delaygen_regular_arg_t		, float		, random_range)
NEOIP_ARGPACK_DECL_ITEM(delaygen_regular_arg_t		, delay_t	, timeout_delay)
NEOIP_ARGPACK_DECL_CLASS_END(delaygen_regular_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAYGEN_REGULAR_ARG_HPP__  */






