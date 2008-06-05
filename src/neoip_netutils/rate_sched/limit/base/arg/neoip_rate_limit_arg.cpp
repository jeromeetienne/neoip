/*! \file
    \brief Definition of the rate_limit_arg_t

*/

/* system include */
/* local include */
#include "neoip_rate_limit_arg.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(rate_limit_arg_t)
NEOIP_ARGPACK_DEF_ITEM(rate_limit_arg_t	, rate_sched_t *	, rate_sched)
NEOIP_ARGPACK_DEF_ITEM(rate_limit_arg_t	, rate_prec_t		, rate_prec)
NEOIP_ARGPACK_DEF_ITEM(rate_limit_arg_t	, size_t		, absrate_max)
NEOIP_ARGPACK_DEF_ITEM(rate_limit_arg_t	, rate_limit_profile_t	, profile)
NEOIP_ARGPACK_DEF_CLASS_END(rate_limit_arg_t)

/** \brief Check the validity of the object
 */
bool	rate_limit_arg_t::is_valid()	const throw()
{
	// check for all the mandatory paramters
	if( !rate_sched_present() )	return false;
	if( !rate_prec_present() )	return false;
	// return no error
	return true;
}

NEOIP_NAMESPACE_END

