/*! \file
    \brief Definition of the slay_itor_arg_t

*/

/* system include */
/* local include */
#include "neoip_slay_itor_arg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(slay_itor_arg_t)
NEOIP_ARGPACK_DEF_ITEM(slay_itor_arg_t	, slay_profile_t	, profile)
NEOIP_ARGPACK_DEF_ITEM(slay_itor_arg_t	, slay_domain_t		, domain)
NEOIP_ARGPACK_DEF_CLASS_END(slay_itor_arg_t)

/** \brief Check the validity of the object
 */
bool	slay_itor_arg_t::is_valid()	const throw()
{
	// check for all the mandatory paramters
	if( !domain_present() )		return false;
	// return no error
	return true;
}

NEOIP_NAMESPACE_END

