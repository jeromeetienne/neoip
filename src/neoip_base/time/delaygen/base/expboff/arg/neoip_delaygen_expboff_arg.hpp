/*! \file
    \brief Header of the delaygen_expboff_arg_t

*/


#ifndef __NEOIP_DELAYGEN_EXPBOFF_ARG_HPP__ 
#define __NEOIP_DELAYGEN_EXPBOFF_ARG_HPP__ 
/* system include */

/* local include */
#include "neoip_argpack.hpp"
#include "neoip_delay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DECL_CLASS_BEGIN(delaygen_expboff_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_FCT(delaygen_expboff_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(delaygen_expboff_arg_t)
NEOIP_ARGPACK_DECL_ITEM(delaygen_expboff_arg_t		, delay_t	, first_delay)
NEOIP_ARGPACK_DECL_ITEM(delaygen_expboff_arg_t		, delay_t	, min_delay)
NEOIP_ARGPACK_DECL_ITEM(delaygen_expboff_arg_t		, delay_t	, max_delay)
NEOIP_ARGPACK_DECL_ITEM(delaygen_expboff_arg_t		, float		, multiplicator)
NEOIP_ARGPACK_DECL_ITEM(delaygen_expboff_arg_t		, float		, random_range)
NEOIP_ARGPACK_DECL_ITEM(delaygen_expboff_arg_t		, delay_t	, timeout_delay)
NEOIP_ARGPACK_DECL_CLASS_END(delaygen_expboff_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAYGEN_EXPBOFF_ARG_HPP__  */






