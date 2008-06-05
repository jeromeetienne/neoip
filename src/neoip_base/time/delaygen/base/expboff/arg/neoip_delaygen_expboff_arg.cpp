/*! \file
    \brief Definition of the delaygen_expboff_arg_t

*/

/* system include */
/* local include */
#include "neoip_delaygen_expboff_arg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(delaygen_expboff_arg_t)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(delaygen_expboff_arg_t	, delay_t	, first_delay	, delay_t::from_sec(0))
NEOIP_ARGPACK_DEF_ITEM(delaygen_expboff_arg_t		, delay_t	, min_delay)
NEOIP_ARGPACK_DEF_ITEM(delaygen_expboff_arg_t		, delay_t	, max_delay)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(delaygen_expboff_arg_t	, float		, multiplicator	, 2.0)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(delaygen_expboff_arg_t	, float		, random_range	, 0.0)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(delaygen_expboff_arg_t	, delay_t	, timeout_delay	, delay_t(delay_t::INFINITE_VAL))
NEOIP_ARGPACK_DEF_CLASS_END(delaygen_expboff_arg_t)

/** \brief Check the validity of the object
 */
bool	delaygen_expboff_arg_t::is_valid()	const throw()
{
	// if the random_range is not between [0,1], return an error
	if( random_range() < 0.0 || random_range() > 1.0 )	return false;
	// return no error
	return true;
}


NEOIP_NAMESPACE_END

