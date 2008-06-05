/*! \file
    \brief Header of the slay_itor_arg_t

*/


#ifndef __NEOIP_SLAY_ITOR_ARG_HPP__ 
#define __NEOIP_SLAY_ITOR_ARG_HPP__ 
/* system include */

/* local include */
#include "neoip_argpack.hpp"
#include "neoip_namespace.hpp"

// include for the defined type
#include "neoip_slay_profile.hpp"
#include "neoip_slay_domain.hpp"


NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DECL_CLASS_BEGIN(slay_itor_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_FCT(slay_itor_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(slay_itor_arg_t)
NEOIP_ARGPACK_DECL_ITEM(slay_itor_arg_t	, slay_profile_t	, profile)
NEOIP_ARGPACK_DECL_ITEM(slay_itor_arg_t	, slay_domain_t		, domain)
NEOIP_ARGPACK_DECL_CLASS_END(slay_itor_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_ITOR_ARG_HPP__  */






